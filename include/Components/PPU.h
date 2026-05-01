#pragma once

#include "../Definitions.h"

#include <memory>
#include <array>

struct PPUMemory {
	std::array<u8, 0x2000> vram = std::array<u8, 0x2000>();
	std::array<u8, 0xa0> oam = std::array<u8, 0xa0>();
};

class PPU {
public:
	/*   Constructor   */
	PPU();

	/*   Memory Access   */
	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	std::unique_ptr<PPUMemory> ram;
};