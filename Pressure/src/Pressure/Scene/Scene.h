#pragma once

#include "Pressure/Core/Timestep.h"

#include <entt.hpp>

namespace Pressure
{

    class Scene
    {
    public:
        Scene();
        ~Scene();

        entt::entity CreateEntity();

        // TODO: Remove this getter
        entt::registry& Registry() { return m_Registry; }

        void OnUpdate(Timestep ts);
    private:
        entt::registry m_Registry;
    };

}