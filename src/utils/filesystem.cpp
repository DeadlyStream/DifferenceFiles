#include "filesystem.h"

namespace Utils {
	namespace fs = std::filesystem;

	void createDirectoryIfNeeded(const fs::path& path) {
		if( !fs::exists(path) )
			fs::create_directories(path);
	}

	fs::path findCorrespondingFile(const fs::path& filePath, const fs::path& sourceDirectory, const fs::path& searchDirectory) {
		return searchDirectory / getRelativePath(filePath, sourceDirectory);
	}

	fs::path getRelativePath(const fs::path& path, const fs::path& directory) {
		const auto relativePath = relative(path, directory);
		return relativePath.empty() ? path.filename() : relativePath;
	}
}