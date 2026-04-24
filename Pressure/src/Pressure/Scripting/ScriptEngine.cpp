#include "prspch.h"
#include "ScriptEngine.h"

#include "Pressure/Scripting/ScriptGlue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Pressure
{

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass EntityClass;
	};

	namespace
	{
		ScriptEngineData* s_Data = nullptr;
	}

	namespace Utils
	{

		char* ReadBytes(const std::filesystem::path& filePath, uint32_t* outSize)
		{
			std::ifstream stream(filePath, std::ios::binary | std::ios::ate);
			if (!stream)
			{
				PRS_CORE_ERROR("Could not open file '{}'", filePath);
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = static_cast<uint32_t>(end - stream.tellg());

			if (size == 0)
			{
				PRS_CORE_ERROR("File '{}' is empty", filePath);
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read(buffer, size);
			stream.close();

			if (outSize)
				*outSize = size;
			return buffer;
		}

		MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);
			if (!fileData)
				return nullptr;

			std::string pathString = assemblyPath.string();

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);
			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				delete[] fileData;
				PRS_CORE_ERROR("Failed to load assembly from '{}' - image open failed with status {}: {}", pathString, status, errorMessage);
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				delete[] fileData;
				PRS_CORE_ERROR("Failed to load assembly from '{}' - assembly load failed with status {}: {}", pathString, status, errorMessage);
				return nullptr;
			}

			mono_image_close(image);
			delete[] fileData;
			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			uint32_t typeCount = mono_table_info_get_rows(typeTable);

			for (uint32_t i = 1; i < typeCount; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				PRS_CORE_TRACE(" - {}.{}", nameSpace, name);
			}
		}

	}
	
	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		LoadAssembly("resources/scripts/Pressure-ScriptCore.dll");

		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = ScriptClass("Pressure", "Entity");

		// Retrieve and instantiate class by name from core assembly
		MonoObject* instance = s_Data->EntityClass.Instantiate();

		// Call method
		MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
		s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);

		// Call a method with parameters
		MonoMethod* printMessageWithParamFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);
		int value1 = 5;
		int value2 = 10;
		void* params[2] = { &value1, &value2 };
		s_Data->EntityClass.InvokeMethod(instance, printMessageWithParamFunc, params);

		// Call a method with a custom message
		MonoString* customMessage = mono_string_new(s_Data->AppDomain, "Hello from C++!");
		MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
		void* stringParam = customMessage;
		s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();

		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain(const_cast<char*>("PressureScriptRuntime"), nullptr);
		PRS_CORE_ASSERT(s_Data->AppDomain);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filePath);
		if (!s_Data->CoreAssembly)
		{
			PRS_CORE_ERROR("Failed to load core script assembly");
			return;
		}
		// PrintAssemblyTypes(s_Data->CoreAssembly);

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("PressureJITRuntime");
		PRS_CORE_ASSERT(rootDomain);

		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		// mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;

		s_Data->CoreAssembly = nullptr;

		delete s_Data;
		s_Data = nullptr;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

}
