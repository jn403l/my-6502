#include <stdio.h>
#include <stdlib.h>

//TODO: use value uint_8t etc
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

struct Mem {
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

struct CPU {

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

  Byte ReadByte(Byte address, s32& cycles, Mem &memory) {
    Byte Data = memory[address];
    cycles--;
		return Data;
	}  

	static constexpr Byte INS_LDA_IMMEDIATE = 0xA9;
	static constexpr Byte INS_LDA_ZEROPAGE	= 0xA5;
	static constexpr Byte INS_LDA_ZEROPX		= 0xB5;
  static constexpr Byte INS_JSR = 0x20;
  static constexpr Byte INS_LDA_ABS = 0xAD;
  static constexpr Byte INS_LDA_ABSX = 0xBD;
  static constexpr Byte INS_LDA_ABSY = 0xB9;  

	void LDASetStatus() {
		zeroFlag = (accumulator == 0);
		negativeFlag = (accumulator & 0b10000000) > 0;
	}

    s32 Execute(s32 cycles, Mem &memory) {
    const s32 cyclesRequested = cycles;           
    while (cycles > 0) {
      Byte Ins = FetchByte(cycles, memory);
			switch (Ins) {
			case INS_LDA_IMMEDIATE: {
				Byte value = FetchByte(cycles, memory);
				accumulator = value;
				LDASetStatus();
			} break;
			case INS_LDA_ZEROPAGE: {
				Byte zeroPageAddress = FetchByte(cycles, memory);
				accumulator =
					ReadByte(zeroPageAddress, cycles, memory);
				LDASetStatus();
			} break;
      case INS_LDA_ZEROPX: {
        Byte zeroPageAddress = FetchByte(cycles, memory);
        zeroPageAddress += indexRegX;
        cycles--;
				accumulator =
					ReadByte(zeroPageAddress, cycles, memory);
				LDASetStatus();
      } break;
      case INS_JSR: {
        Word SubAddr = FetchWord(cycles, memory);
        //TODO: increment stack pointer
				memory.WriteWord(programCounter -1 , stackPointer, cycles);
				programCounter = SubAddr;
				cycles--;
      } break;
      case INS_LDA_ABS: {

      } break;
      case INS_LDA_ABSX: {

      } break;
      case INS_LDA_ABSY: {

      } break;        
			default: {
				printf("Instruction not handled \n");
			}	break;
			}
    }
    return cyclesRequested - cycles;      
  }
};
