#include "bgpch.h"
#include "FileSystem.h"

#include "Bubble/Math/Math.h"

namespace Bubble {

	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return {};
		}


		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint64_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return {};
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();
		return buffer;
	}

	static std::unordered_map<std::string, std::string> loadHashFileData(const std::string& filePath) {
		std::ifstream file(filePath);
		if (!file.is_open()) {
			BG_CORE_WARN("Unable to open file {0}", filePath);
			return {};
		}

		std::unordered_map<std::string, std::string> fileHashMap;
		std::string line;

		// Read each line
		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string filename, hash;

			// Extract the filename and hash (assuming they are separated by space)
			if (lineStream >> filename >> hash) {
				fileHashMap[filename] = hash;
			}
		}

		return fileHashMap;
	}

	static void updateHashFileData(const std::string& filepath, std::string& newHash)
	{
		std::unordered_map<std::string, std::string> fileHashMap = loadHashFileData("assets/cache/file_hashes.txt");

		// Open the file for writing
		std::ofstream file("assets/cache/file_hashes.txt");
		if (!file)
		{
			return;
		}

		// Update the hash data in memory
		fileHashMap[filepath] = newHash;

		// Write all data back to the file
		for (const auto& entry : fileHashMap) {
			file << entry.first << " " << entry.second << "\n";
		}
	}

	bool FileSystem::FileChanged(const std::filesystem::path& filepath)
	{
		// Open file to check
		std::ifstream stream(filepath);
		if (!stream)
		{
			BG_CORE_INFO("File {0} does not exist", filepath.string());
			return false;
		}

		// Hash file
		auto hashes = loadHashFileData("assets/cache/file_hashes.txt");
		if (hashes.size() == 0)
			return false;

		// Get sha inside file
		std::string sha_compare;
		for (const auto& hash : hashes)
		{
			if (hash.first == filepath.string())
			{
				sha_compare = hash.second;
			}
		}

		// Hash file
		std::stringstream buffer;
		buffer << stream.rdbuf();
		std::string sha = sha256(buffer.str());

		// Case 1: If the file hash doesn't exist, add it to the file
		if (sha_compare.empty()) {
			updateHashFileData(filepath.string(), sha);
			return false;
		}

		// Case 2: If the hash has changed, return true and update the hash file
		if (sha_compare != sha) {
			updateHashFileData(filepath.string(), sha);
			return true;
		}

		// If no changes detected
		return false;
	}

}
