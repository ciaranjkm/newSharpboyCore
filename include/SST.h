#pragma once

#include "Logger.h"
#include "Loader.h"

#include "json.hpp"

#include <memory>
#include <vector>
#include <array>
#include <string>

#include "Components/CPU.h"
#include "Components/Bus.h"

struct SSTResult {
	bool success = false;
	int completed = 0;
	int failed = 0;
	std::vector<int> failed_tests = std::vector<int>();
};

struct SSTTest {
	std::string test_name = "";
	CPURegisters initial_registers = {};
	CPURegisters final_registers = {};
	std::vector<std::pair<u16, u8>> initial_memory = std::vector<std::pair<u16, u8>>();
	std::vector<std::pair<u16, u8>> final_memory = std::vector<std::pair<u16, u8>>();

	//std::vector<SSTCycle> cycles = std::vector<SSTCycle>();
};

const std::array<std::string, 256> opcodes = {
    // 0x00-0x0F
    "00", "01", "02", "03", "04", "05", "06", "07",
    "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    // 0x10-0x1F
    "10", "11", "12", "13", "14", "15", "16", "17",
    "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    // 0x20-0x2F
    "20", "21", "22", "23", "24", "25", "26", "27",
    "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    // 0x30-0x3F
    "30", "31", "32", "33", "34", "35", "36", "37",
    "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    // 0x40-0x4F
    "40", "41", "42", "43", "44", "45", "46", "47",
    "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    // 0x50-0x5F
    "50", "51", "52", "53", "54", "55", "56", "57",
    "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    // 0x60-0x6F
    "60", "61", "62", "63", "64", "65", "66", "67",
    "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    // 0x70-0x7F
    "70", "71", "72", "73", "74", "75", "76", "77",
    "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    // 0x80-0x8F
    "80", "81", "82", "83", "84", "85", "86", "87",
    "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    // 0x90-0x9F
    "90", "91", "92", "93", "94", "95", "96", "97",
    "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    // 0xA0-0xAF
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
    "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    // 0xB0-0xBF
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7",
    "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    // 0xC0-0xCF
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7",
    "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    // 0xD0-0xDF
    "D0", "D1", "D2", "", "D4", "D5", "D6", "D7",
    "D8", "D9", "DA", "", "DC", "", "DE", "DF",
    // 0xE0-0xEF
    "E0", "E1", "E2", "", "", "E5", "E6", "E7",
    "E8", "E9", "EA", "", "", "", "EE", "EF",
    // 0xF0-0xFF
    "F0", "F1", "F2", "F3", "", "F5", "F6", "F7",
    "F8", "F9", "FA", "FB", "", "", "FE", "FF",
};

class Core;

class SST {
public:
	SST(Core* core);
	~SST();

	bool initialised();

	void test(u8 opcode, bool prefix, SSTResult& results);

private:
	bool ready = false;
	Core* core;
	std::shared_ptr<std::array<u8, 0x10000>> sst_memory = nullptr;

private:
	int load_json_file(std::filesystem::path path, nlohmann::json& input);
	int load_registers_from_json(int test_number, bool initial, const nlohmann::json& json, CPURegisters& registers);
	int load_memory_from_json(int test_number, bool initial, const nlohmann::json& json, std::vector<std::pair<u16, u8>>& memory);
	//int load_cycles_from_json(int test_number, const nlohmann::json& json, std::vector<SSTCycle>& cycles);

	int check_registers(const CPURegisters& cpu_registers, const CPURegisters& final_registers);
	int check_memory(const std::vector<std::pair<u16, u8>>& final_memory);
	//int check_cycles(const std::vector<SSTCycle>& cpu_cycles, const std::vector<SSTCycle>& final_cycles);
};