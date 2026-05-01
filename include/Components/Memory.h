#pragma once

#include "../Definitions.h"
#include "../Logger.h"

#include <memory>
#include <array>

struct memory {
	std::array<u8, 0x1000> wram0 = std::array<u8, 0x1000>();
	std::array<u8, 0x1000> wram1 = std::array<u8, 0x1000>();
	std::array<u8, 0x2000> echo = std::array<u8, 0x2000>();
	std::array<u8, 0x7e> hram = std::array<u8, 0x7e>();
};

class Memory {
public:
	/*   Constructor    */
	Memory();

	/*   Memory Access   */
	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	std::unique_ptr<memory> ram;
};