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

		template<typename T>
		T GetFieldValue(const std::string& fieldName) const
		{
			if (!GetFieldValueInternal(fieldName, s_FieldValueBuffer))
			{
				return T();
			}

			return *reinterpret_cast<T*>(s_FieldValueBuffer);
		}

		template<typename T>
		void SetFieldValue(const std::string& fieldName, const T& value)
		{
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

		inline static std::byte s_FieldValueBuffer[8];
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		static bool EntityClassExists(const std::string& fullClassName);
		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityId);

		static void LoadAssembly(const std::filesystem::path& filePath);
		static void LoadAppAssembly(const std::filesystem::path& filePath);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);

		static MonoImage* GetCoreAssemblyImage();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};

}
