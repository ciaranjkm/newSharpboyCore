#pragma once

#include "../Definitions.h"

#include <memory>
#include <array>
#include <vector>
#include <span>

struct CartMemory {
	std::vector<u8> rom = std::vector<u8>();
	std::array<u8, 0x2000> ram = std::array<u8, 0x2000>();
	std::array<u8, 0x100> boot_rom = std::array<u8, 0x100>();
};

class Cartridge {
public:
	/*   Constructor   */
	Cartridge();

	/*   Memory Access   */
	u8 read(u16 address);
	void write(u16 address, u8 value);

	/*   Load ROMs   */
	void load_rom(std::span<u8> buffer);
	bool load_boot_rom(std::span<u8> buffer);

private:
	std::unique_ptr<CartMemory> cart_memory;

};