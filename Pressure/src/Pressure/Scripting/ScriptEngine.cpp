#include "prspch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Pressure
{

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
	};

	namespace
	{

		ScriptEngineData* s_Data = nullptr;

		char* ReadBytes(const std::string& filePath, uint32_t* outSize)
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

		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);
			if (!fileData)
				return nullptr;

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);
			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				delete[] fileData;
				PRS_CORE_ERROR("Failed to load assembly from '{}' - image open failed with status {}: {}", assemblyPath, status, errorMessage);
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				delete[] fileData;
				PRS_CORE_ERROR("Failed to load assembly from '{}' - assembly load failed with status {}: {}", assemblyPath, status, errorMessage);
				return nullptr;
			}

			mono_image_close(image);
			delete[] fileData;
			return assembly;
		}

		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const std::string& namespaceName, const std::string& className)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			MonoClass* monoClass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
			if (monoClass == nullptr)
			{
				PRS_CORE_ERROR("Could not find class '{}' in namespace '{}'", className, namespaceName);
				return nullptr;
			}

			return monoClass;
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
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();

		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("PressureJITRuntime");
		PRS_CORE_ASSERT(rootDomain);

		s_Data->RootDomain = rootDomain;

		MonoDomain* appDomain = mono_domain_create_appdomain(const_cast<char*>("PressureScriptRuntime"), nullptr);
		PRS_CORE_ASSERT(appDomain);

		s_Data->AppDomain = appDomain;
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = LoadCSharpAssembly("resources/scripts/Pressure-ScriptCore.dll");
		if (!s_Data->CoreAssembly)
		{
			PRS_CORE_ERROR("Failed to load core script assembly");
			return;
		}
		PrintAssemblyTypes(s_Data->CoreAssembly);

		// Create an object
		MonoClass* scriptCoreClass = GetClassInAssembly(s_Data->CoreAssembly, "Pressure", "Program");
		MonoObject* instance = mono_object_new(s_Data->AppDomain, scriptCoreClass);
		mono_runtime_object_init(instance);

		// Call a method
		MonoMethod* printMessageFunc = mono_class_get_method_from_name(scriptCoreClass, "PrintMessage", 0);
		mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);

		// Call a method with parameters
		MonoMethod* printMessageWithParamFunc = mono_class_get_method_from_name(scriptCoreClass, "PrintInts", 2);
		int value1 = 5;
		int value2 = 10;
		void* params[2] = { &value1, &value2 };
		mono_runtime_invoke(printMessageWithParamFunc, instance, params, nullptr);

		// Call a method with a custom message
		MonoString* customMessage = mono_string_new(s_Data->AppDomain, "Hello from C++!");
		MonoMethod* printCustomMessageFunc = mono_class_get_method_from_name(scriptCoreClass, "PrintCustomMessage", 1);
		void* stringParam = customMessage;
		mono_runtime_invoke(printCustomMessageFunc, instance, &stringParam, nullptr);
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

}
