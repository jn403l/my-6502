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

TEST_F(My6502Test1, RunALittleInlineProgram) {
  // start - inline a little program
  mem[0xFFFC] = CPU::INS_JSR;
  mem[0xFFFD] = 0x42;
  mem[0xFFFE] = 0x42;
  mem[0x4242] = CPU::INS_LDA_IMMEDIATE;
	mem[0x4243] = 0x84;
	// end - inline a little program
  cpu.Execute(8, mem); //immediate(2) + jump(6)  
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
