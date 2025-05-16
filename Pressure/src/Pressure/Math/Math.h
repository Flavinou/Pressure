#pragma once

#include <glm/glm.hpp>

namespace Pressure::Math
{

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& transtlation, glm::vec3& rotation, glm::vec3& scale);

}
