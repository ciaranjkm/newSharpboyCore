#include <Components/Cartridge.h>

/*   Constructor   */
Cartridge::Cartridge() {
	cart_memory = std::make_unique<CartMemory>();
}

/*   Memory Access   */
u8 Cartridge::read(u16 address) {
	if (address >= 0x0000 && address < 0x4000) {
		return cart_memory->rom[address];
	}
	else if (address >= 0x4000 && address < 0x8000) {
		if (address == 0x4244) {
			return cart_memory->rom[u16(address)];
		}

		return cart_memory->rom[u16(address)];
	}
	else if (address >= 0xa000 && address < 0xc000) {
		return cart_memory->ram[u16(address - 0xa000)];
	}
	else {
		return 0xff;
	}
}
void Cartridge::write(u16 address, u8 value) {
	//Writes to ROM ignored for now
	//todo bank switching mbc1-5

	if (address >= 0xa000 && address < 0xc000) {
		cart_memory->ram[u16(address - 0xa000)] = value;
	}
}

/*   Load ROMs   */
//todo :: load cart type into cart here from 0x147
void Cartridge::load_rom(std::span<u8> buffer) {
	cart_memory->rom.clear();
	cart_memory->rom.resize(buffer.size());
	cart_memory->rom.assign(buffer.begin(), buffer.end());
}
bool Cartridge::load_boot_rom(std::span<u8> buffer) {
	if (buffer.size() != 0x100) {
		return false;
	}

	cart_memory->boot_rom.fill(0x00);	
	for (int i = 0; i < 0x100; i++) {
		cart_memory->boot_rom[i] = buffer[i];
	}

	return true;
}
