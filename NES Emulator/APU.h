#pragma once


class CPU;

class APU
{
	const CPU* cpu;
	unsigned char *APU_registers[0x18];
public:
	APU() {};
	~APU() {};
	void APU::initialize_registers(CPU& cpu);

	void write_4015(const unsigned char& value);

	void write_4017(const unsigned char& value);

};