#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem {
  static constexpr u32 MAX_MEM = 1024 * 64;
  Byte Data[MAX_MEM];

	void Initialize() {
		for (u32 i = 0; i < MAX_MEM; i++) {
			Data[i] = 0;
		}
	}

 	/* readable access */
	Byte& operator[](u32 address)  {
		return Data[address];
	}
        
	/* readable access */
	Byte operator[](u32 address) const {
		return Data[address];
	}
};

struct CPU {

  Word programCounter;
  Byte stackPointer;

  Byte accumulator, indexRegX, indexRegY;

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

  Byte FetchByte(u32& Cycles, Mem &memory) {
    Byte Data = memory[programCounter];
    programCounter++;
    Cycles--;
		return Data;
	}

	static constexpr Byte INS_LDA_IMMEDIATE = 0xA9;

  void Execute(u32 Cycles, Mem &memory) {
    while (Cycles > 0) {
      Byte Ins = FetchByte(Cycles, memory);
			switch (Ins) {
			case INS_LDA_IMMEDIATE: {
				Byte value = FetchByte(Cycles, memory);
				accumulator = value;
				zeroFlag = (accumulator == 0);
				negativeFlag = (accumulator & 0b10000000) > 0;
			} break;
			default: {
				printf("Instruction not handled \n");
			}	break;
			}
                        
		}
	}
};

int main() {
	Mem mem;
  CPU cpu;
  cpu.Reset(mem);
  // start - inline a little program
  mem[0xFFFC] = CPU::INS_LDA_IMMEDIATE;
  mem[0xFFFD] = 0x42;
	// end - inline a little program
  cpu.Execute(2, mem);
	return 0;
}
