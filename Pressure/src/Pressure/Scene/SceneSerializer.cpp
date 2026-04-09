#include "prspch.h"
#include "SceneSerializer.h"

#include "Components.h"
#include "Entity.h"

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

}

namespace Pressure
{

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

	static std::string RigidBody2DBodyTypeToString(RigidBody2DComponent::BodyType bodyType)
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

	static RigidBody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return RigidBody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return RigidBody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;

		PRS_CORE_ASSERT(false, "Unknown body type");
		return RigidBody2DComponent::BodyType::Static;
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

			SerializeEntity(out, entity);
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
				PRS_CORE_ERROR("An error occurred while parsing file '{0}'... Error: {1} at line {2}, column {3}", filePath, e.msg, e.mark.line, e.mark.column);
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
				auto tagComponentNode = entityNode["TagComponent"];
				if (tagComponentNode)
				{
					name = tagComponentNode["Tag"].as<std::string>();
				}

				PRS_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponentNode = entityNode["TransformComponent"];
				if (transformComponentNode)
				{
					// Entities always have transform by default, no need to add one
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponentNode["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponentNode["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponentNode["Scale"].as<glm::vec3>();
				}

				auto cameraComponentNode = entityNode["CameraComponent"];
				if (cameraComponentNode)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponentNode["Camera"];

					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveVerticalFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponentNode["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponentNode["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponentNode = entityNode["SpriteRendererComponent"];
				if (spriteRendererComponentNode)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponentNode["Color"].as<glm::vec4>();
				}

				auto rigidBody2DComponent = entityNode["RigidBody2DComponent"];
				if (rigidBody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<RigidBody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidBody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entityNode["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
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

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
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

			out << YAML::EndMap; // SpriteRendererComponent node
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

		out << YAML::EndMap; // Entity node
	}

}
