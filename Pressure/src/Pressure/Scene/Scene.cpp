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
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = Utils::RigidBody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position = { transform.Translation.x, transform.Translation.y };
			bodyDef.rotation = b2MakeRot(transform.Rotation.z);

			b2BodyId bodyId = b2CreateBody(m_PhysicsImpl->WorldId, &bodyDef);
			b2Body_SetMotionLocks(bodyId, { rb2d.FixedRotation, rb2d.FixedRotation, rb2d.FixedRotation });

			rb2d.RuntimeBody = new RuntimeBodyImpl();
			rb2d.RuntimeBody->BodyId = bodyId;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& collider = entity.GetComponent<BoxCollider2DComponent>();

				b2Polygon box = b2MakeBox(collider.Size.x * transform.Scale.x, collider.Size.y * transform.Scale.y);
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
	}

	void Scene::OnPhysics2DStop()
	{
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			delete rb2d.RuntimeBody;
		}

		b2DestroyWorld(m_PhysicsImpl->WorldId);
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

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		// Draw text
		{
			Renderer2D::DrawString("Hello, World!", Font::GetDefault(), glm::mat4(1.0f), glm::vec4(1.0f));
			Renderer2D::DrawString(
				R"(
			// Renderer2D - MSDF text shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 2) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_FontAtlas;

float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlas, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec4 texColor = Input.Color * texture(u_FontAtlas, Input.TexCoord);

	vec3 msd = texture(u_FontAtlas, Input.TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
    o_Color = mix(bgColor, Input.Color, opacity);
	if (o_Color.a == 0.0)
		discard;
	
	o_EntityID = v_EntityID;
}
)"
, Font::GetDefault(), glm::mat4(1.0f), glm::vec4(1.0f));

		}

		Renderer2D::EndScene();
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
}
