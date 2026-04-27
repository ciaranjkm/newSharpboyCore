#pragma once

#include <string>
#include <format>
#include <iostream>

enum log_levels {
	log_info = 0,
	log_debug = 1,
	log_warning = 2,
	log_error = 3,
	log_fatal = 4
};

const std::string log_level_prefix[5] =
	{ "[CORE] ", "[CORE | DEBUG] ", "[CORE | WARNING] ", "[CORE | ERROR] ", "[CORE | FATAL] " };

inline void logger(const std::string& msg, log_levels log_level) {
	std::cout << log_level_prefix[log_level] << msg << "\n";
}