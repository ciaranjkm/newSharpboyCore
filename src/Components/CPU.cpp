#include <Components/CPU.h>

//SST
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

//REGISTERS
void CPU::reset_registers(bool boot_rom) {
	registers.reset(boot_rom);
}

void CPU::load_registers(const CPURegisters& registers) {
	this->registers = registers;
}

CPURegisters CPU::get_registers() {
	return registers;
}

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

//FLAGS
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

//EXECUTION

/*   Send a bus request, is the cpu reading, writing or idle?   */
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

/*   Action the bus response, are we fetching opcode, executing, halted?   */
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

/*   Opcode definitions for bus requests   */
void CPU::opcode_bus_request(BusRequest& request) {
	if (is_instruction_done()) {
		fetch_request(request);
		return;
	}

	switch (ctx.opcode) {
		//MISC OPCODES
	case 0x00: 
		request.idle = true;
		break;

		//LD R R 
	case 0x40: //b b 
	case 0x41: //b c
	case 0x42: //b d
	case 0x43: //b e
	case 0x44: //b h
	case 0x45: //b l
	case 0x47: //b a
	case 0x48: //c b
	case 0x49: //c c 
	case 0x4a: //c d
	case 0x4b: //c e
	case 0x4c: //c h
	case 0x4d: //c l
	case 0x4f: //c a
	case 0x50: //d b
	case 0x51: //d c
	case 0x52: //d d
	case 0x53: //d e
	case 0x54: //d h
	case 0x55: //d l
	case 0x57: //d a
	case 0x58: //e b
	case 0x59: //e c
	case 0x5a: //e d
	case 0x5b: //e e
	case 0x5c: //e h
	case 0x5d: //e l
	case 0x5f: //e a
	case 0x60: //h b
	case 0x61: //h c 
	case 0x62: //h d
	case 0x63: //h e
	case 0x64: //h h
	case 0x65: //h l
	case 0x67: //h a
	case 0x68: //l b
	case 0x69: //l c
	case 0x6a: //l d
	case 0x6b: //l e
	case 0x6c: //l h
	case 0x6d: //l l
	case 0x6f: //l a
	case 0x78: //a b
	case 0x79: //a c
	case 0x7a: //a d
	case 0x7b: //a e
	case 0x7c: //a h
	case 0x7d: //a l
	case 0x7f: //a a 
		break;

		//LD R N
	case 0x06:
	case 0x16:
	case 0x26:
	case 0x0e:
	case 0x1e:
	case 0x2e:
	case 0x3e:
		switch (ctx.m_cycles) {
		case 0: read_pc(request); break;
		}
		break;

		//LD R HL
	case 0x46:
	case 0x4e:
	case 0x56:
	case 0x5e:
	case 0x66:
	case 0x6e:
	case 0x7e:
		switch (ctx.m_cycles) {
		case 0: read_rr(rHL, request); break;
		}
		break;

		//LD HL R
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
	case 0x01:
	case 0x11:
	case 0x21:
	case 0x31:
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

		//PUSH RR
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
	case 0xc1:
	case 0xd1:
	case 0xe1:
	case 0xf1:
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
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x87:
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
	case 0x88: 
	case 0x89: 
	case 0x8a: 
	case 0x8b: 
	case 0x8c: 
	case 0x8d: 
	case 0x8f: 
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
	case 0x90: 
	case 0x91: 
	case 0x92: 
	case 0x93: 
	case 0x94: 
	case 0x95: 
	case 0x97: 
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
	case 0x98: 
	case 0x99: 
	case 0x9a: 
	case 0x9b: 
	case 0x9c: 
	case 0x9d: 
	case 0x9f: 
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
	case 0xb8: 
	case 0xb9: 
	case 0xba: 
	case 0xbb: 
	case 0xbc: 
	case 0xbd: 
	case 0xbf: 
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
	case 0x04: 
	case 0x0c: 
	case 0x14: 
	case 0x1c: 
	case 0x24: 
	case 0x2c: 
	case 0x3c: 
		break;

		//INC (HL)
	case 0x34:
		switch (ctx.m_cycles) {
		case 0: read_rr(rHL, request); break;
		case 1: write_rr(rHL, request, u8(ctx.first_fetch + 1)); break;
		}
		break;

		//DEC R
	case 0x05: 
	case 0x0d: 
	case 0x15: 
	case 0x1d: 
	case 0x25: 
	case 0x2d: 
	case 0x3d: 
		break;

		//DEC (HL)
	case 0x35:
		switch (ctx.m_cycles) {
		case 0: read_rr(rHL, request); break;
		case 1: write_rr(rHL, request, u8(ctx.first_fetch - 1)); break;
		}
		break;

		//AND R
	case 0xa0: 
	case 0xa1: 
	case 0xa2: 
	case 0xa3: 
	case 0xa4: 
	case 0xa5: 
	case 0xa7: 
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
	case 0xb0: 
	case 0xb1: 
	case 0xb2: 
	case 0xb3: 
	case 0xb4: 
	case 0xb5:
	case 0xb7: 
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
	case 0xa8:
	case 0xa9:
	case 0xaa:
	case 0xab:
	case 0xac:
	case 0xad:
	case 0xaf:
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
	case 0x09:
	case 0x19:
	case 0x29:
	case 0x39:
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

	}
}

/*   Opcode definitions for bus responses   */
void CPU::opcode_bus_response(BusResponse response) {
	if (ctx.t_cycles == 3) {
		//variables for calculations inside case statements
		u8 e = 0x00;
		u8 res8 = 0x00;
		u16 res16 = 0x0000;

		//if (ctx.prefixed_opcode) {
		//	switch (ctx.opcode) {

		//	}
		//}

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
		case 0x06:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.b = ctx.first_fetch; break;
			}
			break;
		case 0x16:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.d = ctx.first_fetch; break;
			}
			break;
		case 0x26:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.h = ctx.first_fetch; break;
			}
			break;
		case 0x0e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.c = ctx.first_fetch; break;
			}
			break;
		case 0x1e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.e = ctx.first_fetch; break;
			}
			break;
		case 0x2e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.l = ctx.first_fetch; break;
			}
			break;
		case 0x3e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: registers.a = ctx.first_fetch; break;
			}
			break;

			//LD R HL
		case 0x46:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.b = ctx.first_fetch; break;
			}
			break;
		case 0x4e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.c = ctx.first_fetch; break;
			}
			break;
		case 0x56:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.d = ctx.first_fetch; break;
			}
			break;
		case 0x5e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.e = ctx.first_fetch; break;
			}
			break;
		case 0x66:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.h = ctx.first_fetch; break;
			}
			break;
		case 0x6e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.l = ctx.first_fetch; break;
			}
			break;
		case 0x7e:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; break;
			case 1: registers.a = ctx.first_fetch; break;
			}
			break;

			//LD HL R
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x77:
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
		case 0x01:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: ctx.second_fetch = response.value; registers.pc++; break;
			case 2: set_joined_register(rBC, u16((ctx.second_fetch << 8) | ctx.first_fetch));
			}
			break;
		case 0x11:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: ctx.second_fetch = response.value; registers.pc++; break;
			case 2: set_joined_register(rDE, u16((ctx.second_fetch << 8) | ctx.first_fetch));
			}
			break;
		case 0x21:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: ctx.second_fetch = response.value; registers.pc++; break;
			case 2: set_joined_register(rHL, u16((ctx.second_fetch << 8) | ctx.first_fetch));
			}
			break;
		case 0x31:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.pc++; break;
			case 1: ctx.second_fetch = response.value; registers.pc++; break;
			case 2: set_joined_register(rSP, u16((ctx.second_fetch << 8) | ctx.first_fetch));
			}
			break;

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

			//POP RR
		case 0xc1:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.sp++; break;
			case 1: ctx.second_fetch = response.value; registers.sp++; break;
			case 2: set_joined_register(rBC, u16((ctx.second_fetch << 8) | ctx.first_fetch)); break;
			}
			break;
		case 0xd1:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.sp++; break;
			case 1: ctx.second_fetch = response.value; registers.sp++; break;
			case 2: set_joined_register(rDE, u16((ctx.second_fetch << 8) | ctx.first_fetch)); break;
			}
			break;
		case 0xe1:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.sp++; break;
			case 1: ctx.second_fetch = response.value; registers.sp++; break;
			case 2: set_joined_register(rHL, u16((ctx.second_fetch << 8) | ctx.first_fetch)); break;
			}
			break;
		case 0xf1:
			switch (ctx.m_cycles) {
			case 0: ctx.first_fetch = response.value; registers.sp++; break;
			case 1: ctx.second_fetch = response.value; registers.sp++; break;
			case 2: set_joined_register(rAF, u16(((ctx.second_fetch << 8) | ctx.first_fetch) & 0xfff0)); break;
			}
			break;

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
		case 0x03: 
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rBC, u16(get_joined_register(rBC) + 1)); break;
			}
			break;
		case 0x13:
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rDE, u16(get_joined_register(rDE) + 1)); break;
			}
			break;
		case 0x23:
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rHL, u16(get_joined_register(rHL) + 1)); break;
			}
			break;
			
		case 0x33:
			switch (ctx.m_cycles) {
			case 0: registers.sp++; break;
			}
			break;

			//DEC RR
		case 0x0b:
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rBC, u16(get_joined_register(rBC) - 1)); break;
			}
			break;
		case 0x1b:
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rDE, u16(get_joined_register(rDE) - 1)); break;
			}
			break;
		case 0x2b:
			switch (ctx.m_cycles) {
			case 0: set_joined_register(rHL, u16(get_joined_register(rHL) - 1)); break;
			}
			break;

		case 0x3b:
			switch (ctx.m_cycles) {
			case 0: registers.sp--; break;
			}
			break;

			//ADD HL RR
		case 0x09:
			switch (ctx.m_cycles) {
			case 0:
				res8 = registers.l + registers.c;
				
				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, false);

				if ((registers.l & 0x0f) + (registers.c & 0x0f) > 0x0f) set_flag(fH, true);
				if (registers.l + registers.c > 0xff) set_flag(fC, true);

				registers.l = res8;
				break;
			case 1:
				res8 = registers.h + registers.b + (get_flag(fC) ? 0x01 : 0x00);

				set_flag(fN, false);
				set_flag(fH, false);

				if ((registers.h & 0x0f) + (registers.b & 0x0f) + (get_flag(fC) ? 0x01 : 0x00) > 0x0f) set_flag(fH, true);
				if (registers.h + registers.b + (get_flag(fC) ? 0x01 : 0x00) > 0xff) {
					set_flag(fC, true);
				}
				else {
					set_flag(fC, false);
				}

				registers.h = res8;
				break;
			}
			break;
		case 0x19:
			switch (ctx.m_cycles) {
			case 0:
				res8 = registers.l + registers.e;

				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, false);

				if ((registers.l & 0x0f) + (registers.e & 0x0f) > 0x0f) set_flag(fH, true);
				if (registers.l + registers.e > 0xff) set_flag(fC, true);

				registers.l = res8;
				break;
			case 1:
				res8 = registers.h + registers.d + (get_flag(fC) ? 0x01 : 0x00);

				set_flag(fN, false);
				set_flag(fH, false);

				if ((registers.h & 0x0f) + (registers.d & 0x0f) + (get_flag(fC) ? 0x01 : 0x00) > 0x0f) set_flag(fH, true);
				if (registers.h + registers.d + (get_flag(fC) ? 0x01 : 0x00) > 0xff) {
					set_flag(fC, true);
				}
				else {
					set_flag(fC, false);
				}

				registers.h = res8;
				break;
			}
			break;
		case 0x29:
			switch (ctx.m_cycles) {
			case 0:
				res8 = registers.l + registers.l;

				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, false);

				if ((registers.l & 0x0f) + (registers.l & 0x0f) > 0x0f) set_flag(fH, true);
				if (registers.l + registers.l > 0xff) set_flag(fC, true);

				registers.l = res8;
				break;
			case 1:
				res8 = registers.h + registers.h + (get_flag(fC) ? 0x01 : 0x00);

				set_flag(fN, false);
				set_flag(fH, false);

				if ((registers.h & 0x0f) + (registers.h & 0x0f) + (get_flag(fC) ? 0x01 : 0x00) > 0x0f) set_flag(fH, true);
				if (registers.h + registers.h + (get_flag(fC) ? 0x01 : 0x00) > 0xff) {
					set_flag(fC, true);
				}
				else {
					set_flag(fC, false);
				}

				registers.h = res8;
				break;
			}
			break;
		case 0x39:
			switch (ctx.m_cycles) {
			case 0:
				res8 = registers.l + (registers.sp & 0xff);

				set_flag(fN, false);
				set_flag(fH, false);
				set_flag(fC, false);

				if ((registers.l & 0x0f) + ((registers.sp & 0xff) & 0x0f) > 0x0f) set_flag(fH, true);
				if (registers.l + (registers.sp & 0xff) > 0xff) set_flag(fC, true);

				registers.l = res8;
				break;
			case 1:
				res8 = registers.h + (registers.sp >> 8) + (get_flag(fC) ? 0x01 : 0x00);

				set_flag(fN, false);
				set_flag(fH, false);

				if ((registers.h & 0x0f) + ((registers.sp >> 8) & 0x0f) + (get_flag(fC) ? 0x01 : 0x00) > 0x0f) set_flag(fH, true);
				if (registers.h + (registers.sp >> 8) + (get_flag(fC) ? 0x01 : 0x00) > 0xff) {
					set_flag(fC, true);
				}
				else {
					set_flag(fC, false);
				}

				registers.h = res8;
				break;
			}
			break;

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

//FETCH
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

		ctx.instruction_length = instruction_lengths[ctx.opcode];

		ctx.state = sExecute;
		return;
	}

	ctx.t_cycles++;
}

bool CPU::is_instruction_done() {
	if (ctx.m_cycles == ctx.instruction_length - 1) {
		return true;
	}

	return false;
}

//OPCODE FUNCTIONS
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