#include <Components/Memory.h>

/*   Constructor    */
Memory::Memory() {
	ram = std::make_unique<memory>();
}

/*   Memory Access   */
u8 Memory::read(u16 address) {
	if (address >= 0xc000 && address < 0xd000) {
		return ram->wram0[u16(address - 0xc000)];
	}
	else if (address >= 0xd000 && address < 0xe000) {
		return ram->wram1[u16(address - 0xd000)];
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		return ram->echo[u16(address - 0xe000)];
	}
	else if (address >= 0xff80 && address < 0xffff) {
		return ram->hram[u16(address - 0xff80)];
	}
	else {
		return 0xff;
	}
}
void Memory::write(u16 address, u8 value) {
	if (address >= 0xc000 && address < 0xd000) {
		ram->wram0[u16(address - 0xc000)] = value;
	}
	else if (address >= 0xd000 && address < 0xe000) {
		ram->wram1[u16(address - 0xd000)] = value;
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		ram->echo[u16(address - 0xe000)] = value;
	}
	else if (address >= 0xff80 && address < 0xffff) {
		ram->hram[u16(address - 0xff80)] = value;
	}
}