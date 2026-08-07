#include "prspch.h"
#include "Pressure/Scene/Scene.h"

#include "Pressure/Core/Application.h"
#include "Pressure/Physics/Physics2D.h"
#include "Pressure/Renderer/Renderer2D.h"
#include "Pressure/Scene/Components.h"
#include "Pressure/Scene/Entity.h"
#include "Pressure/Scripting/ScriptEngine.h"
#include "Pressure/Scene/ScriptableEntity.h"

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace Pressure
{

	namespace
	{
		template<typename... Component>
		void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entityMap)
		{
			([&]
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntityID = entityMap.at(src.get<IDComponent>(srcEntity).ID);
					if constexpr (std::is_empty_v<Component>)
					{
						dst.emplace_or_replace<Component>(dstEntityID);
					}
					else
					{
						Component& component = src.get<Component>(srcEntity);
						dst.emplace_or_replace<Component>(dstEntityID, component);
					}
				}
			}(), ...);
		}

		template<typename... Component>
		void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entityMap)
		{
			CopyComponent<Component...>(dst, src, entityMap);
		}

		template<typename... Component>
		void CopyComponentIfExists(Entity dst, Entity src)
		{
			([&]
			{
				if (src.HasComponent<Component>())
				{
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
					dst.OnComponentAddedOrReplaced<Component>(src, dst.GetComponent<Component>());
				}
			}(), ...);
		}

		template<typename... Component>
		void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
		{
			CopyComponentIfExists<Component...>(dst, src);
		}
	}

    Scene::Scene()
		: m_PhysicsImpl(new PhysicsWorldImpl())
    {
		m_EntityQuadTree = CreateScope<QuadTree<Entity>>(glm::vec3(0.0f), 100.0f);
    }

    Scene::~Scene()
    {
		delete m_PhysicsImpl;
    }

    Entity Scene::CreateEntity(const std::string& name/* = std::string()*/)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name/* = std::string()*/)
    {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;
  
		return entity;
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
		PRS_CORE_ASSERT(m_EntityMap.find(uuid) != m_EntityMap.end());
		return { m_EntityMap[uuid], this };
    }

    Entity Scene::FindEntityByName(std::string_view name)
    {
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			if (view.get<TagComponent>(entity).Tag == name)
			{
				return { entity, this };
			}
		}
		return {};
    }

    Entity Scene::DuplicateEntity(Entity entity)
    {
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		if (!m_IsRunning && newEntity.HasComponent<RigidBody2DComponent>())
			InstantiatePhysicsBody(newEntity);

		return newEntity;
    }

    void Scene::DestroyEntity(Entity entity)
	{
		if (m_IsRunning)
		{
			m_PendingDestroyEntities.push_back(entity);
			return;
		}

		DestroyEntityImmediate(entity);
	}

    void Scene::DestroyEntityImmediate(Entity entity)
    {
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
    }

    void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		// Scripting
		{
			ScriptEngine::OnRuntimeStart(this);

			// Register script entities
			auto view = m_Registry.view<ScriptComponent>(entt::exclude<DisabledComponent>);
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnRegisterEntity(entity);
			}

			// Instantiate script entities
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}

			// Instantiate native script entities
			const auto nscView = m_Registry.view<NativeScriptComponent>(entt::exclude<DisabledComponent>);
			nscView.each([this](auto entity, auto& nsc)
			{
				if (!m_Registry.valid(entity))
					return;

				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
			});
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		Application::Get().SetSpeed(1.0f);

		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::OnPhysics2DStart()
	{
		if (!m_Box2DPhysicsSimulationEnabled)
			return;

		b2WorldDef worldDefinition = b2DefaultWorldDef();
		worldDefinition.gravity = { 0.0f, -9.81f };
		m_PhysicsImpl->WorldId = b2CreateWorld(&worldDefinition);

		auto view = m_Registry.view<RigidBody2DComponent>(entt::exclude<DisabledComponent>);
		for (auto e : view)
		{
			Entity entity = { e, this };
			InstantiatePhysicsBody(entity);
		}
	}

	void Scene::OnPhysics2DUpdate(Timestep ts)
	{
		if (m_Box2DPhysicsSimulationEnabled)
		{
			constexpr int32_t subStepCount = 4;
			b2World_Step(m_PhysicsImpl->WorldId, ts, subStepCount);

			const auto view = m_Registry.view<RigidBody2DComponent>(entt::exclude<DisabledComponent>);
			for (const auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

				const b2BodyId body = rb2d.RuntimeBody->BodyId;
				const auto [x, y] = b2Body_GetPosition(body);
				transform.Translation.x = x;
				transform.Translation.y = y;
				auto [c, s] = b2Body_GetRotation(body);
				transform.Rotation.z = std::atan2(s, c);

				b2Body_SetGravityScale(body, rb2d.GravityScale);
			}

			// Collision detection forwarded to scripting
			b2ContactEvents contactEvents = b2World_GetContactEvents(m_PhysicsImpl->WorldId);
			for (int i = 0; i < contactEvents.hitCount; ++i)
			{
				b2ContactHitEvent hitEvent = contactEvents.hitEvents[i];
				b2BodyId bodyA = b2Shape_GetBody(hitEvent.shapeIdA);
				b2BodyId bodyB = b2Shape_GetBody(hitEvent.shapeIdB);
				UUID uuidA = reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyA));
				UUID uuidB = reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyB));

				if (m_EntityMap.find(uuidA) == m_EntityMap.end() || m_EntityMap.find(uuidB) == m_EntityMap.end())
					continue;

				Entity entityA = GetEntityByUUID(uuidA);
				Entity entityB = GetEntityByUUID(uuidB);
				PRS_CORE_ASSERT(entityA && entityB);
				PRS_CORE_ASSERT(entityA != entityB);

				if (entityA.HasComponent<ScriptComponent>())
				{
					ScriptEngine::OnCollision2D(entityA, entityB);
				}
				if (entityB.HasComponent<ScriptComponent>())
				{
					ScriptEngine::OnCollision2D(entityB, entityA);
				}
			}
		}
		else
		{
			// Circle colliders overlap detection forwarded to scripting.
			// Using a spatial partitioning quadtree structure to reduce the number of collision check.
			// Entities and their positions are re-added each frame to the QuadTree to cover for their movement.
			m_EntityQuadTree->Clear();

			auto view = m_Registry.view<TransformComponent, CircleCollider2DComponent>(entt::exclude<DisabledComponent>);
			for (const auto e : view)
			{
				Entity entity = { e, this };
				if (!m_EntityQuadTree->Add(entity))
				{
					// Likely not in visible area, so we can ignore it for collision detection
					PRS_CORE_TRACE("Entity {0} could not be added to the QuadTree", entity.GetUUID());
				}
			}

			std::vector<Entity> neighbours;
			neighbours.reserve(view.size_hint());
			for (const auto e : view)
			{
				Entity entity = { e, this };
				if (!entity.HasComponent<CircleCollider2DComponent, BoxCollider2DComponent>())
					continue;

				neighbours.clear();

				m_EntityQuadTree->QueryRange({ entity.GetCenter(), entity.GetHalfExtent() * 2.0f }, neighbours);
				for (auto& otherEntity : neighbours)
				{
					if (entity == otherEntity)
						continue;

					if (!CheckCollision(entity, otherEntity))
						continue;

					// Deferred collision resolution
					m_PendingCollisions.emplace_back(entity.GetUUID(), otherEntity.GetUUID());

					// Immediate collision resolution
					// if (entity.HasComponent<ScriptComponent>())
					// {
					// 	ScriptEngine::OnCollision2D(entity, otherEntity);
					// }
					// if (otherEntity.HasComponent<ScriptComponent>())
					// {
					// 	ScriptEngine::OnCollision2D(otherEntity, entity);
					// }
				}
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		if (!m_Box2DPhysicsSimulationEnabled)
			return;

		b2DestroyWorld(m_PhysicsImpl->WorldId);
		m_PhysicsImpl->WorldId = b2_nullWorldId;
		delete m_PhysicsImpl;
		m_PhysicsImpl = new PhysicsWorldImpl();

		auto view = m_Registry.view<RigidBody2DComponent>(entt::exclude<DisabledComponent>);
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			rb2d.RuntimeBody = nullptr;
		}
	}

	bool Scene::CheckCollision(Entity entity, Entity otherEntity)
	{
		const auto& tc = entity.GetComponent<TransformComponent>();
		const auto& otherTc = otherEntity.GetComponent<TransformComponent>();

		// Scale also affects the collider size, so we need to take it into account when checking for collisions
		if (entity.HasComponent<CircleCollider2DComponent>() && otherEntity.HasComponent<CircleCollider2DComponent>())
		{
			const auto& circleCollider = entity.GetComponent<CircleCollider2DComponent>();
			const auto& otherCircleCollider = otherEntity.GetComponent<CircleCollider2DComponent>();
			const float radius = circleCollider.Radius * std::max(tc.Scale.x, tc.Scale.y);
			const float otherRadius = otherCircleCollider.Radius * std::max(otherTc.Scale.x, otherTc.Scale.y);
			return Physics2D::CirclesOverlap(entity.GetCenter(), radius,
				otherEntity.GetCenter(), otherRadius);
		}

		if (entity.HasComponent<BoxCollider2DComponent>() && otherEntity.HasComponent<BoxCollider2DComponent>())
		{
			const auto& boxColliderSize = entity.GetComponent<BoxCollider2DComponent>().Size;
			const auto& otherBoxColliderSize = otherEntity.GetComponent<BoxCollider2DComponent>().Size;
			const glm::vec3 boxHalfExtent = { boxColliderSize.x * otherTc.Scale.x * 0.5f, boxColliderSize.y * otherTc.Scale.y * 0.5f, 0.0f };
			const glm::vec3 otherBoxHalfExtent = { otherBoxColliderSize.x * otherTc.Scale.x * 0.5f, otherBoxColliderSize.y * otherTc.Scale.y * 0.5f, 0.0f };
			return Physics2D::BoxesOverlap(entity.GetCenter(), boxHalfExtent,
				otherEntity.GetCenter(), otherBoxHalfExtent);
		}

		if (entity.HasComponent<CircleCollider2DComponent>() && otherEntity.HasComponent<BoxCollider2DComponent>())
		{
			const auto& circleCollider = entity.GetComponent<CircleCollider2DComponent>();
			const auto& otherBoxColliderSize = otherEntity.GetComponent<BoxCollider2DComponent>().Size;
			const float radius = circleCollider.Radius * std::max(tc.Scale.x, tc.Scale.y);
			const glm::vec3 boxHalfExtent = { otherBoxColliderSize.x * otherTc.Scale.x * 0.5f, otherBoxColliderSize.y * otherTc.Scale.y * 0.5f, 0.0f };
			return Physics2D::CircleBoxOverlap(entity.GetCenter(), radius,
				otherEntity.GetCenter(), boxHalfExtent);
		}

		if (entity.HasComponent<BoxCollider2DComponent>() && otherEntity.HasComponent<CircleCollider2DComponent>())
		{
			const auto& boxColliderSize = entity.GetComponent<BoxCollider2DComponent>().Size;
			const auto& otherCircleCollider = otherEntity.GetComponent<CircleCollider2DComponent>();
			const float otherRadius = otherCircleCollider.Radius * std::max(otherTc.Scale.x, otherTc.Scale.y);
			const glm::vec3 boxHalfExtent = { boxColliderSize.x * otherTc.Scale.x * 0.5f, boxColliderSize.y * otherTc.Scale.y * 0.5f, 0.0f };
			return Physics2D::CircleBoxOverlap(otherEntity.GetCenter(), otherRadius,
				entity.GetCenter(), boxHalfExtent);
		}

		return false;
	}

	void Scene::OnScriptEngineUpdate(Timestep ts)
	{
		const auto view = m_Registry.view<ScriptComponent>(entt::exclude<DisabledComponent>);
		const std::vector scriptEntities(view.begin(), view.end());
		for (const auto e : scriptEntities)
		{
			if (!m_Registry.valid(e))
				continue;

			const Entity entity = { e, this };
			ScriptEngine::OnUpdateEntity(entity, ts);
		}

		const auto nscView = m_Registry.view<NativeScriptComponent>(entt::exclude<DisabledComponent>);
		nscView.each([this, ts](auto entity, auto& nsc)
		{
			if (!m_Registry.valid(entity))
				return;

			nsc.Instance->OnUpdate(ts);
		});
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>, entt::exclude<DisabledComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>(entt::exclude<DisabledComponent>);
			for (auto entity : view)
			{
				auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>(entt::exclude<DisabledComponent>);
			for (auto entity : view)
			{
				auto& [transform, text] = view.get<TransformComponent, TextComponent>(entity);

				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, static_cast<int>(entity));
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::InstantiatePhysicsBody(Entity entity) const
	{
		if (!m_Box2DPhysicsSimulationEnabled)
			return;

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

		if (rb2d.RuntimeBody)
		{
			b2DestroyBody(rb2d.RuntimeBody->BodyId);
			delete rb2d.RuntimeBody;
			rb2d.RuntimeBody = nullptr;
		}

		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = Utils::RigidBody2DTypeToBox2DBody(rb2d.Type);
		bodyDef.position = { transform.Translation.x, transform.Translation.y };
		bodyDef.rotation = b2MakeRot(transform.Rotation.z);
		bodyDef.isBullet = rb2d.IsBullet;
		bodyDef.userData = reinterpret_cast<void*>(static_cast<uint64_t>(entity.GetUUID()));

		b2BodyId bodyId = b2CreateBody(m_PhysicsImpl->WorldId, &bodyDef);
		b2Body_SetMotionLocks(bodyId, { false, false, rb2d.FixedRotation });

		rb2d.RuntimeBody = new RuntimeBodyImpl();
		rb2d.RuntimeBody->BodyId = bodyId;

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<BoxCollider2DComponent>();

			b2Polygon box = b2MakeOffsetBox(collider.Size.x * transform.Scale.x
				, collider.Size.y * transform.Scale.y
				, { collider.Offset.x, collider.Offset.y }
			, b2Rot_identity);
			b2ShapeDef shapeDefinition = b2DefaultShapeDef();
			shapeDefinition.density = collider.Density;
			shapeDefinition.material.friction = collider.Friction;
			shapeDefinition.material.restitution = collider.Restitution;
			shapeDefinition.enableContactEvents = true;
			shapeDefinition.enableHitEvents = true;

			b2CreatePolygonShape(bodyId, &shapeDefinition, &box);
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<CircleCollider2DComponent>();

			b2Circle circle;
			circle.center = { collider.Offset.x, collider.Offset.y };
			circle.radius = transform.Scale.x * collider.Radius;

			b2ShapeDef shapeDefinition = b2DefaultShapeDef();
			shapeDefinition.density = collider.Density;
			shapeDefinition.material.friction = collider.Friction;
			shapeDefinition.material.restitution = collider.Restitution;
			shapeDefinition.enableContactEvents = true;
			shapeDefinition.enableHitEvents = true;

			b2CreateCircleShape(bodyId, &shapeDefinition, &circle);
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
			// C# Entity updates
			OnScriptEngineUpdate(ts);

			// Physics
			OnPhysics2DUpdate(ts);

			// Deferred collision resolution
			for (const auto& collision : m_PendingCollisions)
			{
				Entity entityA = GetEntityByUUID(collision.first);
				Entity entityB = GetEntityByUUID(collision.second);
				if (!entityA || !entityB)
					continue;
			
				if (entityA.HasComponent<ScriptComponent>())
				{
					ScriptEngine::OnCollision2D(entityA, entityB);
				}
				if (entityB.HasComponent<ScriptComponent>())
				{
					ScriptEngine::OnCollision2D(entityB, entityA);
				}
			}
			m_PendingCollisions.clear();

			// Deferred entity destruction
			for (const auto e : m_PendingDestroyEntities)
			{
				if (m_Registry.valid(e))
				{
					DestroyEntityImmediate({ e, this });
				}
			}
			m_PendingDestroyEntities.clear();
        }

        // Render 2D
        const Camera* mainCamera = nullptr;
        glm::mat4 mainCameraTransform;

        {
            const auto view = m_Registry.view<TransformComponent, CameraComponent>(entt::exclude<DisabledComponent>);
            for (const auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    mainCamera = &camera.Camera;
                    mainCameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera)
        {
            Renderer2D::BeginScene(*mainCamera, mainCameraTransform);

			// Draw sprites
			{
				const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>, entt::exclude<DisabledComponent>);
				for (auto entity : group)
				{
					auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
				}
			}

			// Draw circles
			{
				const auto view = m_Registry.view<TransformComponent, CircleRendererComponent>(entt::exclude<DisabledComponent>);
				for (auto entity : view)
				{
					auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
				}
			}

			// Draw text
            {
	            const auto view = m_Registry.view<TransformComponent, TextComponent>(entt::exclude<DisabledComponent>);
            	for (auto entity : view)
            	{
            		auto& [transform, text] = view.get<TransformComponent, TextComponent>(entity);

            		Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, static_cast<int>(entity));
            	}
            }

            Renderer2D::EndScene();
        }
    }


	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_Box2DPhysicsSimulationEnabled)
			return;

		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Physics
			constexpr int32_t subStepCount = 4;
			b2World_Step(m_PhysicsImpl->WorldId, ts, subStepCount);

			const auto view = m_Registry.view<RigidBody2DComponent>(entt::exclude<DisabledComponent>);
			for (const auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

				const b2BodyId body = rb2d.RuntimeBody->BodyId;
				const auto [x, y] = b2Body_GetPosition(body);
				transform.Translation.x = x;
				transform.Translation.y = y;
				const auto [c, s] = b2Body_GetRotation(body);
				transform.Rotation.z = std::atan2(s, c);

				b2Body_SetGravityScale(body, rb2d.GravityScale);
			}
		}

		// Render
		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
	}

    void Scene::OnViewportResize(const uint32_t width, const uint32_t height)
    {
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize our non-fixed aspect ratio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }

		// Rebuild the quad tree
		m_EntityQuadTree.reset();
		m_EntityQuadTree = CreateScope<QuadTree<Entity>>(glm::vec3(0.0f), m_ViewportWidth / 2.0f);
    }

    void Scene::OnCreateEntityRuntime(Entity entity) const
    {
		if (!m_IsRunning)
			return;

		if (entity.HasComponent<RigidBody2DComponent>())
			InstantiatePhysicsBody(entity);

		if (entity.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnRegisterEntity(entity);
			ScriptEngine::OnCreateEntity(entity);
		}
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}

		return {};
    }

    glm::vec3 Scene::ScreenToWorldPosition(const glm::vec3& screenPosition)
    {
		PRS_CORE_ASSERT(m_ViewportWidth > 0 && m_ViewportHeight > 0);

		Entity primaryCamera = GetPrimaryCameraEntity();
		PRS_CORE_ASSERT(primaryCamera);

		auto& cameraComponent = primaryCamera.GetComponent<CameraComponent>();
		auto& transformComponent = primaryCamera.GetComponent<TransformComponent>();

		const glm::mat4 view = glm::inverse(transformComponent.GetTransform());
		const glm::mat4& projection = cameraComponent.Camera.GetProjection();
		const glm::mat4 invVP = glm::inverse(projection * view);

		// Screen -> NDC [-1, 1], Y flipped (screen Y=0 is top)
		float ndcX = (screenPosition.x / static_cast<float>(m_ViewportWidth)) * 2.0f - 1.0f;
		float ndcY = 1.0f - (screenPosition.y / static_cast<float>(m_ViewportHeight)) * 2.0f;

		// Un-project near and far points to build a world-space ray
		glm::vec4 nearClip = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		nearClip /= nearClip.w;

		glm::vec4 farClip = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
		farClip /= farClip.w;

		glm::vec3 rayOrigin = glm::vec3(nearClip);
		glm::vec3 rayDir = glm::normalize(glm::vec3(farClip) - rayOrigin);

		// Intersect ray with the target world Z plane (screenPosition.z)
		const float targetZ = screenPosition.z;
		float t = 0.0f;
		if (glm::abs(rayDir.z) > 1e-6f)
			t = (targetZ - rayOrigin.z) / rayDir.z;

		glm::vec3 worldPos = rayOrigin + t * rayDir;

		return worldPos;
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> entityMap;

		// Copy entities
		auto view = other->m_Registry.view<IDComponent>();
		for (auto e : view)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			entityMap[uuid] = static_cast<entt::entity>(newEntity);
		}

		// Copy components (except IDComponent and TagComponent which are already copied)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, entityMap);

		CopyComponent<DisabledComponent>(dstSceneRegistry, srcSceneRegistry, entityMap);

		return newScene;
    }

#pragma region OnComponentAdded overloads
    template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		// static_assert(false, "Unknown component!");
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DisabledComponent>(Entity entity, DisabledComponent& component)
	{
	}
#pragma endregion
}
