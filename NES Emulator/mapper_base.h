#pragma once
#include<fstream>
#include<string>
#include<iostream>

//Number of 16 KB PRG-ROM banks. The PRG-ROM(Program ROM) is the area of ROM used to store the program code
#define PRG_ROM_banks ROM[4]
//Number of 8 KB CHR-ROM / VROM banks. The names CHR - ROM(Character ROM) and VROM are used synonymously
//to refer to the area of ROM used to store graphics information, the pattern tables.
#define CHR_ROM_banks ROM[5]
//Bit 0 - Indicates the type of mirroring used by the game
//where 0 indicates horizontal mirroring, 1 indicates
//vertical mirroring.
#define VERTICAL_MIRRORING (bool )(ROM[6] & 0x01)
#define HORIZONTAL_MIRRORING (bool )((ROM[6] ^ 0x01))
//Bit 1 - Indicates the presence of battery - backed RAM at
//memory locations $6000 - $7FFF
#define BATTERY_BACKED_RAM (bool )(ROM[6] & 0x02)
//Bit 2 - Indicates the presence of a 512-byte trainer at
//memory locations $7000 - $71FF
#define TRAINER (bool )(ROM[6] & 0x04)
//Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM
#define FOUR_SCREEN_VRAM (bool )(ROM[6] & 0x08)
//Number of 8 KB RAM banks. For compatibility with previous
//versions of the iNES format, assume 1 page of RAM when
//this is 0.
//Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
#define RAM_BANKS ROM[8]

class CPU;
class PPU;
class mapper_0;

class mapper_base
{
protected:
	unsigned char *CPU_memory[0x10000];
	unsigned char *PPU_memory[0x4000];
	unsigned char *ROM;
	unsigned long long ROM_size;
	 
	 PPU	*ppu_ptr;
	 CPU	*cpu_ptr;

	const bool load_ROM(const std::string &path);

public:
	mapper_base(const std::string &path,  CPU &cpu,  PPU &ppu);
	~mapper_base();
	virtual const bool control_mapper() = 0;
	virtual const bool initialize_mapper() = 0;
	
	static const short verify_header(const std::string &path);	//Sprawdza poprawnoœc pliku na podstawie pierwszych bajtów nag³ówka, a jeœli jest on poprawny zwraca numer mappera. W przeciwnym wypadku zwraca -1
	void initialize_CPU_memory(const CPU &cpu);
	void initialize_PPU_memory(const PPU &ppu);
};

