#pragma once

#include "Pressure/Core/Buffer.h"

namespace Pressure
{

	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filePath);
	};

}
