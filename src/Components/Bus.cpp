#include <Components/Bus.h>

/*   Startup/Shutdown   */
bool Bus::set_ram(Memory* ram) {
	this->ram = ram;

	if (this->ram) {
		return true;
	}
	
	return false;
}
bool Bus::set_cart(Cartridge* cart) {
	this->cart = cart;

	if (this->cart) {
		return true;
	}

	return false;
}
bool Bus::set_ppu(PPU* ppu) {
	this->ppu = ppu;

	if (this->ppu) {
		return true;
	}

	return false;
}
void Bus::clear_ptrs() {
	this->ram = nullptr;
	this->cart = nullptr;
	this->ppu = nullptr;
}

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

	if (address >= 0x0000 && address < 0x8000) {
		return cart->read(address);
	}
	else if (address >= 0x8000 && address < 0xa000) {
		return ppu->read(address);
	}
	else if (address >= 0xa000 && address < 0xc000) {
		return cart->read(address);
	}
	else if (address >= 0xc000 && address < 0xe000) {
		return ram->read(address);
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		return ram->read(address);
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		return ppu->read(address);
	}
	else if (address >= 0xff00 && address < 0xff80) {
		return io_read(address);
	}
	else if (address >= 0xff80 && address < 0xffff) {
		return ram->read(address);
	}
	else if (address == 0xffff) {
		//return interrupts->get_ir_enable();
	}

	return 0xff;
}
void Bus::write(u16 address, u8 value) {
	if (sst.sst_mode) {
		if (address >= 0x0000 && address < sst.sst_memory->size()) {
			sst.sst_memory->data()[address] = value;
		}
	}

	if (address == 0xff02) {
		if (value == 0x81) {
			uint8_t ch = sb;
			std::cout << ch;
		}

		sc = 0x01;
		return;
	}
	if (address == 0xff01) {
		sb = value;
	}

	if (address >= 0x0000 && address < 0x8000) {
		cart->write(address, value);
	}
	else if (address >= 0x8000 && address < 0xa000) {
		ppu->write(address, value);
	}
	else if (address >= 0xa000 && address < 0xc000) {
		cart->write(address, value);
	}
	else if (address >= 0xc000 && address < 0xe000) {
		ram->write(address, value);
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		ram->write(address, value);
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		ppu->write(address, value);
	}
	else if (address >= 0xff00 && address < 0xff80) {
		io_write(address, value);
	}
	else if (address >= 0xff80 && address < 0xffff) {
		ram->write(address, value);
	}
	else if (address == 0xffff) {
		//interrupts->write_ie_enable();
	}
}

u8 Bus::io_read(u16 address) {
	u8 target = address - 0xff00;

	if (target == 0x44) {
		return 0x90;
	}

	if (address == 0x81) {
		return sb;
	}
	if (address == 0x82) {
		return sc;
	}

	switch (target) {
	case 0x00: 
		//joypad
		break;

	case 0x01: case 0x02:
		//serial
		break;

	case 0x04: case 0x05: case 0x06: case 0x07:
		//timer 
		break;

	case 0x0f:
		//interrupts
		break;

	case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
	case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26:
		//audio
		break;

	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
	case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		//wave audio
		break;

	case 0x46:
		//dma
		break;

	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48:
	case 0x49: case 0x4a: case 0x4b:
		//ppu
		break;

	case 0x50:
		//bank
		break;
	}

	return 0xff;
}
void Bus::io_write(u16 address, u8 value) {
	u8 target = address & 0xff;

	switch (target) {
	case 0x00:
		//joypad
		break;

	case 0x01: case 0x02:
		if (address == 0x81) {
			sb = value;
		}
		if (address == 0x82) {
			std::cout << putchar(sb) << "\n";
			std::cout.flush();
		}
		break;

	case 0x04: case 0x05: case 0x06: case 0x07:
		//timer 
		break;

	case 0x0f:
		//interrupts
		break;

	case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
	case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26:
		//audio
		break;

	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
	case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		//wave audio
		break;

	case 0x46:
		//dma
		break;

	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48:
	case 0x49: case 0x4a: case 0x4b:
		//ppu
		break;

	case 0x50:
		//bank
		break;
	}
}