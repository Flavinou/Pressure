#include "prspch.h"
#include "ScriptGlue.h"

#include <glm/glm.hpp>
#include <mono/metadata/object.h>

namespace Pressure
{

#define PRS_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Pressure.InternalCalls::" #Name, Name)

	namespace
	{

		void NativeLog(MonoString* message, int parameter)
		{
			char* messageStr = mono_string_to_utf8(message);
			std::string str(messageStr);
			mono_free(messageStr);

			PRS_CORE_TRACE("NativeLog: {} - {}", str, parameter);
		}

		void NativeLog_Vector3(glm::vec3* parameter, glm::vec3* result)
		{
			PRS_CORE_TRACE("NativeLog: Vec3({}, {}, {})", parameter->x, parameter->y, parameter->z);
			*result = glm::normalize(*parameter);
		}

		float NativeLog_Vector3Dot(glm::vec3* parameter)
		{
			PRS_CORE_TRACE("NativeLog: Vec3({}, {}, {})", parameter->x, parameter->y, parameter->z);
			return glm::dot(*parameter, *parameter);
		}

	}

	void ScriptGlue::RegisterFunctions()
	{
		PRS_ADD_INTERNAL_CALL(NativeLog);
		PRS_ADD_INTERNAL_CALL(NativeLog_Vector3);
		PRS_ADD_INTERNAL_CALL(NativeLog_Vector3Dot);
	}

}
