#include "prspch.h"
#include "ScriptEngine.h"

#include "Pressure/Scene/Scene.h"
#include "Pressure/Scripting/ScriptGlue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

// Comes from tabledefs.h which is not shipped with the Mono runtime,
// so we need to redefine it here
enum : uint8_t
{
	MONO_FIELD_ATTRIBUTE_ACCESS_MASK = 0x0007,
	MONO_FIELD_ATTRIBUTE_PRIVATE = 0x0001,
	MONO_FIELD_ATTRIBUTE_PUBLIC = 0x0006
};

namespace Pressure
{

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilePath;
		std::filesystem::path AppAssemblyFilePath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	namespace
	{
		ScriptEngineData* s_Data = nullptr;

		std::unordered_map<std::string, ScriptFieldType> s_MonoTypeToScriptFieldTypeMap = {
			{ "System.Single", ScriptFieldType::Float },
			{ "System.Double", ScriptFieldType::Double },
			{ "System.Boolean", ScriptFieldType::Bool },
			{ "System.Char", ScriptFieldType::Char },
			{ "System.Int16", ScriptFieldType::Short },
			{ "System.Int32", ScriptFieldType::Int },
			{ "System.Int64", ScriptFieldType::Long },
			{ "System.Byte", ScriptFieldType::Byte },
			{ "System.UInt16", ScriptFieldType::UShort },
			{ "System.UInt32", ScriptFieldType::UInt },
			{ "System.UInt64", ScriptFieldType::ULong },

			{ "Pressure.Vector2", ScriptFieldType::Vector2 },
			{ "Pressure.Vector3", ScriptFieldType::Vector3 },
			{ "Pressure.Vector4", ScriptFieldType::Vector4 },

			{ "Pressure.Entity", ScriptFieldType::Entity },
		};
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

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			if (!monoType)
				return ScriptFieldType::None;

			const char* typeName = mono_type_get_name(monoType);
			if (const auto it = s_MonoTypeToScriptFieldTypeMap.find(typeName); it != s_MonoTypeToScriptFieldTypeMap.end())
				return it->second;

			PRS_CORE_ERROR("Unsupported Mono type '{}' for script field", typeName);
			return ScriptFieldType::None;
		}

	}
	
	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();

		ScriptGlue::RegisterFunctions();

		LoadAssembly("resources/scripts/Pressure-ScriptCore.dll");
		LoadAppAssembly("SandboxProject/Assets/Scripts/bin/Sandbox.dll");
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Pressure", "Entity", true);
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
	}

	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();
		
		const MonoTableInfo* typeDefTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		uint32_t typeCount = mono_table_info_get_rows(typeDefTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Pressure", "Entity");

		for (uint32_t i = 1; i < typeCount; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (strlen(nameSpace) != 0)
			{
				fullName = fmt::format("{}.{}", nameSpace, className);
			}
			else
			{
				fullName = className;
			}

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);
			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_Data->EntityClasses[fullName] = scriptClass;

			// This routine is an iterator for retrieving fields in a class
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more elements are available, it returns NULL and the handle is undefined.

			int fieldCount = mono_class_num_fields(monoClass);
			PRS_CORE_TRACE("Found {} fields in script class '{}'", fieldCount, fullName);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field) & MONO_FIELD_ATTRIBUTE_ACCESS_MASK;
				if (flags & MONO_FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					PRS_CORE_WARN(" - {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->m_Fields[fieldName] = { fieldName, fieldType, field };
				}
			}
		}
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(const UUID entityId)
	{
		const auto it = s_Data->EntityInstances.find(entityId);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::GetManagedInstance(UUID entityId)
	{
		PRS_CORE_ASSERT(s_Data->EntityInstances.find(entityId) != s_Data->EntityInstances.end());
		return s_Data->EntityInstances[entityId]->GetManagedObject();
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain(const_cast<char*>("PressureScriptRuntime"), nullptr);
		PRS_CORE_ASSERT(s_Data->AppDomain);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssemblyFilePath = filePath;
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filePath);
		if (!s_Data->CoreAssembly)
		{
			PRS_CORE_ERROR("Failed to load core script assembly");
			return;
		}

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filePath)
	{
		s_Data->AppAssemblyFilePath = filePath;
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filePath);
		if (!s_Data->AppAssembly)
		{
			PRS_CORE_ERROR("Failed to load app script assembly!");
			return;
		}

		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptEngine::ReloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);

		LoadAssembly(s_Data->CoreAssemblyFilePath);
		LoadAppAssembly(s_Data->AppAssemblyFilePath);
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Pressure", "Entity", true);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (!EntityClassExists(sc.ClassName))
		{
			PRS_CORE_ERROR("Script class '{}' not found for entity '{}'", sc.ClassName, entity.GetName());
			return;
		}

		UUID entityId = entity.GetUUID();
		Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
		s_Data->EntityInstances[entityId] = instance;

		// Copy field values
		if (s_Data->EntityScriptFields.find(entityId) != s_Data->EntityScriptFields.end())
		{
			const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields[entityId];
			for (const auto& [fieldName, fieldInstance] : fieldMap)
			{
				if (!instance->SetFieldValueInternal(fieldName, fieldInstance.m_Buffer))
				{
					PRS_CORE_ERROR("Failed to set field '{}' for entity '{}'", fieldName, entity.GetName());
				}
			}
		}

		instance->InvokeOnCreate();
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetUUID();
		PRS_CORE_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());

		Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
		instance->InvokeOnUpdate(ts);
	}

	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		const auto it = s_Data->EntityClasses.find(name);
		if (it != s_Data->EntityClasses.end())
		{
			return it->second;
		}

		return nullptr;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		PRS_CORE_ASSERT(entity);

		UUID entityId = entity.GetUUID();
		return s_Data->EntityScriptFields[entityId];
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore/* = false*/)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
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

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate() const
	{
		if (!m_OnCreateMethod)
		{
			return;
		}

		m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts) const
	{
		if (!m_OnUpdateMethod)
		{
			return;
		}

		void* param = &ts;
		m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& fieldName, void* outBuffer) const
	{
		const auto& fields = m_ScriptClass->GetFields();
		const auto it = fields.find(fieldName);
		if (it == fields.end())
		{
			PRS_CORE_ERROR("Field '{}' not found in script class '{}'", fieldName, m_ScriptClass->GetName());
			return false;
		}

		const ScriptField& field = it->second;
		if (!field.MonoClassField)
		{
			PRS_CORE_ERROR("Field '{}' is not a valid MonoClassField", fieldName);
			return false;
		}

		// Get the field value from the Mono object
		mono_field_get_value(m_Instance, field.MonoClassField, outBuffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& fieldName, const void* value) const
	{
		const auto& fields = m_ScriptClass->GetFields();
		const auto it = fields.find(fieldName);
		if (it == fields.end())
		{
			PRS_CORE_ERROR("Field '{}' not found in script class '{}'", fieldName, m_ScriptClass->GetName());
			return false;
		}

		const ScriptField& field = it->second;
		if (!field.MonoClassField)
		{
			PRS_CORE_ERROR("Field '{}' is not a valid MonoClassField", fieldName);
			return false;
		}

		// Set the field value on the Mono object
		mono_field_set_value(m_Instance, field.MonoClassField, const_cast<void*>(value));
		return true;
	}
}
