#include <emu6502.h>

namespace my6502 {

	s32 CPU::Execute(s32 cycles, Mem &memory) {

		/** Load a register from the memory address **/
		auto LoadRegister = [&cycles, &memory, this](Word address, Byte &register_param) {
			register_param = ReadByte(address, cycles, memory);
			LoadRegisterSetStatus(register_param);
		};
  
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
				Word address = AddrZeroPage(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_LDX_ZEROPAGE: {
        Word address = AddrZeroPage(cycles, memory);
				LoadRegister(address, indexRegX);
      } break;
      case INS_LDY_ZEROPAGE: {
				Word address = AddrZeroPage(cycles, memory);
				LoadRegister(address, indexRegY);
      } break;
      case INS_LDA_ZEROPX: {
        Word address = AddrZeroPageX(cycles, memory);            
				LoadRegister(address, accumulator);
      } break;
      case INS_LDY_ZEROPX: {
        Word address = AddrZeroPageX(cycles, memory);            
				LoadRegister(address, indexRegY);
      } break;
			case INS_LDX_ZEROPY: {
        Word address = AddrZeroPageY(cycles, memory);            
				LoadRegister(address, indexRegX);
      } break;        
      case INS_JSR: {
        Word SubAddr = FetchWord(cycles, memory);
				PushPCToStack(cycles, memory);
        programCounter = SubAddr;
        cycles--;
      } break;
			case INS_RTS: {
				Word ReturnAddress = PopWordFromStack(cycles, memory);
				programCounter = ReturnAddress + 1;
				cycles -= 2;
			} break;
      case INS_LDA_ABS: {
        Word address = AddrAbsolute(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_LDX_ABS: {
				Word address = AddrAbsolute(cycles, memory);
				LoadRegister(address, indexRegX);
      } break;
      case INS_LDY_ABS: {
				Word address = AddrAbsolute(cycles, memory);
				LoadRegister(address, indexRegY);
      } break;
      case INS_LDA_ABSX: {
				Word address = AddrAbsoluteX(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_LDY_ABSX: {
				Word address = AddrAbsoluteX(cycles, memory);
				LoadRegister(address, indexRegY);
      } break;        
      case INS_LDA_ABSY: {
				Word address = AddrAbsoluteY(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_LDX_ABSY: {
				Word address = AddrAbsoluteY(cycles, memory);
				LoadRegister(address, indexRegX);
      } break;        
      case INS_LDA_INDIRECTX: {
				Word address = AddrIndirectX(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_STA_INDIRECTX: {
				Word address = AddrIndirectX(cycles, memory);
				WriteByte(accumulator, address, cycles, memory);
      } break;        
      case INS_LDA_INDIRECTY: {
				Word address = AddrIndirectY(cycles, memory);
				LoadRegister(address, accumulator);
      } break;
      case INS_STA_INDIRECTY: {
				Word address = AddrIndirectY_6(cycles, memory);
				WriteByte(accumulator, address, cycles, memory);
      } break;        
      case INS_STA_ZEROPAGE: {
        Word address = AddrZeroPage(cycles, memory);
				WriteByte(accumulator, address, cycles, memory);
      } break;
      case INS_STX_ZEROPAGE: {
        Word address = AddrZeroPage(cycles, memory);
				WriteByte(indexRegX, address, cycles, memory);
      } break;
      case INS_STY_ZEROPAGE: {
        Word address = AddrZeroPage(cycles, memory);
				WriteByte(indexRegY, address, cycles, memory);
      } break;
      case INS_STA_ABSOLUTE: {
        Word address = AddrAbsolute(cycles, memory);
				WriteByte(accumulator, address, cycles, memory);
      } break;
      case INS_STX_ABSOLUTE: {
        Word address = AddrAbsolute(cycles, memory);
				WriteByte(indexRegX, address, cycles, memory);
      } break;
      case INS_STY_ABSOLUTE: {
        Word address = AddrAbsolute(cycles, memory);
				WriteByte(indexRegY, address, cycles, memory);
      } break;
      case INS_STA_ZEROPAGEX: {
        Word address = AddrZeroPageX(cycles, memory);
				WriteByte(accumulator, address, cycles, memory);
      } break;
      case INS_STY_ZEROPAGEX: {
        Word address = AddrZeroPageX(cycles, memory);
				WriteByte(indexRegY, address, cycles, memory);
      } break;
      case INS_STA_ABSOLUTEX: {
        Word address = AddrAbsoluteX_5(cycles, memory);
        WriteByte(accumulator, address, cycles, memory);
      } break;
      case INS_STA_ABSOLUTEY: {
        Word address = AddrAbsoluteY_5(cycles, memory);
        WriteByte(accumulator, address, cycles, memory);
      } break;
      default: {
        printf("Instruction %d not handled \n", Ins);
        throw -1;
      } break;
      }
    }
    return cyclesRequested - cycles;
  }


	Word CPU::AddrZeroPage(s32& cycles, const Mem& memory) {
		Byte zeroPageAddr = FetchByte(cycles, memory);
		return zeroPageAddr;
	}

	Word CPU::AddrZeroPageX(s32& cycles, const Mem& memory) {
		Byte zeroPageAddr = FetchByte(cycles, memory);
		zeroPageAddr += indexRegX;
		cycles--;          
		return zeroPageAddr;
	}

	Word CPU::AddrZeroPageY(s32& cycles, const Mem& memory) {
		Byte zeroPageAddr = FetchByte(cycles, memory);
		zeroPageAddr += indexRegY;
		cycles--;          
		return zeroPageAddr;
	}        

	Word CPU::AddrAbsolute(s32& cycles, const Mem& memory) {
 		Word absAddr = FetchWord(cycles, memory);              
		return absAddr;
	}
	
	Word CPU::AddrAbsoluteX(s32& cycles, const Mem& memory) {
		Word absAddr = FetchWord(cycles, memory);
		Word absAddrX = absAddr + indexRegX;
		if ((absAddrX - absAddr) >= 0xFF) {
			cycles--;
		}
		return absAddrX;
	}

	Word CPU::AddrAbsoluteX_5(s32& cycles, const Mem& memory) {
		Word absAddr = FetchWord(cycles, memory);
		Word absAddrX = absAddr + indexRegX;
    cycles--;
		return absAddrX;
	}

	Word CPU::AddrAbsoluteY(s32 &cycles, const Mem &memory) {
		Word absAddr = FetchWord(cycles, memory);
		Word absAddrY = absAddr + indexRegY;
		if ((absAddrY - absAddr) >= 0xFF) {
			cycles--;
		}
		return absAddrY;
	}

	Word CPU::AddrAbsoluteY_5(s32 &cycles, const Mem &memory) {
		Word absAddr = FetchWord(cycles, memory);
		Word absAddrY = absAddr + indexRegY;
    cycles--;
		return absAddrY;
	}

	Word CPU::AddrIndirectX(s32 &cycles, const Mem &memory) {
		Byte zPAddress = FetchByte(cycles, memory);
		zPAddress += indexRegX;
		cycles--;
		Word effectiveAddr = ReadWord(zPAddress, cycles, memory);
		return effectiveAddr;
	}

	Word CPU::AddrIndirectY(s32 &cycles, const Mem &memory) {
		Byte zPAddress = FetchByte(cycles, memory);
		Word effectiveAddr = ReadWord(zPAddress, cycles, memory);
		Word effectiveAddrY = effectiveAddr + indexRegY;
		if ((effectiveAddrY - effectiveAddr) >= 0xFF) {
			cycles--;
		}
		return effectiveAddrY;
	}

	Word CPU::AddrIndirectY_6(s32 &cycles, const Mem &memory) {
		Byte zPAddress = FetchByte(cycles, memory);
		Word effectiveAddr = ReadWord(zPAddress, cycles, memory);
		Word effectiveAddrY = effectiveAddr + indexRegY;
    cycles--;
		return effectiveAddrY;
	}     
        
}
