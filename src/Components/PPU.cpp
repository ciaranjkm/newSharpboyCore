#include <Components/PPU.h>

/*   Constructor   */
PPU::PPU() {
	ram = std::make_unique<PPUMemory>();
}

/*   Memory Access   */
u8 PPU::read(u16 address) {
	if (address >= 0x8000 && address < 0xa000) {
		return ram->vram[u16(address - 0x8000)];
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		return ram->oam[u16(address - 0xfe00)];
	}
	else {
		return 0xff;
	}
}
void PPU::write(u16 address, u8 value) {
	if (address >= 0x8000 && address < 0xa000) {
		ram->vram[u16(address - 0x8000)] = value;
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		ram->oam[u16(address - 0xfe00)] = value;
	}
}