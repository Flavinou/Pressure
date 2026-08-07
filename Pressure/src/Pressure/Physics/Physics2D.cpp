#include "prspch.h"

#include <glm/vec3.hpp>

#include "glm/gtx/norm.hpp"

namespace Pressure
{
	
	namespace Physics2D
	{

		bool CirclesOverlap(const glm::vec3& center, float radius, const glm::vec3& otherCenter,
			float otherRadius)
		{
			float distanceSquared = glm::distance2(center, otherCenter);
			float radiusSum = radius + otherRadius;
			return distanceSquared <= (radiusSum * radiusSum);
		}

		bool BoxesOverlap(const glm::vec3& center, const glm::vec3& halfExtent, const glm::vec3& otherCenter, const glm::vec3& otherHalfExtent)
		{
			return (center.x - halfExtent.x < otherCenter.x + otherHalfExtent.x &&
					center.x + halfExtent.x > otherCenter.x - otherHalfExtent.x &&
					center.y - halfExtent.y < otherCenter.y + otherHalfExtent.y &&
					center.y + halfExtent.y > otherCenter.y - otherHalfExtent.y);
		}

		bool CircleBoxOverlap(const glm::vec3& circleCenter, float circleRadius, const glm::vec3& boxCenter, const glm::vec3& boxHalfExtent)
		{
			float closestX = std::max(boxCenter.x - boxHalfExtent.x, std::min(circleCenter.x, boxCenter.x + boxHalfExtent.x));
			float closestY = std::max(boxCenter.y - boxHalfExtent.y, std::min(circleCenter.y, boxCenter.y + boxHalfExtent.y));
			float distanceSquared = (circleCenter.x - closestX) * (circleCenter.x - closestX) +
									(circleCenter.y - closestY) * (circleCenter.y - closestY);
			return distanceSquared <= (circleRadius * circleRadius);
		}
	}

}
