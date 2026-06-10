#include "prspch.h"
#include "Project.h"

#include "ProjectSerializer.h"

namespace Pressure
{

	Ref<Project> Project::New()
	{
		ms_ActiveProject = CreateRef<Project>();
		return ms_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			PRS_CORE_INFO("Project '{0}' loaded successfully from '{1}'", project->GetConfig().Name, path.string());

			project->m_ProjectDirectory = path.parent_path();
			ms_ActiveProject = project;
			return ms_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		if (!ms_ActiveProject)
		{
			PRS_CORE_ERROR("No active project to save");
			return false;
		}

		ProjectSerializer serializer(ms_ActiveProject);
		if (serializer.Serialize(path))
		{
			ms_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}
		return false;
	}

}
