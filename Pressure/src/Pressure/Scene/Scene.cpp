#include "prspch.h"
#include "Pressure/Scene/Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Pressure/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Pressure
{

    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string& name/* = std::string()*/)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        // Update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) 
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
        }

        // Render 2D
        Camera* mainCamera = nullptr;
        glm::mat4 mainCameraTransform;

        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
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

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
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

}
