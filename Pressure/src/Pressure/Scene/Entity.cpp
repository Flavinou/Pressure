#include "prspch.h"
#include "Entity.h"

namespace Pressure
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }
}