#pragma once

#include "../Definitions.h"

#include <array>
#include <memory>

#include "Cartridge.h"
#include "Memory.h"
#include "PPU.h"

struct SSTBusContext {
	bool sst_mode = false;
	std::shared_ptr<std::array<u8, 0x10000>> sst_memory = nullptr;
};

struct BusRequest {
	bool idle = false;
	bool reading = false;
	bool commit = false;
	u8 value = 0x00;
	u16 address = 0x0000;
};

struct BusResponse {
	bool idle = false;
	u8 value = 0xff;
};

class Bus {
public:
	/*   Startup/Shutdown   */
	bool set_ram(Memory* ram);
	bool set_cart(Cartridge* cart);
	bool set_ppu(PPU* ppu);
	void clear_ptrs();

	//SST
	void start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram);
	void stop_sst_mode();

	//BUS REQ/RES
	BusResponse respond(BusRequest request);

	//SST
	SSTBusContext sst = {};

	Memory* ram = nullptr;
	Cartridge* cart = nullptr;
	PPU* ppu = nullptr;

	u8 sb = 0x00;
	u8 sc = 0x00;

	u8 read(u16 address);
	void write(u16 address, u8 value);

private:

	u8 io_read(u16 address);
	void io_write(u16 address, u8 value);
};