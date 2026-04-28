#include <Components/CPU.h>

/*   Single Step Test Functionality   */
void CPU::start_sst_mode() {
	ctx.sst_mode = true;
}
void CPU::stop_sst_mode() {
	ctx.sst_mode = false;
}
void CPU::reset_for_next_sst() {
	ctx = {};
	ctx.sst_mode = true;
	//cycles.clear();
}

/*   Get/Reset/Load Register Data   */
void CPU::reset(bool boot_rom) {
	registers.reset(boot_rom);
	ctx = {};
}
void CPU::load_registers(CPURegisters registers = {}) {
	this->registers = registers;
}
CPURegisters CPU::get_registers() {
	return registers;
}

/*   Get and Set 16 Bit Registers   */
u16 CPU::get_joined_register(CPUJoinedRegisters registers) {
	switch (registers) {
	case rAF:
		return u16(this->registers.a << 8 | this->registers.f);
	case rBC:
		return u16(this->registers.b << 8 | this->registers.c);
	case rDE:
		return u16(this->registers.d << 8 | this->registers.e);
	case rHL:
		return u16(this->registers.h << 8 | this->registers.l);
	case rSP:
		return this->registers.sp;
	case rPC:
		return this->registers.pc;
	default:
		return 0x0000;
	}
}
void CPU::set_joined_register(CPUJoinedRegisters registers, u16 value) {
	switch (registers) {
	case rAF:
		this->registers.a = (value >> 8) & 0xff;;
		this->registers.f = value & 0xf0;
		break;
	case rBC:
		this->registers.b = (value >> 8) & 0xff;;
		this->registers.c = value & 0xff;
		break;
	case rDE:
		this->registers.d = (value >> 8) & 0xff;;
		this->registers.e = value & 0xff;
		break;
	case rHL:
		this->registers.h = (value >> 8) & 0xff;;
		this->registers.l = value & 0xff;
		break;
	case rSP:
		this->registers.sp = value;
		break;
	case rPC:
		this->registers.pc = value;
		break;
	default:
		return;
	}
}

/*   Get and Set Flags Register   */
bool CPU::get_flag(CPUFlags flag) {
	return ((registers.f >> flag) & 0x01) != 0;
}
void CPU::set_flag(CPUFlags flag, bool state) {
	if (state) {
		registers.f |= (1 << flag);
	}
	else {
		registers.f &= ~(1 << flag);
	}
}

/*   Get Next Bus Request and Action Next Bus Response   */
BusRequest CPU::get_bus_request() {
	BusRequest request;

	switch (ctx.state) {
	case sFetch:
		fetch_request(request);
		return request;

	case sExecute:
		opcode_bus_request(request);
		return request;


	default:
		return request;
	}
}
void CPU::action_bus_response(BusResponse response) {
	switch (ctx.state) {
	case sFetch:
		fetch_response(response);
		break;

	case sExecute:
		opcode_bus_response(response);
		break;
	}
}

/*   Opcode Bus Request and Response   */
void CPU::opcode_bus_request(BusRequest& request) {
	if (ctx.prefixed_opcode) {
		switch (ctx.opcode) {
			//todo condense all hl top section into one case, all the same operations
			
			//RLC R
		case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07:
			break;

			//RLC HL
		case 0x06:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch);
			}
			break;

			//RRC R
		case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0f: 
			break;

			//RRC HL
		case 0x0e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//RL R
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: 
			break;

			//RL HL
		case 0x16:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//RR R
		case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1f:
			break;

			//RR HL
		case 0x1e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//SLA R
		case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27:
			break;

			//SLA HL
		case 0x26:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//SRA R
		case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2f: 
			break;

			//SRA HL
		case 0x2e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//SWAP R
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: 
			break;

			//SWAP HL
		case 0x36:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//SRL R
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c:case 0x3d: case 0x3f:
			break;

			//SRL HL
		case 0x3e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//BIT R
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f:
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f:
			break;

			//BIT HL
		case 0x46: case 0x4e: case 0x56: case 0x5e: case 0x66: case 0x6e: case 0x76: case 0x7e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//RES R
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f:
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f:
		case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa7: case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xaf:
		case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb7: case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbf:
			break;

			//RES HL
		case 0x86: case 0x8e: case 0x96: case 0x9e: case 0xa6: case 0xae: case 0xb6: case 0xbe:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;

			//SET R
		case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc7: case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xcf:
		case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd7: case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xdf:
		case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe7: case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xef:
		case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf7: case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xff:
			break;

			//SET HL
		case 0xc6: case 0xce: case 0xd6: case 0xde: case 0xe6: case 0xee: case 0xf6: case 0xfe:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, ctx.second_fetch); break;
			}
			break;
		}

		if (is_instruction_done()) {
			fetch_request(request);
		}
	}
	else {
		switch (ctx.opcode) {
		case 0x00:
			break;

			//LD R R 
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f:
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57:
		case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67:
		case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f:
		case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f:
			break;

			//LD R N
		case 0x06: case 0x16: case 0x26: case 0x0e: case 0x1e: case 0x2e: case 0x3e:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//LD R HL
		case 0x46: case 0x4e: case 0x56: case 0x5e: case 0x66: case 0x6e: case 0x7e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//LD HL R :: todo push this to a function
		case 0x70:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.b); break;
			}
			break;
		case 0x71:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.c); break;
			}
			break;
		case 0x72:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.d); break;
			}
			break;
		case 0x73:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.e); break;
			}
			break;
		case 0x74:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.h); break;
			}
			break;
		case 0x75:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.l); break;
			}
			break;
		case 0x77:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.a); break;
			}
			break;

			//LD HL N
		case 0x36:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: write_rr(rHL, request, ctx.first_fetch); break;
			}
			break;

			//LD A BC/DE
		case 0x0a:
			switch (ctx.m_cycles) {
			case 0: read_rr(rBC, request); break;
			}
			break;

		case 0x1a:
			switch (ctx.m_cycles) {
			case 0: read_rr(rDE, request); break;
			}
			break;

			//LD BC/DE A
		case 0x02:
			switch (ctx.m_cycles) {
			case 0: write_rr(rBC, request, registers.a); break;
			}
			break;
		case 0x12:
			switch (ctx.m_cycles) {
			case 0: write_rr(rDE, request, registers.a); break;
			}
			break;

			//LD A NN
		case 0xfa:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2: read_nn(u16((ctx.second_fetch << 8) | ctx.first_fetch), request); break;
			}
			break;

			//LD NN A
		case 0xea:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2: write_nn(u16((ctx.second_fetch << 8) | ctx.first_fetch), request, registers.a); break;
			}
			break;

			//LDH A (C)
		case 0xf2:
			switch (ctx.m_cycles) {
			case 0: read_nn(u16(0xff00 | registers.c), request); break;
			}
			break;

			//LDH (C) A
		case 0xe2:
			switch (ctx.m_cycles) {
			case 0: write_nn(u16(0xff00 | registers.c), request, registers.a); break;
			}
			break;

			//LDH A N
		case 0xf0:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_nn(u16(0xff00 | ctx.first_fetch), request); break;
			}
			break;

			//LDH N A
		case 0xe0:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: write_nn(u16(0xff00 | ctx.first_fetch), request, registers.a); break;
			}
			break;

			//LD A HL-
		case 0x3a:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//LD HL- A
		case 0x32:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.a); break;
			}
			break;

			//LD A HL+
		case 0x2a:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//LD HL+ A
		case 0x22:
			switch (ctx.m_cycles) {
			case 0: write_rr(rHL, request, registers.a); break;
			}
			break;

			//LD RR NN
		case 0x01: case 0x11: case 0x21: case 0x31:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			}
			break;

			//LD NN SP
		case 0x08:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2: write_nn(u16((ctx.second_fetch << 8) | ctx.first_fetch), request, u8(registers.sp & 0xff)); break;
			case 3: write_nn(u16((ctx.second_fetch << 8) | ctx.first_fetch), request, u8(registers.sp >> 8)); break;
			}
			break;

			//LD  SP HL
		case 0xf9:
			switch (ctx.m_cycles) {
			case 0: request.address = get_joined_register(rHL); request.idle = true; break;
			}
			break;

			//PUSH RR :: todo push this to a function
		case 0xc5:
			switch (ctx.m_cycles) {
			case 0: request.address = registers.sp; request.idle; break;
			case 1: write_nn(registers.sp, request, registers.b); break;
			case 2: write_nn(registers.sp, request, registers.c); break;
			}
			break;
		case 0xd5:
			switch (ctx.m_cycles) {
			case 0: request.address = registers.sp; request.idle; break;
			case 1: write_nn(registers.sp, request, registers.d); break;
			case 2: write_nn(registers.sp, request, registers.e); break;
			}
			break;
		case 0xe5:
			switch (ctx.m_cycles) {
			case 0: request.address = registers.sp; request.idle; break;
			case 1: write_nn(registers.sp, request, registers.h); break;
			case 2: write_nn(registers.sp, request, registers.l); break;
			}
			break;
		case 0xf5:
			switch (ctx.m_cycles) {
			case 0: request.address = registers.sp; request.idle; break;
			case 1: write_nn(registers.sp, request, registers.a); break;
			case 2: write_nn(registers.sp, request, registers.f & 0xf0); break;
			}
			break;

			//POP RR
		case 0xc1: case 0xd1: case 0xe1: case 0xf1:
			switch (ctx.m_cycles) {
			case 0: read_nn(registers.sp, request); break;
			case 1: read_nn(registers.sp, request); break;
			}
			break;

			//LD HL SP+E
		case 0xf8:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: request.address = 0x0000; request.idle = true; break;

			}
			break;

			//ADD R
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87:
			break;

			//ADD HL
		case 0x86:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//ADD N
		case 0xc6:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//ADC R
		case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f:
			break;

			//ADC HL
		case 0x8e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//ADC N
		case 0xce:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//SUB R
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97:
			break;

			//SUB HL
		case 0x96:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//SUB N
		case 0xd6:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//SBC R
		case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9f:
			break;

			//SBC HL
		case 0x9e:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//SBC N
		case 0xde:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//CP R
		case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbf:
			break;

			//CP HL
		case 0xbe:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//CP N
		case 0xfe:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//INC R
		case 0x04: case 0x0c: case 0x14: case 0x1c: case 0x24: case 0x2c: case 0x3c:
			break;

			//INC (HL)
		case 0x34:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, u8(ctx.first_fetch + 1)); break;
			}
			break;

			//DEC R
		case 0x05: case 0x0d: case 0x15: case 0x1d: case 0x25: case 0x2d: case 0x3d:
			break;

			//DEC (HL)
		case 0x35:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			case 1: write_rr(rHL, request, u8(ctx.first_fetch - 1)); break;
			}
			break;

			//AND R
		case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa7:
			break;

			//AND HL
		case 0xa6:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//AND N
		case 0xe6:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//OR R
		case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5:case 0xb7:
			break;

			//OR HL
		case 0xb6:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//OR N
		case 0xf6:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//XOR R
		case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xaf:
			break;

			//XOR HL
		case 0xae:
			switch (ctx.m_cycles) {
			case 0: read_rr(rHL, request); break;
			}
			break;

			//XOR N
		case 0xee:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			}
			break;

			//CCF
		case 0x3f: break;
			//SCF
		case 0x37: break;
			//DAA
		case 0x27: break;
			//CPL
		case 0x2f: break;

			//INC RR
		case 0x03:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rBC); break;
			}
			break;
		case 0x13:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rDE); break;
			}
			break;
		case 0x23:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rHL); break;
			}
			break;
		case 0x33: request.idle = true; request.address = registers.sp; break;

			//DEC RR
		case 0x0b:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rBC); break;
			}
			break;
		case 0x1b:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rDE); break;
			}
			break;
		case 0x2b:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = get_joined_register(rHL); break;
			}
			break;
		case 0x3b: request.idle = true; request.address = registers.sp; break;

			//ADD HL RR
		case 0x09: case 0x19: case 0x29: case 0x39:
			switch (ctx.m_cycles) {
			case 0: request.address = 0x0000; request.idle = true; break;
			}
			break;

			//ADD SP E
		case 0xe8:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: request.address = 0x0000; request.idle = true; break;
			case 2: request.address = 0x0000; request.idle = true; break;
			}
			break;

			//JP NN
		case 0xc3:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2: request.address = 0x0000; request.idle = true;
			}
			break;

			//JP HL
		case 0xe9: break;

			//JP CC NN
		case 0xc2: case 0xca: case 0xd2: case 0xda:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2:
				if (ctx.jp_condition) {
					request.idle = true;
					request.address = 0x0000;
				}
				break;
			}
			break;

			//JR E
		case 0x18:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: request.idle = true; request.address = (registers.pc >> 8); break;
			}
			break;

			//JR CC E
		case 0x20: case 0x28: case 0x30: case 0x38:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1:
				if (ctx.jp_condition) {
					request.idle = true;
					request.address = (registers.pc >> 8);
				}
				break;
			}
			break;

			//CALL NN
		case 0xcd:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2: request.idle = true; request.address = registers.sp; break;
			case 3: write_rr(rSP, request, (registers.pc >> 8)); break;
			case 4: write_rr(rSP, request, (registers.pc & 0xff)); break;
			}
			break;

			//CALL CC NN
		case 0xc4: case 0xcc: case 0xd4: case 0xdc:
			switch (ctx.m_cycles) {
			case 0: read_pc(request); break;
			case 1: read_pc(request); break;
			case 2:
				if (ctx.jp_condition) {
					request.idle = true;
					request.address = registers.sp;
				}
				break;
			case 3:
				if (!ctx.jp_condition) break;
				write_rr(rSP, request, (registers.pc >> 8)); break;
			case 4:
				if (!ctx.jp_condition) break;
				write_rr(rSP, request, (registers.pc & 0xff)); break;
			}
			break;

			//RET
		case 0xc9:
			switch (ctx.m_cycles) {
			case 0: read_rr(rSP, request); break;
			case 1: read_rr(rSP, request); break;
			case 2: request.address = 0x0000; request.idle = false; break;
			}
			break;

			//RET CC
		case 0xc0: case 0xc8: case 0xd0: case 0xd8:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = 0x0000; break;
			case 1:
				if (!ctx.jp_condition) break;
				read_rr(rSP, request); break;
			case 2:
				if (!ctx.jp_condition) break;
				read_rr(rSP, request); break;
			case 3:
				if (!ctx.jp_condition) break;
				request.idle = true; request.address = 0x0000; break;
			}
			break;

			//RETI 
		case 0xd9:
			switch (ctx.m_cycles) {
			case 0: read_rr(rSP, request); break;
			case 1: read_rr(rSP, request); break;
			case 2: request.address = 0x0000; request.idle = false; break;
			}
			break;

			//RST
		case 0xc7: case 0xcf: case 0xd7: case 0xdf: case 0xe7: case 0xef: case 0xf7: case 0xff:
			switch (ctx.m_cycles) {
			case 0: request.idle = true; request.address = registers.sp; break;
			case 1: write_rr(rSP, request, (registers.pc >> 8)); break;
			case 2: write_rr(rSP, request, (registers.pc & 0xff)); break;
			}
			break;

			//RLCA
		case 0x07: break;

			//RRCA
		case 0x0f: break;

			//RLA
		case 0x17: break;

			//RRA 
		case 0x1f: break;
		}

		if (is_instruction_done()) {
			fetch_request(request);
		}
	}
}
void CPU::opcode_bus_response(BusResponse response) {
	u8 e = 0x00; //this is annoying but id rather not redclare in every switch :(
	u8 adj = 0x00;
	u8 res8 = 0x00;
	u16 res16 = 0x00;

	//OH NO AN EXTRA 40 bits :O
	

	if (ctx.t_cycles == 3) {
		if (ctx.prefixed_opcode) {
			switch (ctx.opcode) {
				//RLC R
			case 0x00: rlc_r(registers.b); break;
			case 0x01: rlc_r(registers.c); break;
			case 0x02: rlc_r(registers.d); break;
			case 0x03: rlc_r(registers.e); break;
			case 0x04: rlc_r(registers.h); break;
			case 0x05: rlc_r(registers.l); break;
			case 0x07: rlc_r(registers.a); break;

				//RLC HL
			case 0x06:
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value; 
					ctx.second_fetch = (ctx.first_fetch << 1) | ((ctx.first_fetch >> 7) & 0x01);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if ((ctx.first_fetch >> 7) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//RRC R
			case 0x08: rrc_r(registers.b); break;
			case 0x09: rrc_r(registers.c); break;
			case 0x0a: rrc_r(registers.d); break;
			case 0x0b: rrc_r(registers.e); break;
			case 0x0c: rrc_r(registers.h); break;
			case 0x0d: rrc_r(registers.l); break;
			case 0x0f: rrc_r(registers.a); break;

				//RRC HL
			case 0x0e:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value; 
					ctx.second_fetch = (ctx.first_fetch << 7) | (ctx.first_fetch >> 1);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x01) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//RL R
			case 0x10: rl_r(registers.b); break;
			case 0x11: rl_r(registers.c); break;
			case 0x12: rl_r(registers.d); break;
			case 0x13: rl_r(registers.e); break;
			case 0x14: rl_r(registers.h); break;
			case 0x15: rl_r(registers.l); break;
			case 0x17: rl_r(registers.a); break;

				//RL HL
			case 0x16:
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value; 
					ctx.second_fetch = (ctx.first_fetch << 1) | (get_flag(fC) ? 0x01 : 0x00);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if (((ctx.first_fetch >> 7) & 0x01) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//RR R
			case 0x18: rr_r(registers.b); break;
			case 0x19: rr_r(registers.c); break;
			case 0x1a: rr_r(registers.d); break;
			case 0x1b: rr_r(registers.e); break;
			case 0x1c: rr_r(registers.h); break;
			case 0x1d: rr_r(registers.l); break;
			case 0x1f: rr_r(registers.a); break;

				//RR HL
			case 0x1e:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value; 
					ctx.second_fetch = ((get_flag(fC) ? 0x01 : 0x00) << 7) | (ctx.first_fetch >> 1);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x01) != 0x00) set_flag(fC, true);
				}
				break;
				
				//SLA R
			case 0x20: sla_r(registers.b); break;
			case 0x21: sla_r(registers.c); break;
			case 0x22: sla_r(registers.d); break;
			case 0x23: sla_r(registers.e); break;
			case 0x24: sla_r(registers.h); break;
			case 0x25: sla_r(registers.l); break;
			case 0x27: sla_r(registers.a); break;

				//SLA HL
			case 0x26:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					ctx.second_fetch = (ctx.first_fetch << 1) | 0x00;
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if (((ctx.first_fetch >> 7) & 0x01) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//SRA R
			case 0x28: sra_r(registers.b); break;
			case 0x29: sra_r(registers.c); break;
			case 0x2a: sra_r(registers.d); break;
			case 0x2b: sra_r(registers.e); break;
			case 0x2c: sra_r(registers.h); break;
			case 0x2d: sra_r(registers.l); break;
			case 0x2f: sra_r(registers.a); break;

				//SRA HL //todo fix this <<<< 
			case 0x2e:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					ctx.second_fetch = (ctx.first_fetch & 0x80) | (ctx.first_fetch >> 1);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x01) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//SWAP R
			case 0x30: swap_r(registers.b); break;
			case 0x31: swap_r(registers.c); break;
			case 0x32: swap_r(registers.d); break;
			case 0x33: swap_r(registers.e); break;
			case 0x34: swap_r(registers.h); break;
			case 0x35: swap_r(registers.l); break;
			case 0x37: swap_r(registers.a); break;

				//SWAP HL
			case 0x36:
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value;
					ctx.second_fetch = (ctx.first_fetch >> 4) | (ctx.first_fetch << 4);
					break;
				case 1:
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					break;
				}
				break;

				//SRL R
			case 0x38: srl_r(registers.b); break;
			case 0x39: srl_r(registers.c); break;
			case 0x3a: srl_r(registers.d); break;
			case 0x3b: srl_r(registers.e); break;
			case 0x3c: srl_r(registers.h); break;
			case 0x3d: srl_r(registers.l); break;
			case 0x3f: srl_r(registers.a); break;
				break;

				//SRL HL
			case 0x3e:
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value;
					ctx.second_fetch = 0x00 | (ctx.first_fetch >> 1);
					break;
				case 1: 
					registers.f = 0x00;
					if (ctx.second_fetch == 0x00) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x01) != 0x00) set_flag(fC, true);
					break;
				}
				break;

				//BIT
			case 0x40: bit(0, registers.b); break; 
			case 0x41: bit(0, registers.c); break; 
			case 0x42: bit(0, registers.d); break;
			case 0x43: bit(0, registers.e); break;
			case 0x44: bit(0, registers.h); break;
			case 0x45: bit(0, registers.l); break;
			case 0x47: bit(0, registers.a); break;
			case 0x48: bit(1, registers.b); break;
			case 0x49: bit(1, registers.c); break;
			case 0x4a: bit(1, registers.d); break;
			case 0x4b: bit(1, registers.e); break;
			case 0x4c: bit(1, registers.h); break;
			case 0x4d: bit(1, registers.l); break;
			case 0x4f: bit(1, registers.a); break;

			case 0x50: bit(2, registers.b); break;
			case 0x51: bit(2, registers.c); break;
			case 0x52: bit(2, registers.d); break;
			case 0x53: bit(2, registers.e); break;
			case 0x54: bit(2, registers.h); break;
			case 0x55: bit(2, registers.l); break;
			case 0x57: bit(2, registers.a); break;
			case 0x58: bit(3, registers.b); break;
			case 0x59: bit(3, registers.c); break;
			case 0x5a: bit(3, registers.d); break;
			case 0x5b: bit(3, registers.e); break;
			case 0x5c: bit(3, registers.h); break;
			case 0x5d: bit(3, registers.l); break;
			case 0x5f: bit(3, registers.a); break;

			case 0x60: bit(4, registers.b); break;
			case 0x61: bit(4, registers.c); break;
			case 0x62: bit(4, registers.d); break;
			case 0x63: bit(4, registers.e); break;
			case 0x64: bit(4, registers.h); break;
			case 0x65: bit(4, registers.l); break;
			case 0x67: bit(4, registers.a); break;
			case 0x68: bit(5, registers.b); break;
			case 0x69: bit(5, registers.c); break;
			case 0x6a: bit(5, registers.d); break;
			case 0x6b: bit(5, registers.e); break;
			case 0x6c: bit(5, registers.h); break;
			case 0x6d: bit(5, registers.l); break;
			case 0x6f: bit(5, registers.a); break;

			case 0x70: bit(6, registers.b); break;
			case 0x71: bit(6, registers.c); break;
			case 0x72: bit(6, registers.d); break;
			case 0x73: bit(6, registers.e); break;
			case 0x74: bit(6, registers.h); break;
			case 0x75: bit(6, registers.l); break;
			case 0x77: bit(6, registers.a); break;
			case 0x78: bit(7, registers.b); break;
			case 0x79: bit(7, registers.c); break;
			case 0x7a: bit(7, registers.d); break;
			case 0x7b: bit(7, registers.e); break;
			case 0x7c: bit(7, registers.h); break;
			case 0x7d: bit(7, registers.l); break;
			case 0x7f: bit(7, registers.a); break;

				//BIT HL
			case 0x46: 
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(0, ctx.first_fetch); break;
				}
				break;
			case 0x4e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(1, ctx.first_fetch); break;
				}
				break;
			case 0x56:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(2, ctx.first_fetch); break;
				}
				break;
			case 0x5e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(3, ctx.first_fetch); break;
				}
				break;
			case 0x66:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(4, ctx.first_fetch); break;
				}
				break;
			case 0x6e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(5, ctx.first_fetch); break;
				}
				break;
			case 0x76:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(6, ctx.first_fetch); break;
				}
				break;
			case 0x7e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: bit(7, ctx.first_fetch); break;
				}
			    break;

				//RES R
			case 0x80: res(0, registers.b); break;
			case 0x81: res(0, registers.c); break;
			case 0x82: res(0, registers.d); break;
			case 0x83: res(0, registers.e); break;
			case 0x84: res(0, registers.h); break;
			case 0x85: res(0, registers.l); break;
			case 0x87: res(0, registers.a); break;
			case 0x88: res(1, registers.b); break;
			case 0x89: res(1, registers.c); break;
			case 0x8a: res(1, registers.d); break;
			case 0x8b: res(1, registers.e); break;
			case 0x8c: res(1, registers.h); break;
			case 0x8d: res(1, registers.l); break;
			case 0x8f: res(1, registers.a); break;

			case 0x90: res(2, registers.b); break;
			case 0x91: res(2, registers.c); break;
			case 0x92: res(2, registers.d); break;
			case 0x93: res(2, registers.e); break;
			case 0x94: res(2, registers.h); break;
			case 0x95: res(2, registers.l); break;
			case 0x97: res(2, registers.a); break;
			case 0x98: res(3, registers.b); break;
			case 0x99: res(3, registers.c); break;
			case 0x9a: res(3, registers.d); break;
			case 0x9b: res(3, registers.e); break;
			case 0x9c: res(3, registers.h); break;
			case 0x9d: res(3, registers.l); break;
			case 0x9f: res(3, registers.a); break;

			case 0xa0: res(4, registers.b); break;
			case 0xa1: res(4, registers.c); break;
			case 0xa2: res(4, registers.d); break;
			case 0xa3: res(4, registers.e); break;
			case 0xa4: res(4, registers.h); break;
			case 0xa5: res(4, registers.l); break;
			case 0xa7: res(4, registers.a); break;
			case 0xa8: res(5, registers.b); break;
			case 0xa9: res(5, registers.c); break;
			case 0xaa: res(5, registers.d); break;
			case 0xab: res(5, registers.e); break;
			case 0xac: res(5, registers.h); break;
			case 0xad: res(5, registers.l); break;
			case 0xaf: res(5, registers.a); break;

			case 0xb0: res(6, registers.b); break;
			case 0xb1: res(6, registers.c); break;
			case 0xb2: res(6, registers.d); break;
			case 0xb3: res(6, registers.e); break;
			case 0xb4: res(6, registers.h); break;
			case 0xb5: res(6, registers.l); break;
			case 0xb7: res(6, registers.a); break;
			case 0xb8: res(7, registers.b); break;
			case 0xb9: res(7, registers.c); break;
			case 0xba: res(7, registers.d); break;
			case 0xbb: res(7, registers.e); break;
			case 0xbc: res(7, registers.h); break;
			case 0xbd: res(7, registers.l); break;
			case 0xbf: res(7, registers.a); break;

				//RES HL
			case 0x86: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(0, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0x8e: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(1, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0x96: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(2, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0x9e: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(3, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xa6: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(4, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xae: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(5, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xb6: 
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					res(6, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xbe:
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value; 
					res(7, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;

				//SET R
			case 0xc0: set(0, registers.b); break;
			case 0xc1: set(0, registers.c); break;
			case 0xc2: set(0, registers.d); break;
			case 0xc3: set(0, registers.e); break;
			case 0xc4: set(0, registers.h); break;
			case 0xc5: set(0, registers.l); break;
			case 0xc7: set(0, registers.a); break;
			case 0xc8: set(1, registers.b); break;
			case 0xc9: set(1, registers.c); break;
			case 0xca: set(1, registers.d); break;
			case 0xcb: set(1, registers.e); break;
			case 0xcc: set(1, registers.h); break;
			case 0xcd: set(1, registers.l); break;
			case 0xcf: set(1, registers.a); break;

			case 0xd0: set(2, registers.b); break;
			case 0xd1: set(2, registers.c); break;
			case 0xd2: set(2, registers.d); break;
			case 0xd3: set(2, registers.e); break;
			case 0xd4: set(2, registers.h); break;
			case 0xd5: set(2, registers.l); break;
			case 0xd7: set(2, registers.a); break;
			case 0xd8: set(3, registers.b); break;
			case 0xd9: set(3, registers.c); break;
			case 0xda: set(3, registers.d); break;
			case 0xdb: set(3, registers.e); break;
			case 0xdc: set(3, registers.h); break;
			case 0xdd: set(3, registers.l); break;
			case 0xdf: set(3, registers.a); break;

			case 0xe0: set(4, registers.b); break;
			case 0xe1: set(4, registers.c); break;
			case 0xe2: set(4, registers.d); break;
			case 0xe3: set(4, registers.e); break;
			case 0xe4: set(4, registers.h); break;
			case 0xe5: set(4, registers.l); break;
			case 0xe7: set(4, registers.a); break;
			case 0xe8: set(5, registers.b); break;
			case 0xe9: set(5, registers.c); break;
			case 0xea: set(5, registers.d); break;
			case 0xeb: set(5, registers.e); break;
			case 0xec: set(5, registers.h); break;
			case 0xed: set(5, registers.l); break;
			case 0xef: set(5, registers.a); break;

			case 0xf0: set(6, registers.b); break;
			case 0xf1: set(6, registers.c); break;
			case 0xf2: set(6, registers.d); break;
			case 0xf3: set(6, registers.e); break;
			case 0xf4: set(6, registers.h); break;
			case 0xf5: set(6, registers.l); break;
			case 0xf7: set(6, registers.a); break;
			case 0xf8: set(7, registers.b); break;
			case 0xf9: set(7, registers.c); break;
			case 0xfa: set(7, registers.d); break;
			case 0xfb: set(7, registers.e); break;
			case 0xfc: set(7, registers.h); break;
			case 0xfd: set(7, registers.l); break;
			case 0xff: set(7, registers.a); break;

				//SET HL
			case 0xc6: 
				switch (ctx.m_cycles) {
				case 0: 
					ctx.first_fetch = response.value;
					set(0, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xce:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(1, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xd6:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(2, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xde:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(3, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xe6:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(4, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xee:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(5, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xf6:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(6, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			case 0xfe:
				switch (ctx.m_cycles) {
				case 0:
					ctx.first_fetch = response.value;
					set(7, ctx.first_fetch);
					ctx.second_fetch = ctx.first_fetch;
					break;
				}
				break;
			}
		}
		else {
			switch (ctx.opcode) {
				//MISC OPCODES
			case 0x00: break;

				//LD R R
			case 0x40: registers.b = registers.b; break;
			case 0x41: registers.b = registers.c; break;
			case 0x42: registers.b = registers.d; break;
			case 0x43: registers.b = registers.e; break;
			case 0x44: registers.b = registers.h; break;
			case 0x45: registers.b = registers.l; break;
			case 0x47: registers.b = registers.a; break;
			case 0x48: registers.c = registers.b; break;
			case 0x49: registers.c = registers.c; break;
			case 0x4a: registers.c = registers.d; break;
			case 0x4b: registers.c = registers.e; break;
			case 0x4c: registers.c = registers.h; break;
			case 0x4d: registers.c = registers.l; break;
			case 0x4f: registers.c = registers.a; break;
			case 0x50: registers.d = registers.b; break;
			case 0x51: registers.d = registers.c; break;
			case 0x52: registers.d = registers.d; break;
			case 0x53: registers.d = registers.e; break;
			case 0x54: registers.d = registers.h; break;
			case 0x55: registers.d = registers.l; break;
			case 0x57: registers.d = registers.a; break;
			case 0x58: registers.e = registers.b; break;
			case 0x59: registers.e = registers.c; break;
			case 0x5a: registers.e = registers.d; break;
			case 0x5b: registers.e = registers.e; break;
			case 0x5c: registers.e = registers.h; break;
			case 0x5d: registers.e = registers.l; break;
			case 0x5f: registers.e = registers.a; break;
			case 0x60: registers.h = registers.b; break;
			case 0x61: registers.h = registers.c; break;
			case 0x62: registers.h = registers.d; break;
			case 0x63: registers.h = registers.e; break;
			case 0x64: registers.h = registers.h; break;
			case 0x65: registers.h = registers.l; break;
			case 0x67: registers.h = registers.a; break;
			case 0x68: registers.l = registers.b; break;
			case 0x69: registers.l = registers.c; break;
			case 0x6a: registers.l = registers.d; break;
			case 0x6b: registers.l = registers.e; break;
			case 0x6c: registers.l = registers.h; break;
			case 0x6d: registers.l = registers.l; break;
			case 0x6f: registers.l = registers.a; break;
			case 0x78: registers.a = registers.b; break;
			case 0x79: registers.a = registers.c; break;
			case 0x7a: registers.a = registers.d; break;
			case 0x7b: registers.a = registers.e; break;
			case 0x7c: registers.a = registers.h; break;
			case 0x7d: registers.a = registers.l; break;
			case 0x7f: registers.a = registers.a; break;

				//LD R N
			case 0x06: ld_r(registers.b, response.value); break;
			case 0x0e: ld_r(registers.c, response.value); break;
			case 0x16: ld_r(registers.d, response.value); break;
			case 0x1e: ld_r(registers.e, response.value); break;
			case 0x26: ld_r(registers.h, response.value); break;
			case 0x2e: ld_r(registers.l, response.value); break;
			case 0x3e: ld_r(registers.a, response.value); break;

				//LD R HL
			case 0x46: ld_r_hl(registers.b, response.value); break;
			case 0x4e: ld_r_hl(registers.c, response.value); break;
			case 0x56: ld_r_hl(registers.d, response.value); break;
			case 0x5e: ld_r_hl(registers.e, response.value); break;
			case 0x66: ld_r_hl(registers.h, response.value); break;
			case 0x6e: ld_r_hl(registers.l, response.value); break;
			case 0x7e: ld_r_hl(registers.a, response.value); break;

				//LD HL R
			case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
				break;

				//LD HL N
			case 0x36:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: break;
				}
				break;

				//LD A BC/DE
			case 0x0a:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: registers.a = ctx.first_fetch;
				}
				break;
			case 0x1a:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: registers.a = ctx.first_fetch;
				}
				break;

				//LD BC/DE A
			case 0x02:
			case 0x12:
				break;

				//LD A NN
			case 0xfa:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.second_fetch = response.value; registers.pc++; break;
				case 2: ctx.first_fetch = response.value; break;
				case 3: registers.a = ctx.first_fetch;
				}
				break;

				//LD NN A
			case 0xea:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.second_fetch = response.value; registers.pc++; break;
				}
				break;

				//LDH A (C)
			case 0xf2:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: registers.a = ctx.first_fetch; break;
				}
				break;

				//LDH (C) A
			case 0xe2: break;

				//LDH A N
			case 0xf0:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.first_fetch = response.value; break;
				case 2: registers.a = ctx.first_fetch; break;
				}
				break;

				//LDH N A
			case 0xe0:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				}
				break;

				//LD A HL-
			case 0x3a:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; set_joined_register(rHL, u16(get_joined_register(rHL) - 1)); break;
				case 1: registers.a = ctx.first_fetch;
				}
				break;

				//LD HL- A
			case 0x32:
				switch (ctx.m_cycles) {
				case 0: set_joined_register(rHL, u16(get_joined_register(rHL) - 1)); break;
				}
				break;

				//LD A HL+
			case 0x2a:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; set_joined_register(rHL, u16(get_joined_register(rHL) + 1)); break;
				case 1: registers.a = ctx.first_fetch;
				}
				break;

				//LD HL+ A
			case 0x22:
				switch (ctx.m_cycles) {
				case 0: set_joined_register(rHL, u16(get_joined_register(rHL) + 1)); break;
				}
				break;

				//LD RR NN 
			case 0x01: ld_rr_nn(rBC, response.value); break;
			case 0x11: ld_rr_nn(rDE, response.value); break;
			case 0x21: ld_rr_nn(rHL, response.value); break;
			case 0x31: ld_rr_nn(rSP, response.value); break;

				//LD NN SP
			case 0x08:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.second_fetch = response.value; registers.pc++; break;
				case 2:
					res16 = (ctx.second_fetch << 8) | ctx.first_fetch;
					res16++;
					ctx.first_fetch = res16 & 0xff;
					ctx.second_fetch = res16 >> 8;
					break;
				}
				break;

				//LD SP HL
			case 0xf9: registers.sp = get_joined_register(rHL); break;

				//PUSH RR
			case 0xc5:
			case 0xd5:
			case 0xe5:
			case 0xf5:
				switch (ctx.m_cycles) {
				case 0: registers.sp--; break;
				case 1: registers.sp--; break;
				}
				break;

				//POP RR :: todo push this into a function
			case 0xc1: pop_rr(rBC, response.value); break;
			case 0xd1: pop_rr(rDE, response.value); break;
			case 0xe1: pop_rr(rHL, response.value); break;
			case 0xf1: pop_rr(rAF, response.value); break;

				//LD HL SP+E
			case 0xf8:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1:
					e = ctx.first_fetch;
					res16 = (registers.sp & 0xff) + e;
					registers.l = res16 & 0xff;
					registers.f = 0x00;
					if ((registers.sp & 0x0f) + (e & 0x0f) > 0x0f) set_flag(fH, true);
					if (res16 > 0xff) set_flag(fC, true);
					break;
				case 2:
					res8 = (ctx.first_fetch & 0x80) ? 0xff : 0x00;
					registers.h = (registers.sp >> 8) + res8 + ((get_flag(fC)) ? 0x01 : 0x00);
					break;
				}
				break;

				//ADD R
			case 0x80: add_r(registers.b); break;
			case 0x81: add_r(registers.c); break;
			case 0x82: add_r(registers.d); break;
			case 0x83: add_r(registers.e); break;
			case 0x84: add_r(registers.h); break;
			case 0x85: add_r(registers.l); break;
			case 0x87: add_r(registers.a); break;

				//ADD HL
			case 0x86:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: add_r(ctx.first_fetch); break;
				}
				break;

				//ADD N
			case 0xc6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: add_r(ctx.first_fetch); break;
				}
				break;

				//ADC R
			case 0x88: adc_r(registers.b); break;
			case 0x89: adc_r(registers.c); break;
			case 0x8a: adc_r(registers.d); break;
			case 0x8b: adc_r(registers.e); break;
			case 0x8c: adc_r(registers.h); break;
			case 0x8d: adc_r(registers.l); break;
			case 0x8f: adc_r(registers.a); break;

				//ADC HL
			case 0x8e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: adc_r(ctx.first_fetch); break;
				}
				break;

				//ADC N
			case 0xce:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: adc_r(ctx.first_fetch); break;
				}
				break;

				//SUB R
			case 0x90: sub_r(registers.b); break;
			case 0x91: sub_r(registers.c); break;
			case 0x92: sub_r(registers.d); break;
			case 0x93: sub_r(registers.e); break;
			case 0x94: sub_r(registers.h); break;
			case 0x95: sub_r(registers.l); break;
			case 0x97: sub_r(registers.a); break;

				//SUB HL
			case 0x96:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: sub_r(ctx.first_fetch); break;
				}
				break;

				//SUB N
			case 0xd6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: sub_r(ctx.first_fetch); break;
				}
				break;

				//SBC R
			case 0x98: sbc_r(registers.b); break;
			case 0x99: sbc_r(registers.c); break;
			case 0x9a: sbc_r(registers.d); break;
			case 0x9b: sbc_r(registers.e); break;
			case 0x9c: sbc_r(registers.h); break;
			case 0x9d: sbc_r(registers.l); break;
			case 0x9f: sbc_r(registers.a); break;

				//SBC HL
			case 0x9e:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: sbc_r(ctx.first_fetch); break;
				}
				break;

				//SBC N
			case 0xde:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: sbc_r(ctx.first_fetch);
				}
				break;

				//CP R
			case 0xb8: cp_r(registers.b); break;
			case 0xb9: cp_r(registers.c); break;
			case 0xba: cp_r(registers.d); break;
			case 0xbb: cp_r(registers.e); break;
			case 0xbc: cp_r(registers.h); break;
			case 0xbd: cp_r(registers.l); break;
			case 0xbf: cp_r(registers.a); break;

				//CP HL
			case 0xbe:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: cp_r(ctx.first_fetch); break;
				}
				break;

				//CP N
			case 0xfe:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: cp_r(ctx.first_fetch); break;
				}
				break;

				//INC R
			case 0x04: inc_r(registers.b); break;
			case 0x0c: inc_r(registers.c); break;
			case 0x14: inc_r(registers.d); break;
			case 0x1c: inc_r(registers.e); break;
			case 0x24: inc_r(registers.h); break;
			case 0x2c: inc_r(registers.l); break;
			case 0x3c: inc_r(registers.a); break;

				//INC (HL)
			case 0x34:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1:
					res8 = ctx.first_fetch + 1;

					set_flag(fN, false);
					set_flag(fZ, false);
					set_flag(fH, false);

					if (res8 == 0) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x0f) + 0x01 > 0x0f) set_flag(fH, true);
					break;
				}
				break;

				//DEC R
			case 0x05: dec_r(registers.b); break;
			case 0x0d: dec_r(registers.c); break;
			case 0x15: dec_r(registers.d); break;
			case 0x1d: dec_r(registers.e); break;
			case 0x25: dec_r(registers.h); break;
			case 0x2d: dec_r(registers.l); break;
			case 0x3d: dec_r(registers.a); break;

				//DEC (HL)
			case 0x35:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1:
					res8 = ctx.first_fetch - 1;

					set_flag(fN, true);
					set_flag(fZ, false);
					set_flag(fH, false);

					if (res8 == 0) set_flag(fZ, true);
					if ((ctx.first_fetch & 0x0f) < 0x01) set_flag(fH, true);
				}
				break;

				//AND R
			case 0xa0: and_r(registers.b); break;
			case 0xa1: and_r(registers.c); break;
			case 0xa2: and_r(registers.d); break;
			case 0xa3: and_r(registers.e); break;
			case 0xa4: and_r(registers.h); break;
			case 0xa5: and_r(registers.l); break;
			case 0xa7: and_r(registers.a); break;

				//AND HL
			case 0xa6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: and_r(ctx.first_fetch); break;
				}
				break;

				//AND N
			case 0xe6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: and_r(ctx.first_fetch); break;
				}
				break;

				//OR R
			case 0xb0: or_r(registers.b); break;
			case 0xb1: or_r(registers.c); break;
			case 0xb2: or_r(registers.d); break;
			case 0xb3: or_r(registers.e); break;
			case 0xb4: or_r(registers.h); break;
			case 0xb5: or_r(registers.l); break;
			case 0xb7: or_r(registers.a); break;

				//OR HL
			case 0xb6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: or_r(ctx.first_fetch); break;
				}
				break;

				//OR N
			case 0xf6:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: or_r(ctx.first_fetch); break;
				}
				break;

				//XOR R
			case 0xa8: xor_r(registers.b); break;
			case 0xa9: xor_r(registers.c); break;
			case 0xaa: xor_r(registers.d); break;
			case 0xab: xor_r(registers.e); break;
			case 0xac: xor_r(registers.h); break;
			case 0xad: xor_r(registers.l); break;
			case 0xaf: xor_r(registers.a); break;
				break;

				//XOR HL
			case 0xae:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; break;
				case 1: xor_r(ctx.first_fetch); break;
				}
				break;

				//XOR N
			case 0xee:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: xor_r(ctx.first_fetch); break;
				}
				break;

				//CCF
			case 0x3f:
				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, !get_flag(fC));
				break;

				//SCF
			case 0x37:
				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, true);
				break;

				//DAA
			case 0x27: daa(); break;

				//CPL
			case 0x2f:
				registers.a = ~registers.a;
				set_flag(fN, true);
				set_flag(fH, true);
				break;

				//INC RR
			case 0x03: inc_rr(rBC); break;
			case 0x13: inc_rr(rDE); break;
			case 0x23: inc_rr(rHL); break;
			case 0x33: inc_rr(rSP); break;

				//DEC RR
			case 0x0b: dec_rr(rBC); break;
			case 0x1b: dec_rr(rDE); break;
			case 0x2b: dec_rr(rHL); break;
			case 0x3b: dec_rr(rSP); break;

				//ADD HL RR 
			case 0x09: add_hl_rr(registers.c, registers.b); break;
			case 0x19: add_hl_rr(registers.e, registers.d); break;
			case 0x29: add_hl_rr(registers.l, registers.h); break;
			case 0x39: add_hl_rr(registers.sp & 0xff, registers.sp >> 8); break;

				//ADD SP E
			case 0xe8:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1:
					res8 = (registers.sp & 0xff) + ctx.first_fetch;

					registers.f = 0x00;
					if (((registers.sp & 0xff) & 0x0f) + (ctx.first_fetch & 0x0f) > 0x0f) set_flag(fH, true);
					if ((registers.sp & 0xff) + ctx.first_fetch > 0xff) set_flag(fC, true);

					ctx.second_fetch = (ctx.first_fetch >> 7) & 0x01;
					ctx.first_fetch = res8;
					break;
				case 2:
					ctx.second_fetch = (ctx.second_fetch == 0x01) ? 0xff : 0x00;
					res8 = ctx.second_fetch + (get_flag(fC) ? 0x01 : 0x00) + (registers.sp >> 8);
					ctx.second_fetch = res8;
					break;
				case 3:
					registers.sp = u16((ctx.second_fetch << 8) | ctx.first_fetch);
					break;
				}
				break;

				//JP NN
			case 0xc3:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.second_fetch = response.value; registers.pc++; break;
				case 2: registers.pc = u16(ctx.second_fetch << 8 | ctx.first_fetch);
				}
				break;

				//JP HL
			case 0xe9: registers.pc = get_joined_register(rHL); break;

				//JP CC NN :: todo push this into a function
			case 0xc2: jp_cc(!get_flag(fZ), response.value); break;
			case 0xca: jp_cc(get_flag(fZ), response.value); break;
			case 0xd2: jp_cc(!get_flag(fC), response.value); break;
			case 0xda: jp_cc(get_flag(fC), response.value); break;

				//JR E
			case 0x18:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1:
					e = ctx.first_fetch;
					ctx.jp_condition = (e >> 7) & 0x01;
					ctx.first_fetch = e + (registers.pc & 0xff);

					adj = 0;
					if (!ctx.jp_condition && (e + (registers.pc & 0xff)) > 0xff) {
						adj = 1;
					}
					else if (ctx.jp_condition && (e + (registers.pc & 0xff)) <= 0xff) {
						adj = -1;
					}

					ctx.second_fetch = adj + (registers.pc >> 8);
					break;
				case 2: registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); break;
				}
				break;

				//JR CC E :: todo push this into a function
			case 0x20: jr_cc(!get_flag(fZ), response.value); break;
			case 0x28: jr_cc(get_flag(fZ), response.value); break;
			case 0x30: jr_cc(!get_flag(fC), response.value); break;
			case 0x38: jr_cc(get_flag(fC), response.value); break;

				//CALL NN
			case 0xcd:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.pc++; break;
				case 1: ctx.second_fetch = response.value; registers.pc++; break;
				case 2: registers.sp--; break;
				case 3: registers.sp--; break;
				case 4: registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); break;
				}
				break;

				//CALL CC NN
			case 0xc4: call_cc(!get_flag(fZ), response.value); break;
			case 0xcc: call_cc(get_flag(fZ), response.value); break;
			case 0xd4: call_cc(!get_flag(fC), response.value); break;
			case 0xdc: call_cc(get_flag(fC), response.value); break;

				//RET
			case 0xc9:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.sp++; break;
				case 1: ctx.second_fetch = response.value; registers.sp++; break;
				case 2: registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); break;
				}
				break;

				//RET CC
			case 0xc0: ret_cc(!get_flag(fZ), response.value); break;
			case 0xc8: ret_cc(get_flag(fZ), response.value); break;
			case 0xd0: ret_cc(!get_flag(fC), response.value); break;
			case 0xd8: ret_cc(get_flag(fC), response.value); break;

				//RETI
			case 0xd9:
				switch (ctx.m_cycles) {
				case 0: ctx.first_fetch = response.value; registers.sp++; break;
				case 1: ctx.second_fetch = response.value; registers.sp++; break;
				case 2: registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); ctx.ime = true; break;
				}
				break;

				//RST N
			case 0xc7: rst(0x0000); break; case 0xcf: rst(0x0008); break;
	        case 0xd7: rst(0x0010); break; case 0xdf: rst(0x0018); break; 
			case 0xe7: rst(0x0020); break; case 0xef: rst(0x0028); break;
			case 0xf7: rst(0x0030); break; case 0xff: rst(0x0038); break;

				//RLCA + RRCA + RLA + RRA
			case 0x07: rlca(); break; case 0x0f: rrca(); break;
            case 0x17: rla(); break; case 0x1f: rra(); break;
			}
		}

		if (is_instruction_done()) {
			if (ctx.sst_mode) {
				return;
			}

			fetch_response(response);
			return;
		}

		ctx.t_cycles = 0;
		ctx.m_cycles++;
		return;
	}

	ctx.t_cycles++;
}

/*   Fetch Request and Response   */
void CPU::fetch_request(BusRequest& request) {
	request.address = registers.pc;
	request.reading = true;
}
void CPU::fetch_response(BusResponse response) {
	if (ctx.t_cycles == 3) {
		ctx.opcode = response.value;
		registers.pc++;

		ctx.m_cycles = 0;
		ctx.t_cycles = 0;

		if (!ctx.prefix_check) {
			if (ctx.opcode == 0xcb) {
				ctx.state = sFetch;
				ctx.prefix_check = true;

				return;
			}

			ctx.instruction_length = instruction_lengths[ctx.opcode];
			ctx.prefixed_opcode = false;
			ctx.state = sExecute;
			return;
		}
		else {
			ctx.instruction_length = instruction_lengths_prefixed[ctx.opcode];
			ctx.prefixed_opcode = true;
			ctx.state = sExecute;
			return;
		}
	}

	ctx.t_cycles++;
}

/*   Check For Instruction Length   */
bool CPU::is_instruction_done() {
	if (ctx.m_cycles == ctx.instruction_length - (ctx.prefixed_opcode ? 2 : 1)) {
		ctx.prefix_check = false;
		return true;
	}

	return false;
}

/*   Bus Request Functions   */
void CPU::idle(BusRequest& request) {
	request.idle = true;
}

void CPU::read_pc(BusRequest& request) {
	request.address = registers.pc;
	request.reading = true;
}
void CPU::read_rr(CPUJoinedRegisters reg, BusRequest& request) {
	request.address = get_joined_register(reg);
	request.reading = true;
}
void CPU::read_nn(u16 address, BusRequest& request) {
	request.address = address;
	request.reading = true;
}

void CPU::write_rr(CPUJoinedRegisters reg, BusRequest& request, u8 value) {
	request.reading = false;
	request.address = get_joined_register(reg);
	request.value = value;

	if (ctx.t_cycles == 3) {
		request.commit = true;
	}
}
void CPU::write_nn(u16 address, BusRequest& request, u8 value) {
	request.address = address;
	request.value = value;
	request.reading = false;

	if (ctx.t_cycles == 3) {
		request.commit = true;
	}
}

/*   Opcode Logic and Implementations   */
void CPU::ld_r(u8& reg, u8 value) {
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = value; registers.pc++; break;
	case 1: reg = ctx.first_fetch; break;
	}
}
void CPU::ld_r_hl(u8& reg, u8 value) {
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = value; break;
	case 1: reg = ctx.first_fetch; break;
	}
}
void CPU::ld_rr_nn(CPUJoinedRegisters reg, u8 value) {
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = value; registers.pc++; break;
	case 1: ctx.second_fetch = value; registers.pc++; break;
	case 2: set_joined_register(reg, u16((ctx.second_fetch << 8) | ctx.first_fetch));
	}
}

void CPU::pop_rr(CPUJoinedRegisters reg, u8 value) {
	
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = value; registers.sp++; break;
	case 1: ctx.second_fetch = value; registers.sp++; break;
	case 2: 
		if (reg == rAF) {
			ctx.first_fetch &= 0xf0;
		}

		set_joined_register(reg, u16((ctx.second_fetch << 8) | ctx.first_fetch)); 
		break;
	}
}

void CPU::add_r(u8 value) {
	u8 result = registers.a + value;
	registers.f = 0x00;
	if (result == 0) set_flag(fZ, true);
	if ((registers.a & 0x0f) + (value & 0x0f) > 0x0f) set_flag(fH, true);
	if ((registers.a + value) > 0xff) set_flag(fC, true);
	registers.a = result;
}
void CPU::adc_r(u8 value) {
	u8 c = (get_flag(fC) ? 0x01 : 0x00);
	u8 result = registers.a + value + c;

	registers.f = 0x00;
	if (result == 0) set_flag(fZ, true);
	if ((registers.a & 0x0f) + (value & 0x0f) + c > 0x0f) set_flag(fH, true);
	if ((registers.a + value + c) > 0xff) set_flag(fC, true);

	registers.a = result;
}

void CPU::sub_r(u8 value) {
	u8 result = registers.a - value;
	registers.f = 0x00;

	set_flag(fN, true);
	if (result == 0) set_flag(fZ, true);
	if ((registers.a & 0x0f) < (value & 0x0f)) set_flag(fH, true);
	if (registers.a < value) set_flag(fC, true);

	registers.a = result;
}
void CPU::sbc_r(u8 value) {
	u8 c = get_flag(fC) ? 0x01 : 0x00;
	u8 result = registers.a - value - c;
	registers.f = 0x00;

	set_flag(fN, true);
	if (result == 0) set_flag(fZ, true);
	if ((registers.a & 0x0f) < ((value & 0x0f) + c)) set_flag(fH, true);
	if (registers.a < (value + c)) set_flag(fC, true);

	registers.a = result;
}
void CPU::cp_r(u8 value) {
	u8 c = get_flag(fC) ? 0x01 : 0x00;
	u8 result = registers.a - value;
	registers.f = 0x00;

	set_flag(fN, true);
	if (result == 0) set_flag(fZ, true);
	if ((registers.a & 0x0f) < (value & 0x0f)) set_flag(fH, true);
	if (registers.a < value) set_flag(fC, true);
}

void CPU::inc_r(u8& reg) {
	u8 result = reg + 1;

	set_flag(fN, false);
	set_flag(fZ, false);
	set_flag(fH, false);

	if (result == 0) set_flag(fZ, true);
	if (((reg & 0x0f) + 0x01) > 0x0f) set_flag(fH, true);

	reg = result;
}
void CPU::dec_r(u8& reg) {
	u8 result = reg - 1;

	set_flag(fN, true);
	set_flag(fZ, false);
	set_flag(fH, false);

	if (result == 0) set_flag(fZ, true);
	if ((reg & 0x0f) < 0x01) set_flag(fH, true);

	reg = result;
}

void CPU::and_r(u8 value) {
	u8 result = registers.a & value;

	registers.f = 0x00;
	set_flag(fH, true);
	if (result == 0) set_flag(fZ, true);

	registers.a = result;
}
void CPU::or_r(u8 value) {
	u8 result = registers.a | value;

	registers.f = 0x00;
	if (result == 0) set_flag(fZ, true);

	registers.a = result;
}
void CPU::xor_r(u8 value) {
	u8 result = registers.a ^ value;

	registers.f = 0x00;
	if (result == 0) set_flag(fZ, true);

	registers.a = result;
}

void CPU::inc_rr(CPUJoinedRegisters reg) {
	switch (ctx.m_cycles) {
	case 0: set_joined_register(reg, u16(get_joined_register(reg) + 1)); break;
	}
}
void CPU::dec_rr(CPUJoinedRegisters reg) {
	switch (ctx.m_cycles) {
	case 0: set_joined_register(reg, u16(get_joined_register(reg) - 1)); break;
	}
}

void CPU::add_hl_rr(u8 reg_low, u8 reg_high) {
	u8 result = 0x00;

	switch (ctx.m_cycles) {
	case 0:
		result = registers.l + reg_low;

		set_flag(fN, false);
		set_flag(fH, false);
		set_flag(fC, false);

		if ((registers.l & 0x0f) + (reg_low & 0x0f) > 0x0f) set_flag(fH, true);
		if (registers.l + reg_low > 0xff) set_flag(fC, true);

		registers.l = result;
		break;
	case 1:
		result = registers.h + reg_high + (get_flag(fC) ? 0x01 : 0x00);

		set_flag(fN, false);
		set_flag(fH, false);

		if ((registers.h & 0x0f) + (reg_high & 0x0f) + (get_flag(fC) ? 0x01 : 0x00) > 0x0f) set_flag(fH, true);
		if (registers.h + reg_high + (get_flag(fC) ? 0x01 : 0x00) > 0xff) {
			set_flag(fC, true);
		}
		else {
			set_flag(fC, false);
		}

		registers.h = result;
		break;
	}
}

void CPU::daa() {
	u8 a = registers.a;
	u8 adj = 0x00;

	bool set_c = false;
	bool N = get_flag(fN);
	bool H = get_flag(fH);
	bool C = get_flag(fC);

	if (!N) {
		if (H || (registers.a & 0x0f) > 0x09) {
			adj |= 0x06;
		}
		if (C || (registers.a > 0x99)) {
			adj += 0x60;
			set_c = true;
		}

		a += adj;
	}
	else {
		if (H) {
			adj |= 0x06;
		}
		if (C) {
			adj |= 0x60;
		}

		a -= adj;
	}

	registers.a = a;

	set_flag(fH, false);
	set_flag(fZ, false);
	set_flag(fC, false);

	if (registers.a == 0) set_flag(fZ, true);
	if (set_c || C) set_flag(fC, true);
}

void CPU::jp_cc(bool condition, u8 response_value) {
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = response_value; registers.pc++; break;
	case 1:
		ctx.second_fetch = response_value;
		registers.pc++;

		if (condition) {
			ctx.jp_condition = true;
		}
		else {
			ctx.instruction_length--;
			ctx.jp_condition = false;
		}

		break;
	case 2:
		if (ctx.jp_condition) {
			registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch);
		}
		break;
	}
}
void CPU::jr_cc(bool condition, u8 response_value) {
	u8 result = 0x00;
	u8 e = 0x00;
	u8 adj = 0x00;

	switch (ctx.m_cycles) {
	case 0:
		ctx.first_fetch = response_value;
		registers.pc++;

		if (condition) {
			ctx.jp_condition = true;
		}
		else {
			ctx.jp_condition = false;
			ctx.instruction_length--;
		}
		break;
	case 1:
		if (!ctx.jp_condition) break;

		e = ctx.first_fetch;
		result = (e >> 7) & 0x01;
		ctx.first_fetch = e + (registers.pc & 0xff);

		adj = 0;
		if (result == 0 && (e + (registers.pc & 0xff)) > 0xff) {
			adj = 1;
		}
		else if (result == 1 && (e + (registers.pc & 0xff)) <= 0xff) {
			adj = -1;
		}

		ctx.second_fetch = adj + (registers.pc >> 8);
		break;
	case 2:
		if (ctx.jp_condition) {
			registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch);
		}
		break;
	}
}
void CPU::call_cc(bool condition, u8 response_value) {
	switch (ctx.m_cycles) {
	case 0: ctx.first_fetch = response_value; registers.pc++; break;
	case 1:
		ctx.second_fetch = response_value;
		registers.pc++;

		if (condition) {
			ctx.jp_condition = true;
		}
		else {
			ctx.jp_condition = false;
			ctx.instruction_length -= 3;
		}
		break;
	case 2:
		if (!ctx.jp_condition) break;
		registers.sp--; break;
	case 3:
		if (!ctx.jp_condition) break;
		registers.sp--; break;
	case 4:
		if (!ctx.jp_condition) break;
		registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); break;
	}
}
void CPU::ret_cc(bool condition, u8 response_value) {
	switch (ctx.m_cycles) {
	case 0:
		if (condition) {
			ctx.jp_condition = true;
		}
		else {
			ctx.jp_condition = false;
			ctx.instruction_length -= 3;
		}
		break;
	case 1:
		if (!ctx.jp_condition) break;
		ctx.first_fetch = response_value; registers.sp++; break;
	case 2:
		if (!ctx.jp_condition) break;
		ctx.second_fetch = response_value; registers.sp++; break;
	case 3:
		if (!ctx.jp_condition) break;
		registers.pc = u16((ctx.second_fetch << 8) | ctx.first_fetch); break;
	}
}
void CPU::rst(u16 vector) {
	switch (ctx.m_cycles) {
	case 0: registers.sp--; break;
	case 1: registers.sp--; break;
	case 2: registers.pc = vector; break;
	}
}

void CPU::rlca() {
	u8 bit7 = (registers.a >> 7) & 0x01;
	u8 result = (registers.a << 1) | bit7;

	registers.f = 0x00;
	if (bit7 != 0x00) set_flag(fC, true);

	registers.a = result;
}
void CPU::rrca() {
	u8 bit0 = registers.a & 0x01;
	u8 result = (bit0 << 7) | (registers.a >> 1);

	registers.f = 0x00;
	if (bit0 != 0x00) set_flag(fC, true);

	registers.a = result;
}
void CPU::rla() {
	u8 bit7 = (registers.a >> 7) & 0x01;
	u8 carry = get_flag(fC) ? 0x01 : 0x00;
	u8 result = (registers.a << 1) | carry;

	registers.f = 0x00;
	if (bit7 != 0x00) set_flag(fC, true);

	registers.a = result;
}
void CPU::rra() {
	u8 bit0 = registers.a & 0x01;
	u8 carry = get_flag(fC) ? 0x01 : 0x00;
	u8 result = (carry << 7) | (registers.a >> 1);

	registers.f = 0x00;
	if (bit0 != 0x00) set_flag(fC, true);

	registers.a = result;
}

void CPU::rlc_r(u8& reg) {
	u8 bit7 = (reg >> 7) & 0x01;
	u8 result = (reg << 1) | bit7;

	registers.f = 0x00;
	if (bit7 != 0x00) set_flag(fC, true);
	if (result == 0) set_flag(fZ, true);

	reg = result;
}
void CPU::rrc_r(u8& reg) {
	u8 bit0 = reg & 0x01;
	u8 result = (bit0 << 7) | (reg >> 1);

	registers.f = 0x00;
	if (bit0 != 0x00) set_flag(fC, true);
	if (result == 0) set_flag(fZ, true);

	reg = result;

}
void CPU::rl_r(u8& reg) {
	u8 bit7 = (reg >> 7) & 0x01;
	u8 carry = get_flag(fC) ? 0x01 : 0x00;
	u8 result = (reg << 1) | carry;
	
	registers.f = 0x00;
	if (result == 0x00) set_flag(fZ, true);
	if (bit7 != 0x00) set_flag(fC, true);

	reg = result;
}
void CPU::rr_r(u8& reg) {
	u8 bit0 = reg & 0x01;
	u8 carry = get_flag(fC) ? 0x01 : 0x00;
	u8 result = (carry << 7) | (reg >> 1);

	registers.f = 0x00;
	if (result == 0x00) set_flag(fZ, true);
	if (bit0 != 0x00) set_flag(fC, true);

	reg = result;
}
void CPU::sla_r(u8& reg) {
	u8 bit7 = (reg >> 7) & 0x01;
	u8 result = (reg << 1) | 0x00;

	registers.f = 0x00;
	if (result == 0x00) set_flag(fZ, true);
	if (bit7 != 0x00) set_flag(fC, true);

	reg = result;
}
void CPU::sra_r(u8& reg) {
	u8 bit7 = (reg >> 7) & 0x01;
	u8 bit0 = reg & 0x01;
	u8 result = (bit7 << 7) | (reg >> 1);

	registers.f = 0x00;
	if (result == 0) set_flag(fZ, true);
	if (bit0 != 0x00) set_flag(fC, true);

	reg = result;
}
void CPU::swap_r(u8& reg) {
	u8 result = (reg << 4) | (reg >> 4);

	registers.f = 0x00;
	if (result == 0x00) set_flag(fZ, true);

	reg = result;
}
void CPU::srl_r(u8& reg) {
	u8 bit0 = reg & 0x01;
	u8 result = 0x00 | (reg >> 1);

	registers.f = 0x00;
	if (result == 0x00) set_flag(fZ, true);
	if (bit0 != 0x00) set_flag(fC, true);

	reg = result;
}
bool CPU::bit(u8 b, u8 value) {
	set_flag(fN, false);
	set_flag(fH, true);
	set_flag(fZ, true);

	if (((value >> b) & 0x01) != 0x00) {
		set_flag(fZ, false);
		return true;
	}

	return false;
}
void CPU::res(u8 b, u8& reg) {
	u8 mask = ~(0x01 << b);
	u8 result = mask & reg;

	reg = result;
}
void CPU::set(u8 b, u8& reg) {
	u8 mask = (0x01 << b);
	u8 result = mask | reg;

	reg = result;
}