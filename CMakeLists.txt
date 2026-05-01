cmake_minimum_required(VERSION 3.23)
project(NewSharpboyCore)

add_executable(nSharpboyCore)
target_sources(nSharpboyCore
	PRIVATE 
		"src/main.cpp"
		"src/Core.cpp"
		"src/SST.cpp"
		"src/Components/CPU.cpp"
		"src/Components/Bus.cpp"
		"src/Components/Memory.cpp"
		"src/Components/PPU.cpp"
		"src/Components/Cartridge.cpp"

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
			"include/SST.h"
			"include/Components/CPU.h" 
			"include/Components/Bus.h"
			"include/Components/Memory.h"
			
			"include/Components/Cartridge.h"  
			"include/Components/PPU.h" )

set_property(TARGET nSharpboyCore PROPERTY CXX_STANDARD 20)