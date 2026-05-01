#include <Core.h>

//todo add prefixed opcodes, and finish control flow instr
//todo finish testing all opcodes and improve CPU logic

//todo improve/ refactor the SST class
//	   make the class easier to use -> paths for testing and portability

int main() {
	Core core;
	if (!core.initialised()) {
		return -1;
	}

	if (!core.start_emu("C:\\Users\\ciaran\\Desktop\\test_roms\\gb-test-roms\\cpu_instrs\\individual\\07-jr,jp,call,ret,rst.gb", false)) {
		return -2;
	}

	while (true) {
		core.tick();
	}

	return 0;
}