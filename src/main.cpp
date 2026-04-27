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

	//core.sst(0x40, true);
	
	
	for (int i = 0x00; i <= 0xff; i++) {
		core.sst(i, false);
	}

	for (int i = 0x00; i <= 0xff; i++) {
		core.sst(i, true);
	}
		

	return 0;
}