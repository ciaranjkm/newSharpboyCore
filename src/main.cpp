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

	for (int i = 0; i <= 255; i++) {
		core.sst(i, false);
	}

	return 0;
}