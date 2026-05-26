#include "prspch.h"
#include "SceneSerializer.h"

#include "Components.h"
#include "Entity.h"
#include "Pressure/Core/UUID.h"
#include "Pressure/Scripting/ScriptEngine.h"

#include <entt.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Pressure::UUID>
	{
		static Node encode(const Pressure::UUID& uuid)
		{
			Node node;
			node.push_back(static_cast<uint64_t>(uuid));
			return node;
		}

		static bool decode(const Node& node, Pressure::UUID& uuid)
		{
			if (!node.IsScalar())
			{
				return false;
			}
			uuid = Pressure::UUID(node.as<uint64_t>());
			return true;
		}
	};

}

namespace Pressure
{

#define WRITE_SCRIPT_FIELD(FieldType, Type)				\
	case ScriptFieldType::FieldType:					\
		out << scriptField.GetValue<Type>();			\
		break

#define READ_SCRIPT_FIELD(FieldType, Type)				\
	case ScriptFieldType::FieldType:					\
	{													\
		Type data = scriptField["Data"].as<Type>();		\
		fieldInstance.SetValue(data);					\
		break;											\
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	namespace Utils
	{

		std::string RigidBody2DBodyTypeToString(RigidBody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case RigidBody2DComponent::BodyType::Static:    return "Static";
			case RigidBody2DComponent::BodyType::Dynamic:   return "Dynamic";
			case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
			}

			PRS_CORE_ASSERT(false, "Unknown body type");
			return {};
		}

		RigidBody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
		{
			if (bodyTypeString == "Static")    return RigidBody2DComponent::BodyType::Static;
			if (bodyTypeString == "Dynamic")   return RigidBody2DComponent::BodyType::Dynamic;
			if (bodyTypeString == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;

			PRS_CORE_ASSERT(false, "Unknown body type");
			return RigidBody2DComponent::BodyType::Static;
		}

		void SerializeEntity(YAML::Emitter& out, Entity entity)
		{
			PRS_CORE_ASSERT(entity.HasComponent<IDComponent>());

			out << YAML::BeginMap; // Entity node
			out << YAML::Key << "Entity";
			out << YAML::Value << entity.GetUUID();

			if (entity.HasComponent<TagComponent>())
			{
				out << YAML::Key << "TagComponent";
				out << YAML::BeginMap; // TagComponent node

				auto& tag = entity.GetComponent<TagComponent>().Tag;
				out << YAML::Key << "Tag" << YAML::Value << tag;

				out << YAML::EndMap; // TagComponent node
			}

			if (entity.HasComponent<TransformComponent>())
			{
				out << YAML::Key << "TransformComponent";
				out << YAML::BeginMap; // TransformComponent node

				auto& tc = entity.GetComponent<TransformComponent>();
				out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
				out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
				out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

				out << YAML::EndMap; // TransformComponent node
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				out << YAML::Key << "SpriteRendererComponent";
				out << YAML::BeginMap; // SpriteRendererComponent node

				auto& src = entity.GetComponent<SpriteRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << src.Color;
				if (src.Texture)
					out << YAML::Key << "TexturePath" << YAML::Value << src.Texture->GetPath();

				out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;

				out << YAML::EndMap; // SpriteRendererComponent node
			}

			if (entity.HasComponent<CircleRendererComponent>())
			{
				out << YAML::Key << "CircleRendererComponent";
				out << YAML::BeginMap; // CircleRendererComponent node

				auto& crc = entity.GetComponent<CircleRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << crc.Color;
				out << YAML::Key << "Thickness" << YAML::Value << crc.Thickness;
				out << YAML::Key << "Fade" << YAML::Value << crc.Fade;

				out << YAML::EndMap; // CircleRendererComponent node
			}

			if (entity.HasComponent<CameraComponent>())
			{
				out << YAML::Key << "CameraComponent";
				out << YAML::BeginMap; // CameraComponent node

				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap; // Camera node

				out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
				out << YAML::Key << "PerspectiveVerticalFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
				out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
				out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
				out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
				out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

				out << YAML::EndMap; // Camera node

				out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
				out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

				out << YAML::EndMap; // CameraComponent node
			}

			if (entity.HasComponent<ScriptComponent>())
			{
				out << YAML::Key << "ScriptComponent";
				out << YAML::BeginMap; // ScriptComponent node

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();
				out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

				// Fields
				Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
				const auto& fields = entityClass->GetFields();
				if (!fields.empty())
				{
					out << YAML::Key << "ScriptFields" << YAML::Value;
					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					out << YAML::BeginSeq;

					for (const auto& [fieldName, field] : fields)
					{
						if (entityFields.find(fieldName) == entityFields.end())
						{
							continue; // Field has not been set in editor, skip it
						}

						out << YAML::BeginMap; // ScriptField node
						out << YAML::Key << "Name" << YAML::Value << fieldName;
						out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

						out << YAML::Key << "Data" << YAML::Value;
						ScriptFieldInstance& scriptField = entityFields.at(fieldName);

						switch (field.Type)
						{
							WRITE_SCRIPT_FIELD(Float,	float		);
							WRITE_SCRIPT_FIELD(Double,	double		);
							WRITE_SCRIPT_FIELD(Bool,	bool		);
							WRITE_SCRIPT_FIELD(Char,	char		);
							WRITE_SCRIPT_FIELD(Byte,	int8_t		);
							WRITE_SCRIPT_FIELD(Short,	int16_t		);
							WRITE_SCRIPT_FIELD(Int,	int32_t			);
							WRITE_SCRIPT_FIELD(Long,	int64_t		);
							WRITE_SCRIPT_FIELD(UByte,	uint8_t		);
							WRITE_SCRIPT_FIELD(UShort,	uint16_t	);
							WRITE_SCRIPT_FIELD(UInt,	uint32_t	);
							WRITE_SCRIPT_FIELD(ULong,	uint64_t	);
							WRITE_SCRIPT_FIELD(Vector2,	glm::vec2	);
							WRITE_SCRIPT_FIELD(Vector3,	glm::vec3	);
							WRITE_SCRIPT_FIELD(Vector4,	glm::vec4	);
							WRITE_SCRIPT_FIELD(Entity,	UUID		);
						}

						out << YAML::EndMap; // ScriptField node
					}
					out << YAML::EndSeq;
				}

				out << YAML::EndMap; // ScriptComponent node
			}

			if (entity.HasComponent<RigidBody2DComponent>())
			{
				out << YAML::Key << "RigidBody2DComponent";
				out << YAML::BeginMap; // RigidBody2DComponent node

				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
				out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2d.Type);
				out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;

				out << YAML::EndMap; // RigidBody2DComponent node
			}

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				out << YAML::Key << "BoxCollider2DComponent";
				out << YAML::BeginMap; // BoxCollider2DComponent node

				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
				out << YAML::Key << "Size" << YAML::Value << bc2d.Size;
				out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
				out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
				out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;

				out << YAML::EndMap; // BoxCollider2DComponent node
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				out << YAML::Key << "CircleCollider2DComponent";
				out << YAML::BeginMap; // CircleCollider2DComponent node

				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				out << YAML::Key << "Offset" << YAML::Value << cc2d.Offset;
				out << YAML::Key << "Radius" << YAML::Value << cc2d.Radius;
				out << YAML::Key << "Density" << YAML::Value << cc2d.Density;
				out << YAML::Key << "Friction" << YAML::Value << cc2d.Friction;
				out << YAML::Key << "Restitution" << YAML::Value << cc2d.Restitution;

				out << YAML::EndMap; // CircleCollider2DComponent node
			}

			out << YAML::EndMap; // Entity node
		}

	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{		
		PRS_CORE_TRACE("Serializing active scene...");

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.view<entt::entity>().each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			Utils::SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();

		PRS_CORE_TRACE("Serializing done.");
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		// Not implemented
		PRS_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filePath)
	{
		try 
		{
			YAML::Node rootNode;

			try
			{
				rootNode = YAML::LoadFile(filePath);
			}
			catch (YAML::ParserException& e)
			{
				PRS_CORE_ERROR("An error occurred while parsing file '{0}'... Error: {1}", filePath, e.what());
				return false;
			}

			if (!rootNode["Scene"])
			{
				return false;
			}

			std::string sceneName = rootNode["Scene"].as<std::string>();
			PRS_CORE_TRACE("Deserializing scene '{0}'...", sceneName);

			auto entitiesNode = rootNode["Entities"];
			if (!entitiesNode)
			{
				return false;
			}

			for (auto entityNode : entitiesNode)
			{
				uint64_t uuid = entityNode["Entity"].as<uint64_t>();

				std::string name;
				if (auto tagComponentNode = entityNode["TagComponent"])
				{
					name = tagComponentNode["Tag"].as<std::string>();
				}

				PRS_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				if (auto transformComponentNode = entityNode["TransformComponent"])
				{
					// Entities always have transform by default, no need to add one
					auto& [Translation, Rotation, Scale] = deserializedEntity.GetComponent<TransformComponent>();
					Translation = transformComponentNode["Translation"].as<glm::vec3>();
					Rotation = transformComponentNode["Rotation"].as<glm::vec3>();
					Scale = transformComponentNode["Scale"].as<glm::vec3>();
				}

				if (auto cameraComponentNode = entityNode["CameraComponent"])
				{
					auto& [Camera, Primary, FixedAspectRatio] = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponentNode["Camera"];

					Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<int>()));

					Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveVerticalFOV"].as<float>());
					Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					Primary = cameraComponentNode["Primary"].as<bool>();
					FixedAspectRatio = cameraComponentNode["FixedAspectRatio"].as<bool>();
				}

				if (auto scriptComponentNode = entityNode["ScriptComponent"])
				{
					auto& [ClassName] = deserializedEntity.AddComponent<ScriptComponent>();
					ClassName = scriptComponentNode["ClassName"].as<std::string>();

					// Fields
					if (auto scriptFields = scriptComponentNode["ScriptFields"])
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(ClassName);
						PRS_CORE_ASSERT(entityClass);
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);
						for (auto scriptField : scriptFields)
						{
							std::string fieldName = scriptField["Name"].as<std::string>();
							ScriptFieldType fieldType = Utils::ScriptFieldTypeFromString(scriptField["Type"].as<std::string>());
							if (fields.find(fieldName) == fields.end())
							{
								PRS_CORE_WARN("Script field '{0}' not found in class '{1}'", fieldName, ClassName);
								continue;
							}

							ScriptFieldInstance& fieldInstance = entityFields[fieldName];
							fieldInstance.SetField(fields.at(fieldName));

							switch (fieldType)
							{
								READ_SCRIPT_FIELD(Float,	float		);
								READ_SCRIPT_FIELD(Double,	double		);
								READ_SCRIPT_FIELD(Bool,		bool		);
								READ_SCRIPT_FIELD(Char,		char		);
								READ_SCRIPT_FIELD(Byte,		int8_t		);
								READ_SCRIPT_FIELD(Short,	int16_t		);
								READ_SCRIPT_FIELD(Int,		int32_t		);
								READ_SCRIPT_FIELD(Long,		int64_t		);
								READ_SCRIPT_FIELD(UByte,	uint8_t		);
								READ_SCRIPT_FIELD(UShort,	uint16_t	);
								READ_SCRIPT_FIELD(UInt,		uint32_t	);
								READ_SCRIPT_FIELD(ULong,	uint64_t	);
								READ_SCRIPT_FIELD(Vector2,	glm::vec2	);
								READ_SCRIPT_FIELD(Vector3,	glm::vec3	);
								READ_SCRIPT_FIELD(Vector4,	glm::vec4	);
								READ_SCRIPT_FIELD(Entity,	UUID		);
							}
						}
					}
				}

				if (auto spriteRendererComponentNode = entityNode["SpriteRendererComponent"])
				{
					auto& [Color, Texture, TilingFactor] = deserializedEntity.AddComponent<SpriteRendererComponent>();
					Color = spriteRendererComponentNode["Color"].as<glm::vec4>();

					if (spriteRendererComponentNode["TexturePath"])
					{
						Texture = Texture2D::Create(spriteRendererComponentNode["TexturePath"].as<std::string>());
					}
					
					if (spriteRendererComponentNode["TilingFactor"])
					{
						TilingFactor = spriteRendererComponentNode["TilingFactor"].as<float>();
					}
				}

				if (auto circleRendererComponentNode = entityNode["CircleRendererComponent"])
				{
					auto& [Color, Thickness, Fade] = deserializedEntity.AddComponent<CircleRendererComponent>();
					Color = circleRendererComponentNode["Color"].as<glm::vec4>();
					Thickness = circleRendererComponentNode["Thickness"].as<float>();
					Fade = circleRendererComponentNode["Fade"].as<float>();
				}

				if (auto rigidBody2DComponent = entityNode["RigidBody2DComponent"])
				{
					auto& [Type, FixedRotation, _] = deserializedEntity.AddComponent<RigidBody2DComponent>();
					Type = Utils::RigidBody2DBodyTypeFromString(rigidBody2DComponent["BodyType"].as<std::string>());
					FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
				}

				if (auto boxCollider2DComponent = entityNode["BoxCollider2DComponent"])
				{
					auto& [Offset, Size, Density, Friction, Restitution] = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					Density = boxCollider2DComponent["Density"].as<float>();
					Friction = boxCollider2DComponent["Friction"].as<float>();
					Restitution = boxCollider2DComponent["Restitution"].as<float>();
				}

				if (auto circleCollider2DComponent = entityNode["CircleCollider2DComponent"])
				{
					auto& [Offset, Radius, Density, Friction, Restitution] = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					Radius = circleCollider2DComponent["Radius"].as<float>();
					Density = circleCollider2DComponent["Density"].as<float>();
					Friction = circleCollider2DComponent["Friction"].as<float>();
					Restitution = circleCollider2DComponent["Restitution"].as<float>();
				}
			}

			PRS_CORE_TRACE("Deserializing scene '{0}' done.", sceneName);
		}
		catch (YAML::Exception& e)
		{
			PRS_CORE_ERROR("An error occurred while deserializing file '{0}'... Error: {1} at line {2}, column {3}", filePath, e.msg, e.mark.line, e.mark.column);
			return false;
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		//Not implemented
		PRS_CORE_ASSERT(false);
		return false;
	}

}
