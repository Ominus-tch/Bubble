#include "bgpch.h"
#include "FileSystem.h"

#include "Bubble/Math/Math.h"

namespace Bubble {

	std::filesystem::path FileSystem::s_CachePath = "assets/cache/file_times.txt";
	std::vector<std::string> FileSystem::m_ChangedSinceLastRun = {};

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

	

	static std::unordered_map<std::string, std::time_t> LoadFileTimes(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file.is_open())
			return {};

		std::unordered_map<std::string, std::time_t> times;
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream stream(line);
			std::string filename;
			std::time_t timestamp;
			if (stream >> filename >> timestamp)
				times[filename] = timestamp;
		}
		return times;
	}

	static void SaveFileTimes(const std::unordered_map<std::string, std::time_t>& times, const std::string& filePath)
	{
		// Ensure the cache directory exists
		std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());

		std::ofstream file(filePath);
		if (!file.is_open()) {
			BG_CORE_WARN("Unable to open file for writing: {0}", filePath);
			return;
		}

		for (const auto& [path, time] : times) {
			file << path << " " << time << "\n";
		}
	}

	bool FileSystem::FileChanged(const std::filesystem::path& filepath)
	{
		if (std::find(m_ChangedSinceLastRun.begin(), m_ChangedSinceLastRun.end(), filepath.string()) != m_ChangedSinceLastRun.end())
		{
			BG_CORE_INFO("Changed since last run");
			return true;
		}

		auto fileTimes = LoadFileTimes(s_CachePath.string());

		// Check if file exists
		if (!std::filesystem::exists(filepath))
		{
			BG_CORE_INFO("File {0} does not exist", filepath.string());
			return false;
		}

		// Determine which root this file belongs to
		std::filesystem::path root;
		if (filepath.string().find("SandboxProject") != std::string::npos)
			root = "SandboxProject";
		else if (filepath.string().find("assets") != std::string::npos)
			root = "assets";
		else
		{
			BG_WARN("File {0} is outside tracked directories", filepath.string());
			return false;
		}

		// Create consistent cache key (e.g., "assets/shaders/my.glsl")
		std::string relative = std::filesystem::relative(filepath, root).string();
		std::string cacheKey = (root / relative).string();

		// Get current file modification time
		auto modTime = std::filesystem::last_write_time(filepath);
		auto duration = modTime.time_since_epoch();
		auto sysTime = std::chrono::system_clock::from_time_t(0) + duration;
		std::time_t currentTimestamp = std::chrono::system_clock::to_time_t(sysTime);

		// Compare with cached time
		bool changed = false;
		if (fileTimes.find(cacheKey) != fileTimes.end())
		{
			if (fileTimes[cacheKey] != currentTimestamp)
			{
				BG_CORE_INFO("File modified: {0}", cacheKey);
				fileTimes[cacheKey] = currentTimestamp;
				changed = true;
			}
		}
		else
		{
			BG_CORE_INFO("New file detected: {0}", cacheKey);
			fileTimes[cacheKey] = currentTimestamp;
			changed = true;
		}

		if (changed)
			SaveFileTimes(fileTimes, s_CachePath.string());

		return changed;
	}

	std::vector<std::string> FileSystem::ScanAssetsDirectory(bool initScan, const std::filesystem::path& projectDirectory, const std::filesystem::path& assetsDirectory)
	{
		auto fileTimes = LoadFileTimes(s_CachePath.string());
		std::unordered_set<std::string> currentFiles;
		std::vector<std::string> changedFiles;

		bool changed = false;

		// Helper lambda to scan a directory
		auto scanDir = [&](const std::filesystem::path& root)
			{
				for (auto& entry : std::filesystem::recursive_directory_iterator(root))
				{
					if (!entry.is_regular_file())
						continue;

					std::filesystem::path path = entry.path();

					// Skip cache subdirectory inside this root
					if (path.string().find((root / "cache").string()) != std::string::npos)
						continue;

					// Make the path relative to the root passed in, and prefix with root folder name for cache key uniqueness
					std::string relativePath = std::filesystem::relative(path, root).string();
					std::string key = (root.filename() / relativePath).string(); // e.g., "SandboxProject/textures/wood.png"

					currentFiles.insert(key);

					auto modTime = std::filesystem::last_write_time(path);
					auto duration = modTime.time_since_epoch();
					auto sysTime = std::chrono::system_clock::from_time_t(0) + duration;
					std::time_t timestamp = std::chrono::system_clock::to_time_t(sysTime);

					if (fileTimes.find(key) != fileTimes.end())
					{
						if (fileTimes[key] != timestamp)
						{
							BG_CORE_INFO("Modified: {0}", key);
							if (initScan)
							{
								m_ChangedSinceLastRun.push_back(key);
							}
							changedFiles.push_back(key);
							fileTimes[key] = timestamp;
							changed = true;
						}
					}
					else
					{
						BG_CORE_INFO("New file: {0}", key);
						if (initScan)
						{
							m_ChangedSinceLastRun.push_back(key);
						}
						fileTimes[key] = timestamp;
						changed = true;
					}
				}
			};

		// Scan both projectDirectory and assetsDirectory
		scanDir(projectDirectory);
		scanDir(assetsDirectory);

		// Detect deleted files
		std::vector<std::string> toRemove;
		for (const auto& [path, _] : fileTimes)
		{
			if (currentFiles.find(path) == currentFiles.end())
			{
				BG_CORE_INFO("Deleted: {0}", path);
				toRemove.push_back(path);
				changed = true;
			}
		}
		for (const auto& path : toRemove)
			fileTimes.erase(path);

		// Save only if changed
		if (changed)
			SaveFileTimes(fileTimes, s_CachePath.string());

		return changedFiles;
	}

}
