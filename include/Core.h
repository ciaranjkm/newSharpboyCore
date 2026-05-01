#pragma once

#include "Definitions.h"
#include "Loader.h"
#include "Logger.h"

#include "SST.h"

#include "Components/CPU.h"
#include "Components/Bus.h"
#include "Components/Memory.h"
#include "Components/Cartridge.h"

struct CoreContext {
	bool initialised = false;
	int ticks = 0;

	bool boot_rom = false;
	bool rom_loaded = false;
	bool boot_rom_loaded = false;

	bool running = false;
	bool paused = false;
};

class Core {
public:
	/*   Startup + Shutdown   */
	Core();
	~Core();

	void startup();
	void shutdown();
	bool initialised();

	void load_rom(std::filesystem::path path);
	void load_boot_rom(std::filesystem::path path);

	bool start_emu(std::filesystem::path rom_path, bool using_boot_rom, std::filesystem::path boot_rom_path = "");

	/*   SST Functionality   */
	void start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram);
	void stop_sst_mode();
	void reset_for_next_sst(CPURegisters regs);
	SSTResult sst(u8 opcode, bool prefixed);

	/*   System Tick   */
	void tick();

	/*   Getters + Setters   */
	CPURegisters get_cpu_registers();

private:
	CoreContext core = {};

	CPU cpu;
	Memory ram;
	Cartridge cart;
	PPU ppu;
	Bus bus;
};