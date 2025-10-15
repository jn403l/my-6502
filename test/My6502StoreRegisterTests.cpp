#include <gtest/gtest.h>
#include <emu6502.h>

class My6502StoreRegisterTests : public testing::Test {
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

  void TestStoreRegisterZeroPage(Byte OpcodeToTest, RegPtr RegisterToTest);
  void TestStoreRegisterAbsolute(Byte OpcodeToTest, RegPtr RegisterToTest);
  void TestStoreRegisterZeroPageX(Byte OpcodeToTest, RegPtr RegisterToTest);  
};

static void VerifyUnmodifiedFlagsFromStoreRegister(const my6502::CPU &cpu, const my6502::CPU &CPUCopy) {
	EXPECT_EQ(cpu.Flag.carryFlag, CPUCopy.Flag.carryFlag);
	EXPECT_EQ(cpu.Flag.breakCommand, CPUCopy.Flag.breakCommand);
	EXPECT_EQ(cpu.Flag.decimalMode, CPUCopy.Flag.decimalMode);
	EXPECT_EQ(cpu.Flag.interruptDisable, CPUCopy.Flag.interruptDisable);
  EXPECT_EQ(cpu.Flag.overflowFlag, CPUCopy.Flag.overflowFlag);
  EXPECT_EQ(cpu.Flag.zeroFlag, CPUCopy.Flag.zeroFlag);
  EXPECT_EQ(cpu.Flag.negativeFlag, CPUCopy.Flag.negativeFlag);
}

void My6502StoreRegisterTests::TestStoreRegisterZeroPage(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given
  CPU CPUCopy = cpu;
  cpu.*RegisterToTest = 0x34;
  mem[0xFFFC] = OpcodeToTest;
  mem[0xFFFD] = 0x69;
  mem[0x0069] = 0x00;
  constexpr s32 expected_cyles = 3;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x0069], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

void My6502StoreRegisterTests::TestStoreRegisterAbsolute(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given
  CPU CPUCopy = cpu;
  cpu.*RegisterToTest = 0x34;
  mem[0xFFFC] = OpcodeToTest;
  mem[0xFFFD] = 0x00;
  mem[0xFFFE] = 0x80;  
  mem[0x8000] = 0x00;
  constexpr s32 expected_cyles = 4;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x8000], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

void My6502StoreRegisterTests::TestStoreRegisterZeroPageX(Byte OpcodeToTest, RegPtr RegisterToTest) {
  // given
  CPU CPUCopy = cpu;
  cpu.*RegisterToTest = 0x34;
  cpu.indexRegX = 0x0F;
  mem[0xFFFC] = OpcodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0x008F] = 0x00;
  constexpr s32 expected_cyles = 4;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x008F], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}


TEST_F(My6502StoreRegisterTests, STAZeroPageCanStoreTheARegisterIntoMemory) {
  TestStoreRegisterZeroPage(CPU::INS_STA_ZEROPAGE, &CPU::accumulator);
}

TEST_F(My6502StoreRegisterTests, STXZeroPageCanStoreTheXRegisterIntoMemory) {
  TestStoreRegisterZeroPage(CPU::INS_STX_ZEROPAGE, &CPU::indexRegX);
}

TEST_F(My6502StoreRegisterTests, STYZeroPageCanStoreTheYRegisterIntoMemory) {
  TestStoreRegisterZeroPage(CPU::INS_STY_ZEROPAGE, &CPU::indexRegY);
}

TEST_F(My6502StoreRegisterTests, STAAbsoluteCanStoreTheARegisterIntoMemory) {
  TestStoreRegisterAbsolute(CPU::INS_STA_ABSOLUTE, &CPU::accumulator);
}

TEST_F(My6502StoreRegisterTests, STXAbsoluteCanStoreTheXRegisterIntoMemory) {
  TestStoreRegisterAbsolute(CPU::INS_STX_ABSOLUTE, &CPU::indexRegX);
}

TEST_F(My6502StoreRegisterTests, STYAbsoluteCanStoreTheYRegisterIntoMemory) {
  TestStoreRegisterAbsolute(CPU::INS_STY_ABSOLUTE, &CPU::indexRegY);
}

TEST_F(My6502StoreRegisterTests, STAZeroPageXCanStoreTheARegisterIntoMemory) {
  TestStoreRegisterZeroPageX(CPU::INS_STA_ZEROPAGEX, &CPU::accumulator);
}

TEST_F(My6502StoreRegisterTests, STYZeroPageXCanStoreTheYRegisterIntoMemory) {
  TestStoreRegisterZeroPageX(CPU::INS_STY_ZEROPAGEX, &CPU::indexRegY);
}

TEST_F(My6502StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory) {
  // given
  CPU CPUCopy = cpu;
  cpu.accumulator = 0x34;
  cpu.indexRegX = 0x0F;
  mem[0xFFFC] = CPU::INS_STA_ABSOLUTEX;
  mem[0xFFFD] = 0x00;
  mem[0xFFFE] = 0x80;  
  constexpr s32 expected_cyles = 5;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x800F], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(My6502StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory) {
  // given
  CPU CPUCopy = cpu;
  cpu.accumulator = 0x34;
  cpu.indexRegY = 0x0F;
  mem[0xFFFC] = CPU::INS_STA_ABSOLUTEY;
  mem[0xFFFD] = 0x00;
  mem[0xFFFE] = 0x80;  
  constexpr s32 expected_cyles = 5;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x800F], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(My6502StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory) {
  // given
  CPU CPUCopy = cpu;
  cpu.accumulator = 0x34;
  cpu.indexRegX = 0x0F;
  mem[0xFFFC] = CPU::INS_STA_INDIRECTX;
  mem[0xFFFD] = 0x20;
  mem[0x002F] = 0x00;
  mem[0x0030] = 0x80;
  mem[0x8000] = 0x00;
  constexpr s32 expected_cyles = 6;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x8000], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(My6502StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory) {
  // given
  CPU CPUCopy = cpu;
  cpu.accumulator = 0x34;
  cpu.indexRegY = 0x0F;
  mem[0xFFFC] = CPU::INS_STA_INDIRECTY;
  mem[0xFFFD] = 0x20;
  mem[0x0020] = 0x00;
  mem[0x0021] = 0x80;
  mem[0x8000 + 0x0F] = 0x00;
  constexpr s32 expected_cyles = 6;

  // when:
  const s32 CyclesUsed = cpu.Execute(expected_cyles, mem);

  // then:
  EXPECT_EQ(CyclesUsed, expected_cyles);
  EXPECT_EQ(mem[0x8000 + 0x0F], 0x34);
  VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}
