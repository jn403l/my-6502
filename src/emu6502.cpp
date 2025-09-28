#include <emu6502.h>

namespace my6502 {

  s32 CPU::Execute(s32 cycles, Mem &memory) {
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
        accumulator = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(accumulator);
      } break;
      case INS_LDX_ZEROPAGE: {
				Word address = AddrZeroPage(cycles, memory);
        indexRegX = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegX);
      } break;
      case INS_LDY_ZEROPAGE: {
				Word address = AddrZeroPage(cycles, memory);
        indexRegY = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegY);
      } break;
      case INS_LDA_ZEROPX: {
        Word address = AddrZeroPageX(cycles, memory);            
        accumulator = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(accumulator);
      } break;
      case INS_LDY_ZEROPX: {
        Word address = AddrZeroPageX(cycles, memory);            
        indexRegY = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegY);
      } break;
			case INS_LDX_ZEROPY: {
        Word address = AddrZeroPageY(cycles, memory);            
        indexRegX = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegX);
      } break;        
      case INS_JSR: {
        Word SubAddr = FetchWord(cycles, memory);
        // TODO: increment stack pointer
        memory.WriteWord(programCounter - 1, stackPointer, cycles);
        programCounter = SubAddr;
        cycles--;
      } break;
      case INS_LDA_ABS: {
				Word address = AddrAbsolute(cycles, memory);
        accumulator = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(accumulator);
      } break;
      case INS_LDX_ABS: {
				Word address = AddrAbsolute(cycles, memory);
        indexRegX = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegX);
      } break;
      case INS_LDY_ABS: {
				Word address = AddrAbsolute(cycles, memory);
        indexRegY = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegY);
      } break;
      case INS_LDA_ABSX: {
				Word address = AddrAbsoluteX(cycles, memory);
        accumulator = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(accumulator);
      } break;
      case INS_LDY_ABSX: {
				Word address = AddrAbsoluteX(cycles, memory);
        indexRegY = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegY);        
      } break;        
      case INS_LDA_ABSY: {
				Word address = AddrAbsoluteY(cycles, memory);
        accumulator = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(accumulator);
      } break;
      case INS_LDX_ABSY: {
				Word address = AddrAbsoluteY(cycles, memory);
        indexRegX = ReadByte(address, cycles, memory);
        LoadRegisterSetStatus(indexRegX);        
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

	Word CPU::AddrAbsoluteY(s32 &cycles, const Mem &memory) {
		Word absAddr = FetchWord(cycles, memory);
		Word absAddrY = absAddr + indexRegY;
		if ((absAddrY - absAddr) >= 0xFF) {
			cycles--;
		}
		return absAddrY;
	}          
}
