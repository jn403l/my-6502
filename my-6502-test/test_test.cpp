#include <gtest/gtest.h>
#include "../main_6502.h"

class My6502Test1 : public testing::Test {
public:
  Mem mem;
	CPU cpu;
	virtual void SetUp() {
		cpu.Reset(mem); }
	virtual void TearDown() { ; }
};


static void VerifyUnmodifiedFlagsFromLDA(const CPU &cpu, const CPU &CPUCopy) {
	EXPECT_EQ(cpu.carryFlag, CPUCopy.carryFlag);
	EXPECT_EQ(cpu.breakCommand, CPUCopy.breakCommand);
	EXPECT_EQ(cpu.decimalMode, CPUCopy.decimalMode);
	EXPECT_EQ(cpu.interruptDisable, CPUCopy.interruptDisable);
	EXPECT_EQ(cpu.overflowFlag, CPUCopy.overflowFlag);          
}

TEST_F(My6502Test1, CPUDoesNothingWhenWeExecuteZeroCycles) {
  // given:
  constexpr s32 num_cycles = 0;

  // when:
  s32 CyclesUsed = cpu.Execute(num_cycles, mem);

  // then
	EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(My6502Test1, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction) {
	// given:
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x84;

	// when:
	s32 CyclesUsed = cpu.Execute(1, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(CyclesUsed, 2);
}

TEST_F(My6502Test1, LDAImmediateCanLoadAValueIntoTheARegister) {
	// given:
  // start - inline a little program
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x84;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x84);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_TRUE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAImmediateCanAffectTheZeroFlag) {
  // given:
	cpu.accumulator = 0x44;
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x0;

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x0);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_TRUE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister) {
	// given:
  // start - inline a little program
  mem[0XFFFC] = CPU::INS_LDA_ZEROPAGE;
  mem[0XFFFD] = 0x42;
	mem[0x0042] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(3, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, 3);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister) {
  // given:
  cpu.indexRegX = 5; 
  // start - inline a little program
  mem[0XFFFC] = CPU::INS_LDA_ZEROPX;
  mem[0XFFFD] = 0x42;
	mem[0x0047] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;  
	s32 CyclesUsed = cpu.Execute(4, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, 4);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps) {
  // given:
  cpu.indexRegX = 0xFF; 
  // start - inline a little program
  mem[0XFFFC] = CPU::INS_LDA_ZEROPX;
  mem[0XFFFD] = 0x80;
	mem[0x007F] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(4, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, 4);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteCanLoadAValueIntoTheRegister) {
  // given:
  mem[0XFFFC] = CPU::INS_LDA_ABS;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4480] = 0x69;

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteXLoadAValueIntoTheRegister) {
  // given:
	cpu.indexRegX = 1;
  mem[0XFFFC] = CPU::INS_LDA_ABSX;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4481] = 0x69;

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteXLoadAValueIntoTheRegisterWhenItCrossesAPageBoundary) {
  // given:
	cpu.indexRegX = 0xFF;
  mem[0XFFFC] = CPU::INS_LDA_ABSX;
  mem[0XFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
	mem[0x4501] = 0x69; // 0x4402+0xFF crosses page boundary

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteYLoadAValueIntoTheRegister) {
  // given:
	cpu.indexRegY = 1;
  mem[0XFFFC] = CPU::INS_LDA_ABSY;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4481] = 0x69; // 

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteYLoadAValueIntoTheRegisterWhenItCrossesAPageBoundary) {
  // given:
	cpu.indexRegY = 0xFF;
  mem[0XFFFC] = CPU::INS_LDA_ABSY;
  mem[0XFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
	mem[0x4501] = 0x69; // 0x4402+0xFF crosses page boundary

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAIndirectXCanLoadAValueIntoTheARegister) {
  // given:
	cpu.indexRegX = 0x04;
  mem[0XFFFC] = CPU::INS_LDA_INDIRECTX;
  mem[0XFFFD] = 0x02;
  mem[0x0006] = 0x00; // 0x2 + 0x4
  mem[0x0007] = 0x80; 
	mem[0x8000] = 0x30;

	// when:
	constexpr s32 expected_cycles = 6;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x30);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAIndirectYCanLoadAValueIntoTheARegister) {
  // given:
 	cpu.indexRegY = 0x04;
  mem[0XFFFC] = CPU::INS_LDA_INDIRECTY;
  mem[0XFFFD] = 0x02;
  mem[0x0002] = 0x00; // 0x2 + 0x4
  mem[0x0003] = 0x80; // 0x4402+0xFF crosses page boundary
	mem[0x8004] = 0x69;

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPage) {
  // given:
 	cpu.indexRegY = 0xFF;
  mem[0XFFFC] = CPU::INS_LDA_INDIRECTY;
  mem[0XFFFD] = 0x02;
  mem[0x0002] = 0x02; 
  mem[0x0003] = 0x80; 
	mem[0x8101] = 0x69; // 0x8002+0xFF crosses page boundary

	// when:
	constexpr s32 expected_cycles = 6;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem); // immediate(2) + jump(6)

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy);        
}
