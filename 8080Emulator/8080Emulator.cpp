// 8080Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

using namespace std;

/*Flags of the 8080 processor*/
typedef struct ConditionCodes
{
	uint8_t		z : 1;
	uint8_t		s : 1;
	uint8_t		p : 1;
	uint8_t		cy : 1;
	uint8_t     ac : 1;
	uint8_t		pad : 1;
} ConditionCodes;

/*Registers of the 8080 processor
* Pairs: BC, DE, HL
*/
typedef struct State8080
{
	uint8_t		a;
	uint8_t		b;
	uint8_t		c;
	uint8_t		d;
	uint8_t		e;
	uint8_t		h;
	uint8_t		l;
	uint16_t	sp;							//Stack pointer
	uint16_t	pc;							//Program counter
	uint8_t		* memory;
	struct		ConditionCodes		cc;
	uint8_t		int_enable;
} State8080;

bool getParity(uint8_t n)
{
	bool parity = 0;
	while (n)
	{
		parity = !parity;
		n = n & (n - 1);
	}
	return parity;
}

void UnimplementedInstruction(State8080* state)
{
	printf("Error: Unimplemented instruction\n");
	exit(1);
}

void Emulate8080p(State8080* state)
{
	unsigned char* opcode = &state->memory[state->pc];

	switch (*opcode)
	{
		case 0x00: break;							//NOP
		case 0x01:									//LXI   B,word  
		{
			state->c = opcode[1];
			state->b = opcode[2];
			state->pc += 2;  //Advance 2 more bytes  
		}
		break;
		case 0x02:									//STAX B
		{

		}
		break;
		case 0x03:									//INX B
		{
			uint16_t bcAddress = (state->b << 8) | (state->c);
			bcAddress++;
			state->c = (bcAddress & 0xff);
			state->b = (bcAddress >> 8);
		}
		break;
		case 0x04:									//INR B
		{
			uint16_t result = (uint16_t)state->b + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->b = result & 0xff;
		}
		break;
		case 0x05:									//DCR B
		{
			uint8_t result = state->b - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->b = result;
		}
		break;
		case 0x06:
		{

		}
		break;
		case 0x07: UnimplementedInstruction(state); break;
		case 0x08: UnimplementedInstruction(state); break;
		case 0x09:									//DAD B
		{
			uint32_t hlAddress = (state->h << 8) | (state->l);
			uint32_t bcAddress = (state->b << 8) | (state->c);
			uint32_t result = hlAddress + bcAddress;
			state->h = (result & 0xff00) >> 8;
			state->l = (result & 0xff);
			state->cc.cy = ((result & 0xffff0000) > 0);
		}
		break;
		case 0x0a: UnimplementedInstruction(state); break;
		case 0x0b:									//DCX B
		{
			uint16_t bcAddress = (state->b << 8) | (state->c);
			bcAddress--;
			state->c = (bcAddress & 0xff);
			state->b = (bcAddress >> 8);
		}
		break;
		case 0x0c:									//INR C
		{
			uint16_t result = (uint16_t)state->c + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->c = result & 0xff;
		}
		break;
		case 0x0d:									//DCR C
		{
			uint8_t result = state->c - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->c = result;
		}
		break;
		case 0x0e:									//MVI C,byte
		{

		}
		break;
		case 0x0f:									//RRC
		{

		}
		break;
		case 0x10: UnimplementedInstruction(state); break;
		case 0x11:									//LXI D,word
		{
			state->e = opcode[1];
			state->d = opcode[2];
			state->pc += 2;  //Advance 2 more bytes    
			break;
		}
		break;
		case 0x12: UnimplementedInstruction(state); break;
		case 0x13:									//INX D
		{
			uint16_t deAddress = (state->d << 8) | (state->e);
			deAddress++;
			state->e = (deAddress & 0xff);
			state->d = (deAddress >> 8);
		}
		break;
		case 0x14:									//INR D
		{
			uint16_t result = (uint16_t)state->d + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->d = result & 0xff;
		}
		break;
		case 0x15:									//DCR D
		{
			uint8_t result = state->d - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->d = result;
		}
		break;
		case 0x16: UnimplementedInstruction(state); break;
		case 0x17: UnimplementedInstruction(state); break;
		case 0x18: break;
		case 0x19:									//DAD D
		{
			uint32_t hlAddress = (state->h << 8) | (state->l);
			uint32_t deAddress = (state->d << 8) | (state->e);
			uint32_t result = hlAddress + deAddress;
			state->h = (result & 0xff00) >> 8;
			state->l = (result & 0xff);
			state->cc.cy = ((result & 0xffff0000) > 0);
		}
		break;
		case 0x1a:									//LDAX D
		{

		}
		break;
		case 0x1b:									//DCX D
		{
			uint16_t deAddress = (state->d << 8) | (state->e);
			deAddress--;
			state->e = (deAddress & 0xff);
			state->d = (deAddress >> 8);
		}
		break;
		case 0x1c:									//INR E
		{
			uint16_t result = (uint16_t)state->e + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->e = result & 0xff;
		}
		break;
		case 0x1d:									//DCR E
		{
			uint8_t result = state->e - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->e = result;
		}
		break;
		case 0x1e: UnimplementedInstruction(state); break;
		case 0x1f: UnimplementedInstruction(state); break;
		case 0x20: UnimplementedInstruction(state); break;
		case 0x21:									//LXI H,word
		{

		}
		break;
		case 0x22: UnimplementedInstruction(state); break;
		case 0x23:									//INX H
		{
			uint16_t hlAddress = (state->h << 8) | (state->l);
			hlAddress++;
			state->l = (hlAddress & 0xff);
			state->h = (hlAddress >> 8);
		}
		break;
		case 0x24:									//INR H
		{
			uint16_t result = (uint16_t)state->h + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->h = result & 0xff;
		}
		break;
		case 0x25:									//DCR H
		{
			uint8_t result = state->h - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->h = result;
		}
		break;
		case 0x26:									//MVI H,byte
		{

		}
		break;
		case 0x27: UnimplementedInstruction(state); break;
		case 0x28: UnimplementedInstruction(state); break;
		case 0x29:									//DAD H
		{
			uint32_t hlAddress = (state->h << 8) | (state->l);
			uint32_t result = hlAddress + hlAddress;
			state->h = (result & 0xff00) >> 8;
			state->l = (result & 0xff);
			state->cc.cy = ((result & 0xffff0000) > 0);
		}
		break;
		case 0x2a: UnimplementedInstruction(state); break;
		case 0x2b:									//DCX H
		{
			uint16_t hlAddress = (state->h << 8) | (state->l);
			hlAddress--;
			state->l = (hlAddress & 0xff);
			state->h = (hlAddress >> 8);
		}
		break;
		case 0x2c:									//INR L
		{
			uint16_t result = (uint16_t)state->l + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->l = result & 0xff;
		}
		break;
		case 0x2d:									//DCR L
		{
			uint8_t result = state->l - 1;
			state->cc.z = ((result) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result);
			state->l = result;
		}
		break;
		case 0x2e: UnimplementedInstruction(state); break;
		case 0x2f: UnimplementedInstruction(state); break;
		case 0x30: UnimplementedInstruction(state); break;
		case 0x31:									//LXI SP,word
		{

		}
		break;
		case 0x32:									//STA word
		{

		}
		break;
		case 0x33:									//INX SP
		{
			state->sp = state->sp + 1;
		}
		break;
		case 0x34:									//INR M : M=value pointed by HL
		{
			uint16_t hlAddress = (state->h << 8) | (state->l);
			uint16_t m = state->memory[hlAddress];
			uint16_t result = m + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->memory[hlAddress] = m;
		}
		break;
		case 0x35:									//DCR M
		{
			uint16_t hlAddress = (state->h << 8) | (state->l);
			uint16_t m = state->memory[hlAddress];
			uint16_t result = m - 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->memory[hlAddress] = m;
		}
		break;

		case 0x36:									//MVI M,byte
		{

		}
		break;
		case 0x37: UnimplementedInstruction(state); break;
		case 0x38: UnimplementedInstruction(state); break;
		case 0x39:									//DAD SP
		{
			uint32_t hlAddress = (state->h << 8) | (state->l);
			uint32_t result = hlAddress + state->sp;
			state->h = (result & 0xff00) >> 8;
			state->l = (result & 0xff);
			state->cc.cy = ((result & 0xffff0000) > 0);
		}
		break;
		case 0x3a:									//LDA word
		{

		}
		break;
		case 0x3b:									//DCX SP
		{
			state->sp = state->sp - 1;
		}
		break;
		case 0x3c:									//INR A
		{
			uint16_t result = (uint16_t)state->a + 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->a = result & 0xff;
		}
		break;
		case 0x3d:									//DCR A
		{
			uint16_t result = (uint16_t)state->a - 1;
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.p = getParity(result & 0xff);
			state->a = result & 0xff;
		}
		break;
		case 0x3e:									//MVI A,byte
		{

		}
		break;
		case 0x3f: UnimplementedInstruction(state); break;
		case 0x40: UnimplementedInstruction(state); break;
		case 0x41: state->b = state->c; break;		//MOV B,C    
		case 0x42: state->b = state->d; break;		//MOV B,D    
		case 0x43: state->b = state->e; break;		//MOV B,E    
		case 0x44: UnimplementedInstruction(state); break;
		case 0x45: UnimplementedInstruction(state); break;
		case 0x46: UnimplementedInstruction(state); break;
		case 0x47: UnimplementedInstruction(state); break;
		case 0x48: UnimplementedInstruction(state); break;
		case 0x49: UnimplementedInstruction(state); break;
		case 0x4a: UnimplementedInstruction(state); break;
		case 0x4b: UnimplementedInstruction(state); break;
		case 0x4c: UnimplementedInstruction(state); break;
		case 0x4d: UnimplementedInstruction(state); break;
		case 0x4e: UnimplementedInstruction(state); break;
		case 0x4f: UnimplementedInstruction(state); break;
		case 0x50: UnimplementedInstruction(state); break;
		case 0x51: UnimplementedInstruction(state); break;
		case 0x52: UnimplementedInstruction(state); break;
		case 0x53: UnimplementedInstruction(state); break;
		case 0x54: UnimplementedInstruction(state); break;
		case 0x55: UnimplementedInstruction(state); break;
		case 0x56:									//MOV D,M
		{

		}
		break;
		case 0x57: UnimplementedInstruction(state); break;
		case 0x58: UnimplementedInstruction(state); break;
		case 0x59: UnimplementedInstruction(state); break;
		case 0x5a: UnimplementedInstruction(state); break;
		case 0x5b: UnimplementedInstruction(state); break;
		case 0x5c: UnimplementedInstruction(state); break;
		case 0x5d: UnimplementedInstruction(state); break;
		case 0x5e:									//MOV E,M
		{

		}
		break;
		case 0x5f: UnimplementedInstruction(state); break;
		case 0x60: UnimplementedInstruction(state); break;
		case 0x61: UnimplementedInstruction(state); break;
		case 0x62: UnimplementedInstruction(state); break;
		case 0x63: UnimplementedInstruction(state); break;
		case 0x64: UnimplementedInstruction(state); break;
		case 0x65: UnimplementedInstruction(state); break;
		case 0x66:									//MOV H,M
		{

		}
		break;
		case 0x67: UnimplementedInstruction(state); break;
		case 0x68: UnimplementedInstruction(state); break;
		case 0x69: UnimplementedInstruction(state); break;
		case 0x6a: UnimplementedInstruction(state); break;
		case 0x6b: UnimplementedInstruction(state); break;
		case 0x6c: UnimplementedInstruction(state); break;
		case 0x6d: UnimplementedInstruction(state); break;
		case 0x6e: UnimplementedInstruction(state); break;
		case 0x6f:									//MOV L,A
		{

		}
		break;
		case 0x70: UnimplementedInstruction(state); break;
		case 0x71: UnimplementedInstruction(state); break;
		case 0x72: UnimplementedInstruction(state); break;
		case 0x73: UnimplementedInstruction(state); break;
		case 0x74: UnimplementedInstruction(state); break;
		case 0x75: UnimplementedInstruction(state); break;
		case 0x76: UnimplementedInstruction(state); break;
		case 0x77:									//MOV M,A
		{

		}
		break;
		case 0x78: UnimplementedInstruction(state); break;
		case 0x79: UnimplementedInstruction(state); break;
		case 0x7a: state->a = state->d; break;		//MOV D,A
		case 0x7b: state->a = state->e; break;		//MOV E,A
		case 0x7c: state->a = state->h; break;		//MOV H,A
		case 0x7d: UnimplementedInstruction(state); break;
		case 0x7e:									//MOV A,M
		{

		}
		break;
		case 0x7f: UnimplementedInstruction(state); break;
		case 0x80:									//ADD B
		{
			//do the match with higher precision to capture carry
			uint16_t result = (uint16_t)state->a + (uint16_t)state->b;
			
			//Check the last 8 bits and set the zero flag if result is zero
			state->cc.z = ((result & 0xff) == 0);

			//Check bit 7 to determine the sign
			state->cc.s = ((result & 0x80) != 0);

			//Check if there was a carry needed (bigger than 8bit)
			state->cc.cy = (result > 0xff);

			state->cc.p = getParity(result & 0xff);

			//Set the last 8 bits as result
			state->a = result & 0xff;
		}
		break;
		case 0x81: UnimplementedInstruction(state); break;
		case 0x82: UnimplementedInstruction(state); break;
		case 0x83: UnimplementedInstruction(state); break;
		case 0x84: UnimplementedInstruction(state); break;
		case 0x85: UnimplementedInstruction(state); break;
		case 0x86:	//ADD M			Here M is the value stored in the address of pair HL
		{
			//Create the 16bit address from the H and L registers
			uint16_t offset = (state->h << 8) | (state->l);
			uint16_t result = (uint16_t)state->a + state->memory[offset];
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.cy = (result > 0xff);
			state->cc.p = getParity(result & 0xff);
			state->a = result & 0xff;
		}
		break;
		case 0x87: UnimplementedInstruction(state); break;
		case 0x88: UnimplementedInstruction(state); break;
		case 0x89: UnimplementedInstruction(state); break;
		case 0x8a: UnimplementedInstruction(state); break;
		case 0x8b: UnimplementedInstruction(state); break;
		case 0x8c: UnimplementedInstruction(state); break;
		case 0x8d: UnimplementedInstruction(state); break;
		case 0x8e: UnimplementedInstruction(state); break;
		case 0x8f: UnimplementedInstruction(state); break;
		case 0x90: UnimplementedInstruction(state); break;
		case 0x91: UnimplementedInstruction(state); break;
		case 0x92: UnimplementedInstruction(state); break;
		case 0x93: UnimplementedInstruction(state); break;
		case 0x94: UnimplementedInstruction(state); break;
		case 0x95: UnimplementedInstruction(state); break;
		case 0x96: UnimplementedInstruction(state); break;
		case 0x97: UnimplementedInstruction(state); break;
		case 0x98: UnimplementedInstruction(state); break;
		case 0x99: UnimplementedInstruction(state); break;
		case 0x9a: UnimplementedInstruction(state); break;
		case 0x9b: UnimplementedInstruction(state); break;
		case 0x9c: UnimplementedInstruction(state); break;
		case 0x9d: UnimplementedInstruction(state); break;
		case 0x9e: UnimplementedInstruction(state); break;
		case 0x9f: UnimplementedInstruction(state); break;
		case 0xa0: UnimplementedInstruction(state); break;
		case 0xa1: UnimplementedInstruction(state); break;
		case 0xa2: UnimplementedInstruction(state); break;
		case 0xa3: UnimplementedInstruction(state); break;
		case 0xa4: UnimplementedInstruction(state); break;
		case 0xa5: UnimplementedInstruction(state); break;
		case 0xa6: UnimplementedInstruction(state); break;
		case 0xa7:									//ANA A
		case 0xa8: UnimplementedInstruction(state); break;
		case 0xa9: UnimplementedInstruction(state); break;
		case 0xaa: UnimplementedInstruction(state); break;
		case 0xab: UnimplementedInstruction(state); break;
		case 0xac: UnimplementedInstruction(state); break;
		case 0xad: UnimplementedInstruction(state); break;
		case 0xae: UnimplementedInstruction(state); break;
		case 0xaf:									//XRA
		case 0xb0: UnimplementedInstruction(state); break;
		case 0xb1: UnimplementedInstruction(state); break;
		case 0xb2: UnimplementedInstruction(state); break;
		case 0xb3: UnimplementedInstruction(state); break;
		case 0xb4: UnimplementedInstruction(state); break;
		case 0xb5: UnimplementedInstruction(state); break;
		case 0xb6: UnimplementedInstruction(state); break;
		case 0xb7: UnimplementedInstruction(state); break;
		case 0xb8: UnimplementedInstruction(state); break;
		case 0xb9: UnimplementedInstruction(state); break;
		case 0xba: UnimplementedInstruction(state); break;
		case 0xbb: UnimplementedInstruction(state); break;
		case 0xbc: UnimplementedInstruction(state); break;
		case 0xbd: UnimplementedInstruction(state); break;
		case 0xbe: UnimplementedInstruction(state); break;
		case 0xbf: UnimplementedInstruction(state); break;
		case 0xc0: UnimplementedInstruction(state); break;
		case 0xc1:									//POP B
		{

		}
		break;
		case 0xc2:									//JNZ adr
		{

		}
		break;
		case 0xc3:									//JMP adr
		{

		}
		break;
		case 0xc4: UnimplementedInstruction(state); break;
		case 0xc5:									//PUSH B
		{

		}
		break;
		case 0xc6:	//ADI byte
		{
			uint16_t result = (uint16_t)state->a + (uint16_t) opcode[1];
			state->cc.z = ((result & 0xff) == 0);
			state->cc.s = ((result & 0x80) != 0);
			state->cc.cy = (result > 0xff);
			state->cc.p = getParity(result & 0xff);
			state->a = result & 0xff;
		}
		break;
		case 0xc7: UnimplementedInstruction(state); break;
		case 0xc8: UnimplementedInstruction(state); break;
		case 0xc9:									//RET
		{

		}
		break;
		case 0xca: UnimplementedInstruction(state); break;
		case 0xcb: UnimplementedInstruction(state); break;
		case 0xcc: UnimplementedInstruction(state); break;
		case 0xcd:									//CALL adr
		{

		}
		break;
		case 0xce: UnimplementedInstruction(state); break;
		case 0xcf: UnimplementedInstruction(state); break;
		case 0xd0: UnimplementedInstruction(state); break;
		case 0xd1:									//POP D
		{

		}
		break;
		case 0xd2: UnimplementedInstruction(state); break;
		case 0xd3: UnimplementedInstruction(state); break;
		case 0xd4: UnimplementedInstruction(state); break;
		case 0xd5:									//PUSH D
		{

		}
		break;
		case 0xd6: UnimplementedInstruction(state); break;
		case 0xd7: UnimplementedInstruction(state); break;
		case 0xd8: UnimplementedInstruction(state); break;
		case 0xd9: UnimplementedInstruction(state); break;
		case 0xda: UnimplementedInstruction(state); break;
		case 0xdb: UnimplementedInstruction(state); break;
		case 0xdc: UnimplementedInstruction(state); break;
		case 0xdd: UnimplementedInstruction(state); break;
		case 0xde: UnimplementedInstruction(state); break;
		case 0xdf: UnimplementedInstruction(state); break;
		case 0xe0: UnimplementedInstruction(state); break;
		case 0xe1:									//POP H
		{

		}
		break;
		case 0xe2: UnimplementedInstruction(state); break;
		case 0xe3: UnimplementedInstruction(state); break;
		case 0xe4: UnimplementedInstruction(state); break;
		case 0xe5:									//PUSH H
		{

		}
		break;
		case 0xe6:									//ANI byte
		{

		}
		break;
		case 0xe7: UnimplementedInstruction(state); break;
		case 0xe8: UnimplementedInstruction(state); break;
		case 0xe9: UnimplementedInstruction(state); break;
		case 0xea: UnimplementedInstruction(state); break;
		case 0xeb:									//XCHG
		{

		}
		break;
		case 0xec: UnimplementedInstruction(state); break;
		case 0xed: UnimplementedInstruction(state); break;
		case 0xee: UnimplementedInstruction(state); break;
		case 0xef: UnimplementedInstruction(state); break;
		case 0xf0: UnimplementedInstruction(state); break;
		case 0xf1:									//POP PSW
		{

		}
		break;
		case 0xf2: UnimplementedInstruction(state); break;
		case 0xf3: UnimplementedInstruction(state); break;
		case 0xf4: UnimplementedInstruction(state); break;
		case 0xf5:									//PUSH PSW
		{

		}
		break;
		case 0xf6: UnimplementedInstruction(state); break;
		case 0xf7: UnimplementedInstruction(state); break;
		case 0xf8: UnimplementedInstruction(state); break;
		case 0xf9: UnimplementedInstruction(state); break;
		case 0xfa: UnimplementedInstruction(state); break;
		case 0xfb: UnimplementedInstruction(state); break;
		case 0xfc: UnimplementedInstruction(state); break;
		case 0xfd: UnimplementedInstruction(state); break;
		case 0xfe:									//CPI byte
		{

		}
		break;
		case 0xff: UnimplementedInstruction(state); break;
	}
	state->pc += 1;
}

/*
	*codebuffer is a valid pointer to 8080 assembly code
	pc is the current offset into the code

	returns the number of bytes of the op
   */

int Disassemble8080Op(unsigned char* codebuffer, int pc)
{
	unsigned char* code = &codebuffer[pc];
	int opbytes = 1;
	printf("%04x ", pc);
	switch (*code)
	{
		case 0x00: printf("NOP"); break;
		case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x02: printf("STAX   B"); break;
		case 0x03: printf("INX    B"); break;
		case 0x04: printf("INR    B"); break;
		case 0x05: printf("DCR    B"); break;
		case 0x06: printf("MVI    B,#0x%02x", code[1]); opbytes = 2; break;
		case 0x07: printf("RLC"); break;
		case 0x08: printf("NOP"); break;
		case 0x09: printf("DAD    B"); break;
		case 0x0a: printf("LDAX   B"); break;
		case 0x0b: printf("DCX    B"); break;
		case 0x0c: printf("INR    C"); break;
		case 0x0d: printf("DCR    C"); break;
		case 0x0e: printf("MVI    C,#0x%02x", code[1]); opbytes = 2; break;
		case 0x0f: printf("RRC"); break;
		case 0x10: printf("NOP"); break;
		case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x12: printf("STAX   D"); break;
		case 0x13: printf("INX    D"); break;
		case 0x14: printf("INR    D"); break;
		case 0x15: printf("DCR    D"); break;
		case 0x16: printf("MVI    D,#0x%02x", code[1]); opbytes = 2; break;
		case 0x17: printf("RAL"); break;
		case 0x18: printf("NOP"); break;
		case 0x19: printf("DAD    D"); break;
		case 0x1a: printf("LDAX   D"); break;
		case 0x1b: printf("DCX    D"); break;
		case 0x1c: printf("INR    E"); break;
		case 0x1d: printf("DCR    E"); break;
		case 0x1e: printf("MVI    E, #0x%02x", code[1]); opbytes = 2; break;
		case 0x1f: printf("RAR"); break;
		case 0x20: printf("NOP"); break;
		case 0x21: printf("LXI    H, #$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x22: printf("SHLD   $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x23: printf("INX    H"); break;
		case 0x24: printf("INR    H"); break;
		case 0x25: printf("DCR    H"); break;
		case 0x26: printf("MVI    H, #0x%02x", code[1]); opbytes = 2; break;
		case 0x27: printf("DAA"); break;
		case 0x28: printf("NOP"); break;
		case 0x29: printf("DAD    H"); break;
		case 0x2a: printf("LHLD   $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x2b: printf("DCX    H"); break;
		case 0x2c: printf("INR    L"); break;
		case 0x2d: printf("DCR    L"); break;
		case 0x2e: printf("MVI    L,#0x%02x", code[1]); opbytes = 2; break;
		case 0x2f: printf("CMA"); break;
		case 0x30: printf("NOP"); break;
		case 0x31: printf("LXI    SP, #$%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x32: printf("STA    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x33: printf("INX    SP"); break;
		case 0x34: printf("INR    M"); break;
		case 0x35: printf("DCR    M"); break;
		case 0x36: printf("MVI    M,#0x%02x", code[1]); opbytes = 2; break;
		case 0x37: printf("STC"); break;
		case 0x38: printf("NOP"); break;
		case 0x39: printf("DAD    SP"); break;
		case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0x3b: printf("DCX    SP"); break;
		case 0x3c: printf("INR    A"); break;
		case 0x3d: printf("DCR    A"); break;
		case 0x3e: printf("MVI    A,#0x%02x", code[1]); opbytes = 2; break;
		case 0x3f: printf("CMC"); break;
		case 0x40: printf("MOV    B,B"); break;
		case 0x41: printf("MOV    B,C"); break;
		case 0x42: printf("MOV    B,D"); break;
		case 0x43: printf("MOV    B,E"); break;
		case 0x44: printf("MOV    B,H"); break;
		case 0x45: printf("MOV    B,L"); break;
		case 0x46: printf("MOV    B,M"); break;
		case 0x47: printf("MOV    B,A"); break;
		case 0x48: printf("MOV    C,B"); break;
		case 0x49: printf("MOV    C,C"); break;
		case 0x4a: printf("MOV    C,D"); break;
		case 0x4b: printf("MOV    C,E"); break;
		case 0x4c: printf("MOV    C,H"); break;
		case 0x4d: printf("MOV    C,L"); break;
		case 0x4e: printf("MOV    C,M"); break;
		case 0x4f: printf("MOV    C,A"); break;
		case 0x50: printf("MOV    D,B"); break;
		case 0x51: printf("MOV    D,C"); break;
		case 0x52: printf("MOV    D,D"); break;
		case 0x53: printf("MOV    D,E"); break;
		case 0x54: printf("MOV    D,H"); break;
		case 0x55: printf("MOV    D,L"); break;
		case 0x56: printf("MOV    D,M"); break;
		case 0x57: printf("MOV    D,A"); break;
		case 0x58: printf("MOV    E,B"); break;
		case 0x59: printf("MOV    E,C"); break;
		case 0x5a: printf("MOV    E,D"); break;
		case 0x5b: printf("MOV    E,E"); break;
		case 0x5c: printf("MOV    E,H"); break;
		case 0x5d: printf("MOV    E,L"); break;
		case 0x5e: printf("MOV    E,M"); break;
		case 0x5f: printf("MOV    E,A"); break;
		case 0x60: printf("MOV    H,B"); break;
		case 0x61: printf("MOV    H,C"); break;
		case 0x62: printf("MOV    H,D"); break;
		case 0x63: printf("MOV    H,E"); break;
		case 0x64: printf("MOV    H,H"); break;
		case 0x65: printf("MOV    H,L"); break;
		case 0x66: printf("MOV    H,M"); break;
		case 0x67: printf("MOV    H,A"); break;
		case 0x68: printf("MOV    L,B"); break;
		case 0x69: printf("MOV    L,C"); break;
		case 0x6a: printf("MOV    L,D"); break;
		case 0x6b: printf("MOV    L,E"); break;
		case 0x6c: printf("MOV    L,H"); break;
		case 0x6d: printf("MOV    L,L"); break;
		case 0x6e: printf("MOV    L,M"); break;
		case 0x6f: printf("MOV    L,A"); break;
		case 0x70: printf("MOV    M,B"); break;
		case 0x71: printf("MOV    M,C"); break;
		case 0x72: printf("MOV    M,D"); break;
		case 0x73: printf("MOV    M,E"); break;
		case 0x74: printf("MOV    M,H"); break;
		case 0x75: printf("MOV    M,L"); break;
		case 0x76: printf("HLT"); break;
		case 0x77: printf("MOV    M,A"); break;
		case 0x78: printf("MOV    A,B"); break;
		case 0x79: printf("MOV    A,C"); break;
		case 0x7a: printf("MOV    A,D"); break;
		case 0x7b: printf("MOV    A,E"); break;
		case 0x7c: printf("MOV    A,H"); break;
		case 0x7d: printf("MOV    A,L"); break;
		case 0x7e: printf("MOV    A,M"); break;
		case 0x7f: printf("MOV    A,A"); break;
		case 0x80: printf("ADD    B"); break;
		case 0x81: printf("ADD    C"); break;
		case 0x82: printf("ADD    D"); break;
		case 0x83: printf("ADD    E"); break;
		case 0x84: printf("ADD    H"); break;
		case 0x85: printf("ADD    L"); break;
		case 0x86: printf("ADD    M"); break;
		case 0x87: printf("ADD    A"); break;
		case 0x88: printf("ADC    B"); break;
		case 0x89: printf("ADC    C"); break;
		case 0x8a: printf("ADC    D"); break;
		case 0x8b: printf("ADC    E"); break;
		case 0x8c: printf("ADC    H"); break;
		case 0x8d: printf("ADC    L"); break;
		case 0x8e: printf("ADC    M"); break;
		case 0x8f: printf("ADC    A"); break;
		case 0x90: printf("SUB    B"); break;
		case 0x91: printf("SUB    C"); break;
		case 0x92: printf("SUB    D"); break;
		case 0x93: printf("SUB    E"); break;
		case 0x94: printf("SUB    H"); break;
		case 0x95: printf("SUB    L"); break;
		case 0x96: printf("SUB    M"); break;
		case 0x97: printf("SUB    A"); break;
		case 0x98: printf("SBB    B"); break;
		case 0x99: printf("SBB    C"); break;
		case 0x9a: printf("SBB    D"); break;
		case 0x9b: printf("SBB    E"); break;
		case 0x9c: printf("SBB    H"); break;
		case 0x9d: printf("SBB    L"); break;
		case 0x9e: printf("SBB    M"); break;
		case 0x9f: printf("SBB    A"); break;
		case 0xa0: printf("ANA    B"); break;
		case 0xa1: printf("ANA    C"); break;
		case 0xa2: printf("ANA    D"); break;
		case 0xa3: printf("ANA    E"); break;
		case 0xa4: printf("ANA    H"); break;
		case 0xa5: printf("ANA    L"); break;
		case 0xa6: printf("ANA    M"); break;
		case 0xa7: printf("ANA    A"); break;
		case 0xa8: printf("XRA    B"); break;
		case 0xa9: printf("XRA    C"); break;
		case 0xaa: printf("XRA    D"); break;
		case 0xab: printf("XRA    E"); break;
		case 0xac: printf("XRA    H"); break;
		case 0xad: printf("XRA    L"); break;
		case 0xae: printf("XRA    M"); break;
		case 0xaf: printf("XRA    A"); break;
		case 0xb0: printf("ORA    B"); break;
		case 0xb1: printf("XRA    C"); break;
		case 0xb2: printf("XRA    D"); break;
		case 0xb3: printf("XRA    E"); break;
		case 0xb4: printf("XRA    H"); break;
		case 0xb5: printf("XRA    L"); break;
		case 0xb6: printf("XRA    M"); break;
		case 0xb7: printf("XRA    A"); break;
		case 0xb8: printf("CMP    B"); break;
		case 0xb9: printf("CMP    C"); break;
		case 0xba: printf("CMP    D"); break;
		case 0xbb: printf("CMP    E"); break;
		case 0xbc: printf("CMP    H"); break;
		case 0xbd: printf("CMP    L"); break;
		case 0xbe: printf("CMP    M"); break;
		case 0xbf: printf("CMP    A"); break;
		case 0xc0: printf("RNZ"); break;
		case 0xc1: printf("POP    B"); break;
		case 0xc2: printf("JNZ    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xc3: printf("JMP    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xc4: printf("CNZ    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xc5: printf("PUSH   B"); break;
		case 0xc6: printf("ADI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xc7: printf("RST    0"); break;
		case 0xc8: printf("RZ"); break;
		case 0xc9: printf("RET    0"); break;
		case 0xca: printf("JZ    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xcb: printf("NOP"); break;
		case 0xcc: printf("CZ    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xcd: printf("CALL    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xce: printf("ACI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xcf: printf("RST    1"); break;
		case 0xd0: printf("RNC"); break;
		case 0xd1: printf("POP    D"); break;
		case 0xd2: printf("JNC    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xd3: printf("OUT    #0x%02x", code[1]); opbytes = 2; break;
		case 0xd4: printf("CNC    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xd5: printf("PUSH   D"); break;
		case 0xd6: printf("SUI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xd7: printf("RST    2"); break;
		case 0xd8: printf("RC"); break;
		case 0xd9: printf("NOP"); break;
		case 0xda: printf("JC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xdb: printf("IN     #0x%02x", code[1]); opbytes = 2; break;
		case 0xdc: printf("CC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xdd: printf("NOP"); break;
		case 0xde: printf("SBI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xdf: printf("RST    3"); break;
		case 0xe0: printf("RPO"); break;
		case 0xe1: printf("POP    H"); break;
		case 0xe2: printf("JPO    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xe3: printf("XTHL"); break;
		case 0xe4: printf("CPO    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xe5: printf("PUSH   H"); break;
		case 0xe6: printf("ANI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xe7: printf("RST    4"); break;
		case 0xe8: printf("RPE"); break;
		case 0xe9: printf("PCHL"); break;
		case 0xea: printf("JPE    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xeb: printf("XCHG"); break;
		case 0xec: printf("CPE    $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xed: printf("NOP"); break;
		case 0xee: printf("XRI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xef: printf("RST    5"); break;
		case 0xf0: printf("RP"); break;
		case 0xf1: printf("POP    PSW"); break;
		case 0xf2: printf("JP     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xf3: printf("DI"); break;
		case 0xf4: printf("CP     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xf5: printf("PUSH   PSW"); break;
		case 0xf6: printf("ORI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xf7: printf("RST    6"); break;
		case 0xf8: printf("RM"); break;
		case 0xf9: printf("SPHL"); break;
		case 0xfa: printf("JM     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xfb: printf("EI"); break;
		case 0xfc: printf("CM     $%02x%02x", code[2], code[1]); opbytes = 3; break;
		case 0xfd: printf("NOP"); break;
		case 0xfe: printf("CPI    #0x%02x", code[1]); opbytes = 2; break;
		case 0xff: printf("RST    7"); break;
	}

	printf("\n");

	return opbytes;
}


int main(int argc, char **argv)
{
	cout << "Starting...\n";

	FILE* file;
	const char* path;

	if (argc < 0)
		path = argv[1];
	else
		path = "C:\\Users\\Ruben.RUOFF\\Documents\\8080Emulator\\SpaceInvadersROM\\invaders";
		

	file = fopen(path, "rb");

	if (file == NULL)
	{
		printf("error: Couldn't open %s\n", argv[1]);
		exit(1);
	}

	//Get the file size and read it into a memory buffer    
	fseek(file, 0L, SEEK_END);
	int fsize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	unsigned char* buffer = new unsigned char[fsize];

	fread(buffer, fsize, 1, file);
	fclose(file);

	int pc = 0;

	while (pc < fsize)
	{
		pc += Disassemble8080Op(buffer, pc);
	}
	return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
