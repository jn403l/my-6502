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

#if 0
#include "main_6502.h"

int main() {
	Mem mem;
  CPU cpu;
  cpu.Reset(mem);
  // start - inline a little program
  mem[0xFFFC] = CPU::INS_JSR;
  mem[0xFFFD] = 0x42;
  mem[0xFFFE] = 0x42;
  mem[0x4242] = CPU::INS_LDA_IMMEDIATE;
	mem[0x4243] = 0x84;
	// end - inline a little program
  cpu.Execute(8, mem); //immediate(2) + jump(6)
	return 0;
}
#endif
