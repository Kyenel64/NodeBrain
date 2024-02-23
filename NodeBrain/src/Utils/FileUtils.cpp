#include "NBpch.h"
#include "FileUtils.h"

#include <fstream>

namespace NodeBrain
{
	namespace Utils
	{
		std::vector<char> ReadFile(const std::filesystem::path& path)
		{
			NB_PROFILE_FN();

			std::ifstream file(path, std::ios::ate | std::ios::binary);
			NB_ASSERT(file.is_open(), "Could not open file");

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		}
	}
}