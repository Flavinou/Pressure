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

		inline RigidBody2DComponent::BodyType RigidBody2DTypeFromBox2DBody(b2BodyType bodyType)
		{
			switch (bodyType)
			{
				case b2_staticBody:    return RigidBody2DComponent::BodyType::Static;
				case b2_dynamicBody:   return RigidBody2DComponent::BodyType::Dynamic;
				case b2_kinematicBody: return RigidBody2DComponent::BodyType::Kinematic;
			}

			PRS_CORE_ASSERT(false, "Unknown Box2D body type!");
			return RigidBody2DComponent::BodyType::Static;
		}
	}

	namespace Physics2D
	{
		bool CirclesOverlap(const glm::vec3& center, float radius, const glm::vec3& otherCenter, float otherRadius);
		bool BoxesOverlap(const glm::vec3& center, const glm::vec3& halfExtent, const glm::vec3& otherCenter, const glm::vec3& otherHalfExtent);
		bool CircleBoxOverlap(const glm::vec3& circleCenter, float circleRadius, const glm::vec3& boxCenter, const glm::vec3& boxHalfExtent);
	}

}
