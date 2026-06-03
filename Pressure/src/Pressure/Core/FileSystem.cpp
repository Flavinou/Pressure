#include "prspch.h"
#include "FileSystem.h"

namespace Pressure
{

	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filePath)
	{
		std::ifstream stream(filePath, std::ios::binary | std::ios::ate);
		if (!stream)
		{
			PRS_CORE_ERROR("Could not open file '{}'", filePath);
			return {};
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint64_t size = static_cast<uint64_t>(end - stream.tellg());
		if (size == 0)
		{
			PRS_CORE_ERROR("File '{}' is empty", filePath);
			return {};
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();
		return buffer;
	}

}
