#ifndef UTILS_FILESYSTEM_H
#define UTILS_FILESYSTEM_H

#include <filesystem>

namespace Utils {
	void createDirectoryIfNeeded(const std::filesystem::path& path);

	std::filesystem::path findCorrespondingFile(
		const std::filesystem::path& filePath,
		const std::filesystem::path& sourceDirectory,
		const std::filesystem::path& searchDirectory
	);

	std::filesystem::path getRelativePath(
		const std::filesystem::path& path,
		const std::filesystem::path& directory
	);
}

#endif