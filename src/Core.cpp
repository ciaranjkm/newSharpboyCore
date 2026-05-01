#pragma once

#include <Core.h>

/*   Startup/Shutdown   */
Core::Core() {
	startup();

	if (core.initialised) {
		logger("Initialisation completed!", log_levels::log_info);
		return;
	}

	shutdown();
}
Core::~Core() {
	shutdown();

	//COMPONENT CLEANUP AND SHUTDOWN
	logger("Shutdown and cleanup completed!", log_levels::log_info);
}

void Core::startup() {
	core.initialised = false;

	//Attach components to the bus
	if (!bus.set_ram(&ram)) {
		return;
	}
	if (!bus.set_cart(&cart)) {
		return;
	}
	if (!bus.set_ppu(&ppu)) {
		return;
	}

	//Attach bus to components
	/*
		ppu.attach_bus(&bus)
			return;
		dma.attach_bus(&bus)
			return;
	*/

	core.initialised = true;
}
void Core::shutdown() {
	bus.clear_ptrs();
}
bool Core::initialised() {
	return core.initialised;
}

void Core::load_rom(std::filesystem::path path) {
	core.rom_loaded = false;
	std::vector<u8> buffer;
	
	if (ldr_read_rom(path, buffer) < 0) {
		return;
	}

	cart.load_rom(buffer);
	logger(std::format("Loaded ROM successfully."), log_levels::log_info);
	logger(std::format("Path : {} || Bytes : {}", path.generic_string(), buffer.size()), log_levels::log_info);
	
	core.rom_loaded = true;
}
void Core::load_boot_rom(std::filesystem::path path) {
	core.boot_rom_loaded = false;
	std::vector<u8> buffer;
	if (ldr_read_rom(path, buffer) < 0) {
		return;
	}

	if (!cart.load_boot_rom(buffer)) {
		logger(std::format("Failed to push boot ROM into cartridge, {}", path.generic_string()), log_levels::log_error);
		return;
	}

	core.boot_rom_loaded = true;
}

bool Core::start_emu(std::filesystem::path rom_path, bool using_boot_rom, std::filesystem::path boot_rom_path) {
	//Set boot rom flag and load ROMs
	core.boot_rom = using_boot_rom;

	load_rom(rom_path);
	if (core.boot_rom) {
		load_boot_rom(boot_rom_path);
	}

	//Check if ROMs loaded correctly
	if (!core.rom_loaded || core.boot_rom_loaded) {
		return false;
	}

	//Reset other components
	cpu.reset(core.boot_rom);
	cpu.set_bus(&bus); //todo remove this!!!! testing only 
	//ppu.reset();
	//dma.reset();

	core.running = true;
	core.paused = false;

	return true;
}

/*   SST Functionality   */
void Core::start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram) {
	cpu.start_sst_mode();
	bus.start_sst_mode(ram);
}
void Core::stop_sst_mode() {
	cpu.stop_sst_mode();
	bus.stop_sst_mode();
}
void Core::reset_for_next_sst(CPURegisters regs) {
	cpu.load_registers(regs);
	cpu.reset_for_next_sst();
}
SSTResult Core::sst(u8 opcode, bool prefixed) {
	SSTResult result = {};
	
	SST sst(this);
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

/*   System Tick   */
void Core::tick() {
	if (!core.running) {
		return;
	}

	BusRequest req = cpu.get_bus_request();
	BusResponse res = bus.respond(req);

	cpu.action_bus_response(res);

	if (cpu.check_for_refetch()) {
		req = cpu.refetch_request();
		res = bus.respond(req);

		cpu.refetch_response(res);
	}
}

/*   Getters + Setters   */
CPURegisters Core::get_cpu_registers() {
	return cpu.get_registers();
}
