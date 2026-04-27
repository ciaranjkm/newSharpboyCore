#pragma once

#include "../Definitions.h"

#include <array>
#include <memory>

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
	//SST
	void start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram);
	void stop_sst_mode();

	//BUS REQ/RES
	BusResponse respond(BusRequest request);

	//SST
	SSTBusContext sst = {};

private:
	u8 read(u16 address);
	void write(u16 address, u8 value);
};