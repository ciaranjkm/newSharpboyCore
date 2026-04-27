cmake_minimum_required(VERSION 3.23)
project(NewSharpboyCore)

add_executable(nSharpboyCore)
target_sources(nSharpboyCore
	PRIVATE 
		"src/main.cpp"
		"src/Core.cpp"
		"src/Components/CPU.cpp"
		"src/Components/Bus.cpp"

		"src/SST.cpp"

	PUBLIC 
		FILE_SET coreHeaders
		TYPE HEADERS
		BASE_DIRS include	
		FILES
			
			"include/json.hpp"
		    "include/Loader.h"
			"include/Definitions.h" 
			"include/Logger.h" 
			
			"include/Core.h"
			"include/Components/CPU.h" 
			"include/Components/Bus.h"
			"include/SST.h" 
			
			"include/Components/InstructionDefinitions.h"
)

set_property(TARGET nSharpboyCore PROPERTY CXX_STANDARD 20)