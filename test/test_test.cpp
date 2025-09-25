#include <gtest/gtest.h>
#include <main_6502.h>

class My6502Test1 : public testing::Test {
public:
  my6502::Mem mem;
	my6502::CPU cpu;
	virtual void SetUp() {
		cpu.Reset(mem); }
  virtual void TearDown() { ; }

  void TestLoadRegisterImmediate(my6502::Byte Opcode,
                                 my6502::Byte my6502::CPU::*Register);
  void TestLoadRegisterZeroPage(my6502::Byte OpcodeToTest,
                                my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterZeroPageX(my6502::Byte OpcodeToTest,
                                 my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterZeroPageY(my6502::Byte OpcodeToTest,
                                 my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterAbsolute(my6502::Byte OpcodeToTest,
                                my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterAbsoluteX(my6502::Byte OpcodeToTest,
                                 my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterAbsoluteY(my6502::Byte OpcodeToTest,
                                 my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterAbsoluteYWhenCrossingPage(
      my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest);
  void TestLoadRegisterAbsoluteXWhenCrossingPage(
      my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest);
};


static void VerifyUnmodifiedFlagsFromLoadRegister(const my6502::CPU &cpu, const my6502::CPU &CPUCopy) {
	EXPECT_EQ(cpu.carryFlag, CPUCopy.carryFlag);
	EXPECT_EQ(cpu.breakCommand, CPUCopy.breakCommand);
	EXPECT_EQ(cpu.decimalMode, CPUCopy.decimalMode);
	EXPECT_EQ(cpu.interruptDisable, CPUCopy.interruptDisable);
	EXPECT_EQ(cpu.overflowFlag, CPUCopy.overflowFlag);
}

TEST_F(My6502Test1, CPUDoesNothingWhenWeExecuteZeroCycles) {
  // given:
  using namespace my6502;
  constexpr s32 num_cycles = 0;

  // when:
  s32 CyclesUsed = cpu.Execute(num_cycles, mem);

  // then
	EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(My6502Test1, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction) {
  // given:
  using namespace my6502;
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x84;

	// when:
	s32 CyclesUsed = cpu.Execute(1, mem); // immediate(2)

	// then:
	EXPECT_EQ(CyclesUsed, 2);
}

void My6502Test1::TestLoadRegisterImmediate(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  using namespace my6502;        
  mem[0XFFFC] = OpcodeToTest;
	mem[0XFFFD] = 0x84;
	// end - inline a little program

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem);

	// then:
	EXPECT_EQ(cpu.*RegisterToTest, 0x84);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_TRUE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);  
}

TEST_F(My6502Test1, LDAImmediateCanLoadAValueIntoTheARegister) {
  using namespace my6502;
  TestLoadRegisterImmediate(CPU::INS_LDA_IMMEDIATE, &CPU::accumulator);
}

TEST_F(My6502Test1, LDXImmediateCanLoadAValueIntoTheXRegister) {
  using namespace my6502;
  TestLoadRegisterImmediate(CPU::INS_LDX_IMMEDIATE, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDYImmediateCanLoadAValueIntoTheYRegister) {
  using namespace my6502;
  TestLoadRegisterImmediate(CPU::INS_LDY_IMMEDIATE, &CPU::indexRegY);
}

TEST_F(My6502Test1, LDAImmediateCanAffectTheZeroFlag) {
  // given:
  using namespace my6502;  
	cpu.accumulator = 0x44;
  mem[0XFFFC] = CPU::INS_LDA_IMMEDIATE;
	mem[0XFFFD] = 0x0;

	// when:
	CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(2, mem);

	// then:
	EXPECT_EQ(cpu.accumulator, 0x0);
	EXPECT_EQ(CyclesUsed, 2);
	EXPECT_TRUE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502Test1::TestLoadRegisterZeroPage(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
	// given:
  // start - inline a little program
  using namespace my6502;        
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
  VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister) {
  using namespace my6502;
  TestLoadRegisterZeroPage(CPU::INS_LDA_ZEROPAGE, &CPU::accumulator);
}

TEST_F(My6502Test1, LDXZeroPageCanLoadAValueIntoTheXRegister) {
  using namespace my6502;
  TestLoadRegisterZeroPage(CPU::INS_LDX_ZEROPAGE, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDYZeroPageCanLoadAValueIntoTheYRegister) {
  using namespace my6502;
  TestLoadRegisterZeroPage(CPU::INS_LDY_ZEROPAGE, &CPU::indexRegY);
}

void My6502Test1::TestLoadRegisterZeroPageX(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  cpu.indexRegX = 5;
  // start - inline a little program
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502Test1::TestLoadRegisterZeroPageY(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  cpu.indexRegY = 5;
  // start - inline a little program
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister) {
  using namespace my6502;
  TestLoadRegisterZeroPageX(CPU::INS_LDA_ZEROPX, &CPU::accumulator);
}

// TODO: function might not be applicable
TEST_F(My6502Test1, LDXZeroPageYCanLoadAValueIntoTheXRegister) {
  using namespace my6502;
  TestLoadRegisterZeroPageY(CPU::INS_LDX_ZEROPY, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDYZeroPageXCanLoadAValueIntoTheYRegister) {
  using namespace my6502;
  TestLoadRegisterZeroPageX(CPU::INS_LDY_ZEROPX, &CPU::indexRegY);
}

TEST_F(My6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);        
}
void My6502Test1::TestLoadRegisterAbsolute(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);        
}

TEST_F(My6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::accumulator);
}

TEST_F(My6502Test1, LDXAbsoluteCanLoadAValueIntoTheXRegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDYAbsoluteCanLoadAValueIntoTheYRegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::indexRegY);
}

void My6502Test1::TestLoadRegisterAbsoluteX(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void My6502Test1::TestLoadRegisterAbsoluteY(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAAbsoluteXLoadAValueIntoTheARegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteX(CPU::INS_LDA_ABSX, &CPU::accumulator);
}

TEST_F(My6502Test1, LDXAbsoluteYLoadAValueIntoTheXRegister)
{
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteY(CPU::INS_LDX_ABSY, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDYAbsoluteXLoadAValueIntoTheYRegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteX(CPU::INS_LDY_ABSX, &CPU::indexRegY);
}

void My6502Test1::TestLoadRegisterAbsoluteXWhenCrossingPage(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAAbsoluteXLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteXWhenCrossingPage(CPU::INS_LDA_ABSX, &CPU::accumulator);
}

TEST_F(My6502Test1, LDYAbsoluteXLoadAValueIntoTheYRegisterWhenItCrossesAPageBoundary) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteXWhenCrossingPage(CPU::INS_LDY_ABSX, &CPU::indexRegY);
}

TEST_F(My6502Test1, LDAAbsoluteYLoadAValueIntoTheARegister) {
  // given:
  using namespace my6502;
  TestLoadRegisterAbsoluteY(CPU::INS_LDA_ABSY, &CPU::accumulator);
}

void My6502Test1::TestLoadRegisterAbsoluteYWhenCrossingPage(my6502::Byte OpcodeToTest, my6502::Byte my6502::CPU::*RegisterToTest) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAAbsoluteYLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary) {
  using namespace my6502;
  TestLoadRegisterAbsoluteYWhenCrossingPage(CPU::INS_LDA_ABSY, &CPU::accumulator);
}

TEST_F(My6502Test1, LDXAbsoluteYLoadAValueIntoTheXRegisterWhenItCrossesAPageBoundary) {
  using namespace my6502;
  TestLoadRegisterAbsoluteYWhenCrossingPage(CPU::INS_LDX_ABSY, &CPU::indexRegX);
}

TEST_F(My6502Test1, LDAIndirectXCanLoadAValueIntoTheARegister) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAIndirectYCanLoadAValueIntoTheARegister) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(My6502Test1, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPage) {
  // given:
  using namespace my6502;  
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
	EXPECT_FALSE(cpu.zeroFlag);
	EXPECT_FALSE(cpu.negativeFlag);
	VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);        
}
