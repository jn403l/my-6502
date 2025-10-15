#include <gtest/gtest.h>
#include <emu6502.h>

class My6502JumpsAndCallsTest : public testing::Test {
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

TEST_F(My6502JumpsAndCallsTest, CanJumpToASubroutineAndJumpBackAgain) {
  CPU CPUCopy = cpu;
  cpu.Reset(0xFF00, mem);
  mem[0xFF00] = CPU::INS_JSR;
  mem[0xFF01] = 0x00;
  mem[0xFF02] = 0x80;
  mem[0x8000] = CPU::INS_RTS;
  mem[0xFF03] = CPU::INS_LDA_IMMEDIATE;
  mem[0xFF04] = 0x69;
  constexpr s32 expected_cyles = 6 + 6 + 2;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(cpu.stackPointer, CPUCopy.stackPointer);
}

TEST_F(My6502JumpsAndCallsTest, JSRDoesNotAffectTheProcessorStatus) {
  CPU CPUCopy = cpu;
  cpu.Reset(0xFF00, mem);
  mem[0xFF00] = CPU::INS_JSR;
  mem[0xFF01] = 0x00;
  mem[0xFF02] = 0x80;
  constexpr s32 expected_cyles = 6;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
	EXPECT_EQ(cpu.processorStatus, CPUCopy.processorStatus);
}
