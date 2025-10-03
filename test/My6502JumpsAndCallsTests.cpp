#include <gtest/gtest.h>
#include <emu6502.h>

class My6502LoadRegisterTests : public testing::Test {
public:
  using Byte   = my6502::Byte;
  using CPU    = my6502::CPU;
  using Mem    = my6502::Mem;
  using s32    = my6502::s32;
  using RegPtr = Byte CPU::*;   // alias for pointer-to-member
  
  Mem mem{};
  CPU cpu{};

	virtual void SetUp() {
		cpu.Reset(mem); }
  virtual void TearDown() { ; }

};


static void VerifyUnmodifiedFlagsFromLoadRegister(const my6502::CPU &cpu, const my6502::CPU &CPUCopy) {
	EXPECT_EQ(cpu.carryFlag, CPUCopy.carryFlag);
	EXPECT_EQ(cpu.breakCommand, CPUCopy.breakCommand);
	EXPECT_EQ(cpu.decimalMode, CPUCopy.decimalMode);
	EXPECT_EQ(cpu.interruptDisable, CPUCopy.interruptDisable);
	EXPECT_EQ(cpu.overflowFlag, CPUCopy.overflowFlag);
}

