#pragma once

#include "Pressure/Scene/Components.h"


#include <box2d/id.h>
#include <box2d/types.h>

namespace Pressure
{

	struct PhysicsWorldImpl
	{
		b2WorldId WorldId{};
	};

	struct RuntimeBodyImpl
	{
		b2BodyId BodyId{};
	};

	namespace Utils
	{
		inline b2BodyType RigidBody2DTypeToBox2DBody(RigidBody2DComponent::BodyType type)
		{
			switch (type)
			{
			case RigidBody2DComponent::BodyType::Static:    return b2_staticBody;
			case RigidBody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case RigidBody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
			}

			PRS_CORE_ASSERT(false, "Unknown RigidBody2DComponent::BodyType!");
			return b2_staticBody;
		}
	}

}
