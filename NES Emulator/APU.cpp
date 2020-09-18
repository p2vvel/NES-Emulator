#include "APU.h"
#include"CPU.h"

void APU::initialize_registers(CPU& cpu)
{
	for (int i = 0; i < 0x18; i++)
		APU_registers[i] = cpu.memory[0x4000 + i];
}

void APU::write_4015(const unsigned char& value)
{
	*APU_registers[0x15] = value;
}

void APU::write_4017(const unsigned char& value)
{
	*APU_registers[0x17] = value;
}