#include <main_6502.h>

int main() {
  my6502::Mem mem;
  my6502::CPU cpu;
  cpu.Reset(mem);
  // start - inline a little program
  mem[0xFFFC] = my6502::CPU::INS_JSR;
  mem[0xFFFD] = 0x42;
  mem[0xFFFE] = 0x42;
  mem[0x4242] = my6502::CPU::INS_LDA_IMMEDIATE;
	mem[0x4243] = 0x84;
	// end - inline a little program
  cpu.Execute(8, mem); //immediate(2) + jump(6)
	return 0;
}
