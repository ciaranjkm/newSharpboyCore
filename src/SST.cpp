#include <SST.h>

#include <Core.h>

SST::SST(System* system) {
	sst_memory = std::make_shared<std::array<u8, 0x10000>>();

	this->system = system;

	if (sst_memory) {
		if (this->system) {
			this->system->start_sst_mode(sst_memory);
			ready = true;
		}
	}
}

SST::~SST() {
	system->stop_sst_mode();
	system = nullptr;

	sst_memory.reset();
	sst_memory = nullptr;

	ready = false;
}

bool SST::initialised() {
	return ready;
}

void SST::test(u8 opcode, bool prefix, SSTResult& results) {
	using j = nlohmann::json;
	j test_json;

	std::string op = opcodes[opcode];
	std::filesystem::path path = "C:\\Users\\ciaran\\Documents\\Development\\SSTs\\sm83\\v1\\" + op + ".json";

	if (load_json_file(path, test_json) < 0) {
		results.success = false;
		results.failed = -1;
		return;
	}

	SSTTest test_case = {};

	std::cout << test_json[0]["name"].get<std::string>() << " ";

	for (int t = 0; t < 1000; t++) {	
		//LOAD REGISTERS FROM JSON
		if (load_registers_from_json(t, true, test_json, test_case.initial_registers) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);
			
			continue;
		}
		if (load_registers_from_json(t, false, test_json, test_case.final_registers) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);
			
			continue;
		}

		//LOAD MEMORY FROM JSON
		if (load_memory_from_json(t, true, test_json, test_case.initial_memory) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}
		if (load_memory_from_json(t, false, test_json, test_case.final_memory) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}

		//LOAD CYCLES FROM JSON
		/*
		if(load_cycles_from_json(t, test_json, test_case.cycles) < 0){
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}
		*/

		//LOAD REGISTERS INTO CPU + RESET CYCLES
		system->cpu.load_registers(test_case.initial_registers);
		system->cpu.reset_for_next_sst();

		//LOAD MEMORY INTO 64KB FLAT RAM
		for (std::pair<u16, u8> p : test_case.initial_memory) {
			sst_memory->data()[p.first] = p.second;
		}

		int length = instruction_lengths[opcode];
		if (prefix) {
			length = instruction_lengths[opcode];
		}

		//ADD CYCLES FOR FETCH PHASE + MULTIPLY FOR T CYCLES
		length += 1;
		length *= 4;

		//COMPLETE TEST INSTRUCTION
		for (int i = 0; i < length; i++) {
			system->tick();
		}

		//CHECK REGISTERS
		if (check_registers(system->cpu.get_registers(), test_case.final_registers) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}

		//CHECK MEMORY
		if (check_memory(test_case.final_memory) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}

		//CHECK CYCLES
		/*
		if (check_cycles(system->cpu.get_sst_cycles(), test_case.cycles) < 0) {
			results.failed++;
			results.failed_tests.push_back(t);

			continue;
		}
		*/

		//ADD FAIL/SUCCESS
		results.completed++;
	}

	if (results.completed != 1000) {
		results.success = false;
	}
	else {
		results.success = true;
	}

	return;
}

int SST::load_json_file(std::filesystem::path path, nlohmann::json& input) {
	using j = nlohmann::json;

	std::ifstream file_in(path);
	if (!file_in.is_open()) {
		logger(std::format("File could not be opened, {}", path.generic_string()), log_levels::log_error);
		return -1;
	}

	file_in >> input;
	return 0;
}

int SST::load_registers_from_json(int test_number, bool initial, const nlohmann::json& json, CPURegisters& registers) {
	if (test_number < 0 || test_number >= 1000) {
		return -1;
	}

	std::string initial_string = "initial";
	if (!initial) {
		initial_string = "final";
	}

	registers.pc = json[test_number][initial_string]["pc"].get<u16>();
	registers.sp = json[test_number][initial_string]["sp"].get<u16>();
	registers.a = json[test_number][initial_string]["a"].get<u16>();
	registers.f = json[test_number][initial_string]["f"].get<u16>();
	registers.b = json[test_number][initial_string]["b"].get<u16>();
	registers.c = json[test_number][initial_string]["c"].get<u16>();
	registers.d = json[test_number][initial_string]["d"].get<u16>();
	registers.e = json[test_number][initial_string]["e"].get<u16>();
	registers.h = json[test_number][initial_string]["h"].get<u16>();
	registers.l = json[test_number][initial_string]["l"].get<u16>();

	return 0;
}

int SST::load_memory_from_json(int test_number, bool initial, const nlohmann::json& json, std::vector<std::pair<u16, u8>>& memory) {
	if (test_number < 0 || test_number >= 1000) {
		return -1;
	}

	std::string initial_string = "initial";
	if (!initial) {
		initial_string = "final";
	}

	memory = json[test_number][initial_string]["ram"].get<std::vector<std::pair<u16, u8>>>();

	return 0;
}
/*
int SST::load_cycles_from_json(int test_number, const nlohmann::json& json, std::vector<SSTCycle>& cycles) {
	if (test_number < 0 || test_number >= 1000) {
		return -1;
	}

	cycles.clear();
	for (const auto& c : json[test_number]["cycles"]) {
		SSTCycle cycle = {};

		cycle.address = c[0].get<u16>();
		cycle.value = c[1].get<u8>();
		cycle.operation = c[2].get<std::string>();

		cycles.push_back(cycle);
	}

	return 0;
}
*/
int SST::check_registers(const CPURegisters& cpu_registers, const CPURegisters& final_registers) {
	if (cpu_registers.a != final_registers.a) {
		return -1;
	}
	if (cpu_registers.f != final_registers.f) {
		return -1;
	}
	if (cpu_registers.b != final_registers.b) {
		return -1;
	}
	if (cpu_registers.c != final_registers.c) {
		return -1;
	}
	if (cpu_registers.d != final_registers.d) {
		return -1;
	}
	if (cpu_registers.e != final_registers.e) {
		return -1;
	}
	if (cpu_registers.h != final_registers.h) {
		return -1;
	}
	if (cpu_registers.l != final_registers.l) {
		return -1;
	}
	if (cpu_registers.pc != final_registers.pc) {
		return -1;
	}
	if (cpu_registers.sp != final_registers.sp) {
		return -1;
	}

	return 0;
}

int SST::check_memory(const std::vector<std::pair<u16, u8>>& final_memory) {
	for (std::pair<u16, u8> p : final_memory) {
		if (sst_memory->data()[p.first] != p.second) {
			return -1;
		}
	}

	return 0;
}
/*
int SST::check_cycles(const std::vector<SSTCycle>& cpu_cycles, const std::vector<SSTCycle>& final_cycles) {
	if (cpu_cycles.size() != final_cycles.size()) {
		return -1;
	}

	for (int i = 0; i < cpu_cycles.size(); i++) {
		if (cpu_cycles[i].address != final_cycles[i].address) {
			return -1;
		}
		if (cpu_cycles[i].value != final_cycles[i].value) {
			return -1;
		}
		if (cpu_cycles[i].operation != final_cycles[i].operation) {
			return -1;
		}
	}

	return 0;
}
*/