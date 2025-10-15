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

  void TestLoadRegisterImmediate(Byte Opcode,
                                 RegPtr RegisterToTest);
  void TestLoadRegisterZeroPage(Byte OpcodeToTest,
                                RegPtr RegisterToTest);
  void TestLoadRegisterZeroPageX(Byte OpcodeToTest,
                                 RegPtr RegisterToTest);
  void TestLoadRegisterZeroPageY(Byte OpcodeToTest,
                                 RegPtr RegisterToTest);
  void TestLoadRegisterAbsolute(Byte OpcodeToTest,
                                RegPtr RegisterToTest);
  void TestLoadRegisterAbsoluteX(Byte OpcodeToTest,
                                 RegPtr RegisterToTest);
  void TestLoadRegisterAbsoluteY(Byte OpcodeToTest,
                                 RegPtr RegisterToTest);
  void TestLoadRegisterAbsoluteYWhenCrossingPage(
      Byte OpcodeToTest, RegPtr RegisterToTest);
  void TestLoadRegisterAbsoluteXWhenCrossingPage(
      Byte OpcodeToTest, RegPtr RegisterToTest);
};


static void VerifyUnmodifiedFlagsFromLoadRegister(const my6502::CPU &cpu, const my6502::CPU &CPUCopy) {
	EXPECT_EQ(cpu.Flag.carryFlag, CPUCopy.Flag.carryFlag);
	EXPECT_EQ(cpu.Flag.breakCommand, CPUCopy.Flag.breakCommand);
	EXPECT_EQ(cpu.Flag.decimalMode, CPUCopy.Flag.decimalMode);
	EXPECT_EQ(cpu.Flag.interruptDisable, CPUCopy.Flag.interruptDisable);
	EXPECT_EQ(cpu.Flag.overflowFlag, CPUCopy.Flag.overflowFlag);
}

TEST_F(My6502LoadRegisterTests, CPUDoesNothingWhenWeExecuteZeroCycles) {
  // given:
  constexpr s32 num_cycles = 0;

  // when:
  s32 CyclesUsed = cpu.Execute(num_cycles, mem);

  // then
	EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(My6502LoadRegisterTests, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction) {
  // given:
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x84;

	// when:
	s32 CyclesUsed = cpu.Execute(1, mem); // immediate(2)

	// then:
	EXPECT_EQ(CyclesUsed, 2);
}

void My6502LoadRegisterTests::TestLoadRegisterImmediate(Byte OpcodeToTest, RegPtr RegisterToTest) {
  mem[0XFFFC] = OpcodeToTest;
	mem[0XFFFD] = 0x84;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x84);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_TRUE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);  
}

TEST_F(My6502LoadRegisterTests, LDAImmediateCanLoadAValueIntoTheARegister) {
  TestLoadRegisterImmediate(CPU::INS_LDA_IMMEDIATE, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDXImmediateCanLoadAValueIntoTheXRegister) {
  TestLoadRegisterImmediate(CPU::INS_LDX_IMMEDIATE, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDYImmediateCanLoadAValueIntoTheYRegister) {
  TestLoadRegisterImmediate(CPU::INS_LDY_IMMEDIATE, &CPU::indexRegY);
}

TEST_F(My6502LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag) {
  // given:
	cpu.accumulator = 0x44;
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x0;

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x0);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_TRUE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502LoadRegisterTests::TestLoadRegisterZeroPage(Byte OpcodeToTest, RegPtr RegisterToTest) {
	// given:
  // start - inline a little program
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x42;
	mem[0x0042] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(3, mem); 

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, 3);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
  VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAZeroPageCanLoadAValueIntoTheARegister) {
  TestLoadRegisterZeroPage(CPU::INS_LDA_ZEROPAGE, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDXZeroPageCanLoadAValueIntoTheXRegister) {
  TestLoadRegisterZeroPage(CPU::INS_LDX_ZEROPAGE, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDYZeroPageCanLoadAValueIntoTheYRegister) {
  TestLoadRegisterZeroPage(CPU::INS_LDY_ZEROPAGE, &CPU::indexRegY);
}

void My6502LoadRegisterTests::TestLoadRegisterZeroPageX(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
  cpu.indexRegX = 5;
  // start - inline a little program
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x42;
	mem[0x0047] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;  
	s32 CyclesUsed = cpu.Execute(4, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, 4);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502LoadRegisterTests::TestLoadRegisterZeroPageY(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
  cpu.indexRegY = 5;
  // start - inline a little program
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x42;
	mem[0x0047] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;  
	s32 CyclesUsed = cpu.Execute(4, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, 4);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegister) {
  TestLoadRegisterZeroPageX(CPU::INS_LDA_ZEROPX, &CPU::accumulator);
}

// TODO: function might not be applicable
TEST_F(My6502LoadRegisterTests, LDXZeroPageYCanLoadAValueIntoTheXRegister) {
  TestLoadRegisterZeroPageY(CPU::INS_LDX_ZEROPY, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDYZeroPageXCanLoadAValueIntoTheYRegister) {
  TestLoadRegisterZeroPageX(CPU::INS_LDY_ZEROPX, &CPU::indexRegY);
}

TEST_F(My6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps) {
  // given:
  cpu.indexRegX = 0xFF; 
  // start - inline a little program
  mem[0XFFFC] = CPU::INS_LDA_ZEROPX;
  mem[0XFFFD] = 0x80;
	mem[0x007F] = 0x69;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(4, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, 4);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);        
}
void My6502LoadRegisterTests::TestLoadRegisterAbsolute(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
  cpu.Flag.zeroFlag = cpu.Flag.negativeFlag = true;
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4480] = 0x69;

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);        
}

TEST_F(My6502LoadRegisterTests, LDAAbsoluteCanLoadAValueIntoTheARegister) {
  // given:
  TestLoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDXAbsoluteCanLoadAValueIntoTheXRegister) {
  // given:
  TestLoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDYAbsoluteCanLoadAValueIntoTheYRegister) {
  // given:
  TestLoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::indexRegY);
}

void My6502LoadRegisterTests::TestLoadRegisterAbsoluteX(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
  cpu.Flag.zeroFlag = cpu.Flag.negativeFlag = true;
	cpu.indexRegX = 1;
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4481] = 0x69;

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502LoadRegisterTests::TestLoadRegisterAbsoluteY(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
  cpu.Flag.zeroFlag = cpu.Flag.negativeFlag = true;
	cpu.indexRegY = 1;
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
	mem[0x4481] = 0x69;

	// when:
	constexpr s32 expected_cycles = 4;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAAbsoluteXLoadAValueIntoTheARegister) {
  // given:
  TestLoadRegisterAbsoluteX(CPU::INS_LDA_ABSX, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDXAbsoluteYLoadAValueIntoTheXRegister)
{
  // given:
  TestLoadRegisterAbsoluteY(CPU::INS_LDX_ABSY, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDYAbsoluteXLoadAValueIntoTheYRegister) {
  // given:
  TestLoadRegisterAbsoluteX(CPU::INS_LDY_ABSX, &CPU::indexRegY);
}

void My6502LoadRegisterTests::TestLoadRegisterAbsoluteXWhenCrossingPage(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
	cpu.indexRegX = 0xFF;
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
	mem[0x4501] = 0x69; // 0x4402+0xFF crosses page boundary

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAAbsoluteXLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary) {
  // given:
  TestLoadRegisterAbsoluteXWhenCrossingPage(CPU::INS_LDA_ABSX, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDYAbsoluteXLoadAValueIntoTheYRegisterWhenItCrossesAPageBoundary) {
  // given:
  TestLoadRegisterAbsoluteXWhenCrossingPage(CPU::INS_LDY_ABSX, &CPU::indexRegY);
}

TEST_F(My6502LoadRegisterTests, LDAAbsoluteYLoadAValueIntoTheARegister) {
  // given:
  TestLoadRegisterAbsoluteY(CPU::INS_LDA_ABSY, &CPU::accumulator);
}

void My6502LoadRegisterTests::TestLoadRegisterAbsoluteYWhenCrossingPage(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given:
	cpu.indexRegY = 0xFF;
  mem[0XFFFC] = OpcodeToTest;
  mem[0XFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
	mem[0x4501] = 0x69; // 0x4402+0xFF crosses page boundary

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAAbsoluteYLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary) {
  TestLoadRegisterAbsoluteYWhenCrossingPage(CPU::INS_LDA_ABSY, &CPU::accumulator);
}

TEST_F(My6502LoadRegisterTests, LDXAbsoluteYLoadAValueIntoTheXRegisterWhenItCrossesAPageBoundary) {
  TestLoadRegisterAbsoluteYWhenCrossingPage(CPU::INS_LDX_ABSY, &CPU::indexRegX);
}

TEST_F(My6502LoadRegisterTests, LDAIndirectXCanLoadAValueIntoTheARegister) {
  // given:
  cpu.Flag.zeroFlag = cpu.Flag.negativeFlag = true;  
	cpu.indexRegX = 0x04;
  mem[0XFFFC] = CPU::INS_LDA_INDIRECTX;
  mem[0XFFFD] = 0x02;
  mem[0x0006] = 0x00; // 0x2 + 0x4
  mem[0x0007] = 0x80; 
	mem[0x8000] = 0x30;

	// when:
	constexpr s32 expected_cycles = 6;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x30);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegister) {
  // given:
  cpu.Flag.zeroFlag = cpu.Flag.negativeFlag = true;  
 	cpu.indexRegY = 0x04;
  mem[0XFFFC] = CPU::INS_LDA_INDIRECTY;
  mem[0XFFFD] = 0x02;
  mem[0x0002] = 0x00; // 0x2 + 0x4
  mem[0x0003] = 0x80; // 0x4402+0xFF crosses page boundary
	mem[0x8004] = 0x69;

	// when:
	constexpr s32 expected_cycles = 5;
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPage) {
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
	s32 CyclesUsed = cpu.Execute(expected_cycles, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x69);
	EXPECT_EQ(CyclesUsed, expected_cycles);
	EXPECT_FALSE(cpu.Flag.zeroFlag);
	EXPECT_FALSE(cpu.Flag.negativeFlag);
  VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

