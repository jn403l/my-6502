#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace my6502 {
  // TODO: use value uint_8t etc
  using Byte = unsigned char;
  using Word = unsigned short;

  using u32 = unsigned int;
  using s32 = signed int;

  struct Mem;
  struct CPU;
}


struct my6502::Mem {
  static constexpr u32 MAX_MEM = 1024 * 64;
  Byte Data[MAX_MEM];

	void Initialize() {
		for (u32 i = 0; i < MAX_MEM; i++) {
			Data[i] = 0;
		}
	}

 	/* write access */
	Byte& operator[](u32 address)  {
		return Data[address];
	}
        
	/* readable access */
	Byte operator[](u32 address) const {
		return Data[address];
	}

	void WriteWord(Word value,
								 u32 address,
								 s32& cycles) {
		Data[address]			= value & 0xFF;
		Data[address + 1] = (value >> 8);
		cycles -= 2;
	}
};

struct my6502::CPU {

  Word programCounter;
  Word stackPointer;

  Byte accumulator, indexRegX, indexRegY;

	//TODO: implement in a single byte(?)
  Byte carryFlag : 1;
  Byte zeroFlag : 1;
  Byte interruptDisable : 1;
  Byte decimalMode : 1;
  Byte breakCommand : 1;
  Byte overflowFlag : 1;
  Byte negativeFlag : 1;

  void Reset(Mem& memory) {
    programCounter = 0xFFFC;
    stackPointer = 0x0100;
    carryFlag = zeroFlag = interruptDisable = decimalMode = breakCommand = overflowFlag = negativeFlag = 0;
    accumulator = indexRegX = indexRegY = 0;
		memory.Initialize();
  }

  Byte FetchByte(s32& cycles, Mem &memory) {
    Byte Data = memory[programCounter];
    programCounter++;
    cycles--;
		return Data;
  }

  Word FetchWord(s32 &cycles, Mem &memory) {
		// 6502 is little endian
    Word Data = memory[programCounter];
    programCounter++;

		Data |= (memory[programCounter] << 8);
    programCounter++;
    cycles -= 2;    
		return Data;
  }  

  Byte ReadByte(Word address, s32 &cycles, Mem &memory) {
    Byte Data = memory[address];
    cycles--;
    return Data;
  }

  Word ReadWord(Word address, s32 &cycles, Mem &memory) {
    Byte loByte = ReadByte(address, cycles, memory);
    Byte hiByte = ReadByte(address + 1, cycles, memory);
    return loByte | (hiByte << 8);
  }

  // LDA
  static constexpr Byte INS_LDA_IMMEDIATE = 0xA9;
	static constexpr Byte INS_LDA_ZEROPAGE	= 0xA5;
	static constexpr Byte INS_LDA_ZEROPX		= 0xB5;
  static constexpr Byte INS_JSR           = 0x20;
  static constexpr Byte INS_LDA_ABS       = 0xAD;
  static constexpr Byte INS_LDA_ABSX      = 0xBD;
  static constexpr Byte INS_LDA_ABSY      = 0xB9;
  static constexpr Byte INS_LDA_INDIRECTX = 0xA1;
  static constexpr Byte INS_LDA_INDIRECTY = 0xB1;
  // LDX
  static constexpr Byte INS_LDX_IMMEDIATE = 0xA2;
  static constexpr Byte INS_LDX_ZEROPAGE  = 0xA6;
  static constexpr Byte INS_LDX_ZEROPY    = 0xB6;
  static constexpr Byte INS_LDX_ABS       = 0xAE;
  static constexpr Byte INS_LDX_ABSY      = 0xBE;  
  // LDY
  static constexpr Byte INS_LDY_IMMEDIATE = 0xA0;
  static constexpr Byte INS_LDY_ZEROPAGE  = 0xA4;
  static constexpr Byte INS_LDY_ZEROPX    = 0xB4;
  static constexpr Byte INS_LDY_ABS       = 0xAC;
  static constexpr Byte INS_LDY_ABSX      = 0xBC;
  /* Sets the correct process status after a load register instruction*/
	void LoadRegisterSetStatus(Byte Register) {
		zeroFlag = (Register == 0);
		negativeFlag = (Register & 0b10000000) > 0;
	}

    s32 Execute(s32 cycles, Mem &memory) {
    const s32 cyclesRequested = cycles;           
    while (cycles > 0) {
      Byte Ins = FetchByte(cycles, memory);
			switch (Ins) {
			case INS_LDA_IMMEDIATE: {
				accumulator = FetchByte(cycles, memory);
				LoadRegisterSetStatus(accumulator);
      } break;
			case INS_LDX_IMMEDIATE: {
				indexRegX = FetchByte(cycles, memory);
				LoadRegisterSetStatus(indexRegX);
                        } break;
			case INS_LDY_IMMEDIATE: {
				indexRegY = FetchByte(cycles, memory);
				LoadRegisterSetStatus(indexRegY);
			} break;                          
			case INS_LDA_ZEROPAGE: {
				Byte zeroPageAddress = FetchByte(cycles, memory);
				accumulator =
					ReadByte(zeroPageAddress, cycles, memory);
				LoadRegisterSetStatus(accumulator);
			} break;
      case INS_LDA_ZEROPX: {
        Byte zeroPageAddress = FetchByte(cycles, memory);
        zeroPageAddress += indexRegX;
        cycles--;
				accumulator =
					ReadByte(zeroPageAddress, cycles, memory);
				LoadRegisterSetStatus(accumulator);
      } break;
      case INS_JSR: {
        Word SubAddr = FetchWord(cycles, memory);
        //TODO: increment stack pointer
				memory.WriteWord(programCounter -1 , stackPointer, cycles);
				programCounter = SubAddr;
				cycles--;
      } break;
      case INS_LDA_ABS: {
        Word AbsAddress = FetchWord(cycles, memory);
        accumulator = ReadByte(AbsAddress, cycles, memory);
				LoadRegisterSetStatus(accumulator);        
      } break;
      case INS_LDA_ABSX: {
        Word AbsAddress = FetchWord(cycles, memory);
        Word AbsAddressX = AbsAddress + indexRegX;
        accumulator = ReadByte(AbsAddressX, cycles, memory);
				LoadRegisterSetStatus(accumulator);        
        if ((AbsAddressX - AbsAddress) >= 0xFF) {
          cycles--;
        }
      } break;
      case INS_LDA_ABSY: {
        Word AbsAddress = FetchWord(cycles, memory);
        Word AbsAddressY = AbsAddress + indexRegY;
        accumulator = ReadByte(AbsAddressY, cycles, memory);
				LoadRegisterSetStatus(accumulator);
        if ((AbsAddressY - AbsAddress) >= 0xFF) {
          cycles--;
        }
      } break;
      case INS_LDA_INDIRECTX: {
        Byte zPAddress = FetchByte(cycles, memory);
        zPAddress += indexRegX;
        cycles--;
        Word effectiveAddr = ReadWord(zPAddress, cycles, memory);
        accumulator = ReadByte(effectiveAddr, cycles, memory);
				LoadRegisterSetStatus(accumulator);        
      } break;
      case INS_LDA_INDIRECTY: {
        Byte zPAddress = FetchByte(cycles, memory);
        Word effectiveAddr = ReadWord(zPAddress, cycles, memory);
        Word effectiveAddrY = effectiveAddr + indexRegY;
        accumulator = ReadByte(effectiveAddrY, cycles, memory);
				LoadRegisterSetStatus(accumulator);        
        if ((effectiveAddrY - effectiveAddr) >= 0xFF) {
          cycles--;
        }
      } break;       
			default: {
        printf("Instruction %d not handled \n", Ins);
        throw -1;                  
			}	break;
			}
    }
    return cyclesRequested - cycles;      
  }
};
