#pragma once

#include <string>
#include <vector>

namespace Pressure
{

	struct FileFilter
	{
		std::string FilterName;
		std::string FilterExtension;
	};

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const std::vector<FileFilter>& filters);
		static std::string SaveFile(const std::vector<FileFilter>& filters);
	};

}
