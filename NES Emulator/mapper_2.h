#pragma once
#include "mapper_base.h"

//https://wiki.nesdev.com/w/index.php/UxROM

class mapper_2 : public mapper_base
{
private:
	virtual const bool initialize_mapper()
	{
		//CPU $8000 - $BFFF: 16 KB switchable PRG ROM bank - nie inicjalizuje teraz, zrobi to program przy pierwszym wpisie do pamieci >= 0x8000
		
		//CPU $C000 - $FFFF : 16 KB PRG ROM bank, fixed to the last bank
		const unsigned char last_bank_index = PRG_ROM_banks - 1;
		for (int i = 0; i < 0x4000; i++)
			* CPU_memory[0xc000 + i] = ROM[GET_PRG_ROM_INDEX(last_bank_index) + i];


		//£adowanie zawartoœci CHR-ROM do pamiêci PPU
		if (CHR_ROM_banks)
			for (int i = 0; i < 0x2000; i++)
				*PPU_memory[i] = ROM[16 + (PRG_ROM_banks * 0x4000) + i];


		// Fixed vertical or horizontal mirroring
		if (VERTICAL_MIRRORING)//Vertical mirroring: $2000 equals $2800 and $2400 equals $2C00
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::vertical);
		else if (HORIZONTAL_MIRRORING)//Horizontal mirroring: $2000 equals $2400 and $2800 equals
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::horizontal);


		return true;
	}

public:
	mapper_2(const std::string& path, CPU& cpu, PPU& ppu) : mapper_base(path, cpu, ppu) { initialize_mapper(); };
	~mapper_2() {};

	virtual void control_mapper(const unsigned char& value = 0, const unsigned short& address = 0)
	{
		const unsigned char bank_index = value & 0xf;
		for (int i = 0; i < 0x4000; i++)
			* CPU_memory[0x8000 + i] = ROM[GET_PRG_ROM_INDEX(bank_index) + i];
	}

};