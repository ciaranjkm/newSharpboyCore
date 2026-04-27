#include <Components/Bus.h>

//SST
void Bus::start_sst_mode(std::shared_ptr<std::array<u8, 0x10000>> ram) {
	sst.sst_memory = ram;
	sst.sst_mode = true;
}

void Bus::stop_sst_mode() {
	sst.sst_memory.reset();
	sst.sst_memory = nullptr;
}

//BUS REQ/RES
BusResponse Bus::respond(BusRequest request) {
	BusResponse response;
	
	if (request.idle) {
		response.idle = true;
	}
	else if (request.reading) {
		response.value = read(request.address);
	}
	else {
		if (request.commit) {
			write(request.address, request.value);
		}
	}
	
	return response;
}

u8 Bus::read(u16 address) {
	if (sst.sst_mode) {
		if (address >= 0x0000 && address < sst.sst_memory->size()) {
			return sst.sst_memory->data()[address];
		}

		return 0xff;
	}

	return 0xff;
}

void Bus::write(u16 address, u8 value) {
	if (sst.sst_mode) {
		if (address >= 0x0000 && address < sst.sst_memory->size()) {
			sst.sst_memory->data()[address] = value;
		}
	}
}