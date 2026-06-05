#include "prspch.h"
#include "ProjectSerializer.h"

#include "Project.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Pressure
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filePath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap; // Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream file(filePath);
		if (!file)
		{
			PRS_CORE_ERROR("Failed to open file for writing: {}", filePath.string());
			return false;
		}

		file << out.c_str();
		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filePath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath.string());
		}
		catch (YAML::ParserException& e)
		{
			PRS_CORE_ERROR("An error occurred while parsing file '{0}'... Error: {1}", filePath.string(), e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
		{
			PRS_CORE_ERROR("Project node not found in file '{}'", filePath.string());
			return false;
		}

		if (projectNode["Name"])
			config.Name = projectNode["Name"].as<std::string>();
		if (projectNode["StartScene"])
			config.StartScene = projectNode["StartScene"].as<std::string>();
		if (projectNode["AssetDirectory"])
			config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		if (projectNode["ScriptModulePath"])
			config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
}
