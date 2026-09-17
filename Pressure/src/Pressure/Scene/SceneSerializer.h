#pragma once

#include "Scene.h"

namespace Pressure
{

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filePath);
		void SerializeRuntime(const std::filesystem::path& filePath);

		bool Deserialize(const std::filesystem::path& filePath);
		bool DeserializeRuntime(const std::filesystem::path& filePath);
	private:
		Ref<Scene> m_Scene;
	};

}
