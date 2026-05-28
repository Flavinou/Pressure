#pragma once

#include "Pressure/Scene/Entity.h"

#include <filesystem>
#include <map>
#include <string>

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Pressure
{

	class Scene;

	enum class ScriptFieldType : uint8_t
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		std::string Name;
		ScriptFieldType Type;

		MonoClassField* MonoClassField { nullptr };
	};

	// ScriptField + data storage
	class ScriptFieldInstance
	{
	public:
		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= sizeof(m_Buffer), "Type is too large to fit in buffer");
			return *reinterpret_cast<T*>(m_Buffer);
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= sizeof(m_Buffer), "Type is too large to fit in buffer");
			memcpy(m_Buffer, &value, sizeof(T));
		}

		void SetField(const ScriptField& field) { m_Field = field; }

	private:
		ScriptField m_Field{};
		std::byte m_Buffer[16]{};

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		[[nodiscard]] const std::string& GetName() const { return m_ClassName; }
		[[nodiscard]] const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		std::map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate() const;
		void InvokeOnUpdate(float ts) const;

		Ref<ScriptClass> GetScriptClass() const { return m_ScriptClass; }

		MonoObject* GetManagedObject() const { return m_Instance; }

		template<typename T>
		T GetFieldValue(const std::string& fieldName) const
		{
			static_assert(sizeof(T) <= sizeof(s_FieldValueBuffer), "Type is too large to fit in buffer");
			if (!GetFieldValueInternal(fieldName, s_FieldValueBuffer))
			{
				return T();
			}

			return *reinterpret_cast<T*>(s_FieldValueBuffer);
		}

		template<typename T>
		void SetFieldValue(const std::string& fieldName, T& value)
		{
			static_assert(sizeof(T) <= sizeof(s_FieldValueBuffer), "Type is too large to fit in buffer");

			SetFieldValueInternal(fieldName, &value);
		}

	private:
		bool GetFieldValueInternal(const std::string& fieldName, void* outBuffer) const;
		bool SetFieldValueInternal(const std::string& fieldName, const void* value) const;

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static std::byte s_FieldValueBuffer[16];

		friend class ScriptEngine;
		friend class ScriptFieldInstance;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<ScriptClass> GetEntityClass(const std::string& name);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
		static bool EntityClassExists(const std::string& fullClassName);
		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityId);

		static void LoadAssembly(const std::filesystem::path& filePath);
		static void LoadAppAssembly(const std::filesystem::path& filePath);

		static void ReloadAssembly();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);

		static MonoImage* GetCoreAssemblyImage();

		static MonoObject* GetManagedInstance(UUID entityId);

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};

	namespace Utils
	{
		inline const char* ScriptFieldTypeToString(const ScriptFieldType type)
		{
			switch (type)
			{
				case ScriptFieldType::None:		return "None";
				case ScriptFieldType::Float:	return "Float";
				case ScriptFieldType::Double:	return "Double";
				case ScriptFieldType::Bool:		return "Bool";
				case ScriptFieldType::Char:		return "Char";
				case ScriptFieldType::Byte:		return "Byte";
				case ScriptFieldType::Short:	return "Short";
				case ScriptFieldType::Int:		return "Int";
				case ScriptFieldType::Long:		return "Long";
				case ScriptFieldType::UByte:	return "UByte";
				case ScriptFieldType::UShort:	return "UShort";
				case ScriptFieldType::UInt:		return "UInt";
				case ScriptFieldType::ULong:	return "ULong";
				case ScriptFieldType::Vector2:	return "Vector2";
				case ScriptFieldType::Vector3:	return "Vector3";
				case ScriptFieldType::Vector4:	return "Vector4";
				case ScriptFieldType::Entity:	return "Entity";
				default:						return "<Unknown>";
			}
		}

		inline ScriptFieldType ScriptFieldTypeFromString(const std::string_view fieldType)
		{
			if (fieldType == "None")		return ScriptFieldType::None;
			if (fieldType == "Float")		return ScriptFieldType::Float;
			if (fieldType == "Double")		return ScriptFieldType::Double;
			if (fieldType == "Bool")		return ScriptFieldType::Bool;
			if (fieldType == "Char")		return ScriptFieldType::Char;
			if (fieldType == "Byte")		return ScriptFieldType::Byte;
			if (fieldType == "Short")		return ScriptFieldType::Short;
			if (fieldType == "Int")			return ScriptFieldType::Int;
			if (fieldType == "Long")		return ScriptFieldType::Long;
			if (fieldType == "UByte")		return ScriptFieldType::UByte;
			if (fieldType == "UShort")		return ScriptFieldType::UShort;
			if (fieldType == "UInt")		return ScriptFieldType::UInt;
			if (fieldType == "ULong")		return ScriptFieldType::ULong;
			if (fieldType == "Vector2")		return ScriptFieldType::Vector2;
			if (fieldType == "Vector3")		return ScriptFieldType::Vector3;
			if (fieldType == "Vector4")		return ScriptFieldType::Vector4;
			if (fieldType == "Entity")		return ScriptFieldType::Entity;

			PRS_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}
	}

}
