#pragma once

#include "Definitions.h"
#include "Logger.h"

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

inline int ldr_check_exists(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		logger(std::format("File not found, expected path: {}", path.generic_string()), log_levels::log_error);
		return -1;
	}

	return 0;
}

inline int ldr_check_extension(const std::filesystem::path& path, const std::string& target_extension) {
	if (path.filename().extension() != target_extension) {
		logger(std::format("File extension mismatch, expected extension: {}, real extension: {}", path.filename().extension().generic_string(), target_extension), log_levels::log_error);
		return -1;
	}

	return 0;
}

inline size_t ldr_get_size(const std::filesystem::path& path) {
	return std::filesystem::file_size(path);
}

inline int ldr_read_rom(const std::filesystem::path& path, std::vector<u8>& rom) {
	bool success = true;

	if (ldr_check_exists(path) < 0) {
		success = false;
	}

	if (ldr_check_extension(path, ".gb") < 0) {
		success = false;
	}

	if (!success) {
		return -1;
	}

	std::ifstream file_in{ path, std::ios::binary};
	rom.resize(ldr_get_size(path));

	if (!file_in) {
		logger(std::format("File could not be opened: {}", path.generic_string()), log_levels::log_error);
		return -1;
	}

	file_in.read((char*)&rom[0], rom.size());
	file_in.close();

	logger(std::format("File read success | {} | size in bytes: {}", path.filename().generic_string(), rom.size()), log_levels::log_info);
	return 0;
}