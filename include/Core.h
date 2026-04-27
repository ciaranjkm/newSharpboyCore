#pragma once

#include "Definitions.h"
#include "Loader.h"
#include "Logger.h"

#include "SST.h"

#include "Components/CPU.h"
#include "Components/Bus.h"

struct CoreContext {
	bool initialised = false;
	bool running = false;
	bool paused = false;
};

struct System {
	CPU cpu;
	Bus bus;

	void start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram) {
		cpu.start_sst_mode();
		bus.start_sst_mode(ram);
	}

	void stop_sst_mode() {
		cpu.stop_sst_mode();
		bus.stop_sst_mode();
	}

	void tick() {
		BusRequest req = cpu.get_bus_request();
		BusResponse res = bus.respond(req);

		cpu.action_bus_response(res);
	}
};

class Core {
public:
	Core();
	~Core();

	bool initialised();

	//SST
	SSTResult sst(u8 opcode, bool prefixed);

private:
	CoreContext ctx = {};
	System system = {};

};