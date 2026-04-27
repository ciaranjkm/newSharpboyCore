#pragma once

#include <Core.h>

Core::Core() {
	//INITIAL SETUP FOR COMPONENTS ETC
	bool success = true;
	ctx.initialised = false;

	if (success) {
		ctx.initialised = true;
		logger("Initialisation completed!", log_levels::log_info);
	}
}

Core::~Core() {
	//COMPONENT CLEANUP AND SHUTDOWN
	logger("Shutdown and cleanup completed!", log_levels::log_info);
	ctx.initialised = false;
}

bool Core::initialised() {
	return ctx.initialised;
}

//SST
SSTResult Core::sst(u8 opcode, bool prefixed) {
	SSTResult result = {};
	
	SST sst(&system);
	if (!sst.initialised()) {
		result.failed = true;

		return result;
	}

	sst.test(opcode, prefixed, result);

	std::cout << "Tests completed: " << (int)result.completed << " | Tests failed: " << (int)result.failed << "\n";
	/*
	std::cout << "Failed test numbers: ";
	for (int t : result.failed_tests) {
		std::cout << t;
	}
	*/
	std::cout << "\n";

	return result;
}