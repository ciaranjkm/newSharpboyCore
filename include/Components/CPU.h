#pragma once

#include "../Definitions.h"
#include "../Logger.h"
#include "InstructionDefinitions.h"

#include "Bus.h"

struct CPURegisters {
	u8 a = 0x00;
	u8 f = 0x00;
	u8 b = 0x00;
	u8 c = 0x00;
	u8 d = 0x00;
	u8 e = 0x00;
	u8 h = 0x00;
	u8 l = 0x00;

	u16 sp = 0x0000;
	u16 pc = 0x0000;

	void reset(bool boot_rom) {
		if (boot_rom) {
			a = 0x00;
			f = 0x00;
			b = 0x00;
			c = 0x00;
			d = 0x00;
			e = 0x00;
			h = 0x00;
			l = 0x00;

			return;
		}

		a = 0x01;
		f = 0x10; //todo:: check checksum for carry and half carry flags
		b = 0x00;
		c = 0x13;
		d = 0x00;
		e = 0xd8;
		h = 0x01;
		l = 0x4d;

		pc = 0x0100;
		sp = 0xfffe;
	}
};

enum CPUJoinedRegisters {
	rAF,
	rBC,
	rDE,
	rHL,
	rSP,
	rPC
};

enum CPUFlags {
	fZ = 7,
	fN = 6,
	fH = 5,
	fC = 4
};

enum CPUState {
	sFetch,
	sWrite,
	sRead,
	sExecute,
	sHalt,
	sStop,
	sInterrupt
};

struct CPUContext {
	bool sst_mode = false;
	
	CPUState state = sFetch;
	u8 instruction_length = 0x00;
	u8 t_cycles = 0;
	u8 m_cycles = 0;
	u8 opcode = 0x00;
	bool prefixed_opcode = false;
	bool prefix_check = false;

	bool jp_condition = false;
	
	u8 first_fetch = 0x00;
	u8 second_fetch = 0x00;

	bool ime = false;
	bool enable_ime = false;
	bool disable_ime = false;
};

class CPU {
public:
	//SST
	void start_sst_mode();
	void stop_sst_mode();
	void reset_for_next_sst();

	//REGISTERS
	void reset_registers(bool boot_rom);
	void load_registers(const CPURegisters& registers = {});
	CPURegisters get_registers();

	u16 get_joined_register(CPUJoinedRegisters registers);
	void set_joined_register(CPUJoinedRegisters registers, u16 value);

	//FLAGS
	bool get_flag(CPUFlags flag);
	void set_flag(CPUFlags flag, bool state);

	//EXECUTION
	BusRequest get_bus_request();
	void action_bus_response(BusResponse response);

private:
	CPUContext ctx = {};
	CPURegisters registers = {};

private:
	//EXECUTION
	void opcode_bus_request(BusRequest& request);
	void opcode_bus_response(BusResponse response);

	//FETCH
	void fetch_request(BusRequest& request);
	void fetch_response(BusResponse response);

	//OPCODES
	bool is_instruction_done();

	//OPCODE FUNCTIONS
	void idle(BusRequest& request);
	void read_pc(BusRequest& request);
	void read_rr(CPUJoinedRegisters reg, BusRequest& request);
	void read_nn(u16 address, BusRequest& request);

	void write_rr(CPUJoinedRegisters reg, BusRequest& request, u8 value);
	void write_nn(u16 address, BusRequest& request, u8 value);

	void add_r(u8 value);
	void adc_r(u8 value);

	void sub_r(u8 value);
	void sbc_r(u8 value);
	void cp_r(u8 value);

	void inc_r(u8& reg);
	void dec_r(u8& reg);

	void and_r(u8 value);
	void xor_r(u8 value);
	void or_r(u8 value);

	void daa();

	void jp_cc(bool condition, u8 response_value);
	void call_cc(bool conditon, u8 response_value);
	void ret_cc(bool condition, u8 response_value);
	void rst(u16 vector);

	void rlca();
	void rrca();
	void rla();
	void rra();
	void rlc_r(u8& reg);
	void rrc_r(u8& reg);
	void rl_r(u8& reg);
	void rr_r(u8& reg);
	void sla_r(u8& reg);
	void sra_r(u8& reg);
	void swap_r(u8& reg);
	void srl_r(u8& reg);
	bool bit(u8 b, u8 value);
	void res(u8 b, u8& reg);
	void set(u8 b, u8& reg);
};