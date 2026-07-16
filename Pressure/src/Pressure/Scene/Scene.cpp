#include "prspch.h"
#include "Pressure/Scene/Scene.h"

#include "Pressure/Physics/Physics2D.h"
#include "Pressure/Renderer/Renderer2D.h"
#include "Pressure/Scene/Components.h"
#include "Pressure/Scene/Entity.h"
#include "Pressure/Scene/ScriptableEntity.h"
#include "Pressure/Scripting/ScriptEngine.h"

#include <box2d/box2d.h>
#include <glm/glm.hpp>

#include "Pressure/Renderer/Font.h"

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
					Component& component = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntityID, component);
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
		return newEntity;
    }

    void Scene::DestroyEntity(Entity entity)
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

			// Instantiate all script entities
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

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
		b2WorldDef worldDefinition = b2DefaultWorldDef();
		worldDefinition.gravity = { 0.0f, -9.81f };
		m_PhysicsImpl->WorldId = b2CreateWorld(&worldDefinition);

		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			InstantiatePhysicsBody(entity);
		}
	}

	void Scene::OnPhysics2DStop()
	{
		b2DestroyWorld(m_PhysicsImpl->WorldId);
		m_PhysicsImpl = nullptr;

		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			rb2d.RuntimeBody = nullptr;
		}
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
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
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

		rb2d.RuntimeBody = nullptr;

		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = Utils::RigidBody2DTypeToBox2DBody(rb2d.Type);
		bodyDef.position = { transform.Translation.x, transform.Translation.y };
		bodyDef.rotation = b2MakeRot(transform.Rotation.z);

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

			b2CreateCircleShape(bodyId, &shapeDefinition, &circle);
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
			// C# Entity updates
			const auto view = m_Registry.view<ScriptComponent>();
			for (const auto e : view)
			{
				const Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, ts);
			}

            m_Registry.view<NativeScriptComponent>().each([this, ts](auto entity, auto& nsc) 
            {
				// TODO: Move to Scene::OnScenePlay and not check this every frame
                if (!nsc.Instance)
                {
					nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
                }

				nsc.Instance->OnUpdate(ts);
            });

			// Physics
			{
				constexpr int32_t subStepCount = 4;
				b2World_Step(m_PhysicsImpl->WorldId, ts, subStepCount);

				const auto view = m_Registry.view<RigidBody2DComponent>();
				for (const auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

					const b2BodyId body = rb2d.RuntimeBody->BodyId;
					const auto [x, y] = b2Body_GetPosition(body);
					transform.Translation.x = x;
					transform.Translation.y = y;
					auto [c, s] = b2Body_GetRotation(body);
					transform.Rotation.z = std::atan2(s, c);

					b2Body_SetGravityScale(body, rb2d.GravityScale);
				}
			}
        }

        // Render 2D
        const Camera* mainCamera = nullptr;
        glm::mat4 mainCameraTransform;

        {
            const auto view = m_Registry.view<TransformComponent, CameraComponent>();
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
				const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
				}
			}

			// Draw circles
			{
				const auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
				}
			}

			// Draw text
            {
	            const auto view = m_Registry.view<TransformComponent, TextComponent>();
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
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Physics
			constexpr int32_t subStepCount = 4;
			b2World_Step(m_PhysicsImpl->WorldId, ts, subStepCount);

			const auto view = m_Registry.view<RigidBody2DComponent>();
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
    }

    void Scene::OnCreateEntityRuntime(Entity entity) const
    {
		if (!m_IsRunning)
			return;

		InstantiatePhysicsBody(entity);
		ScriptEngine::OnCreateEntity(entity);
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
#pragma endregion
}
