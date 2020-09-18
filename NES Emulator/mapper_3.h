#pragma once
#include "mapper_base.h"


//https://wiki.nesdev.com/w/index.php/INES_Mapper_003

class mapper_3 : public mapper_base
{
private:
	virtual const bool initialize_mapper()
	{

		const unsigned int rom_start = GET_PRG_ROM_INDEX(0);

		//£adowanie PRG-ROM do RAMu
		switch (PRG_ROM_banks)
		{
		case 1://1 BANK PRG = mirroring
			for (int i = 0; i < 0x4000; i++)
				* CPU_memory[0x8000 + i] = *CPU_memory[0xC000 + i] = ROM[rom_start + i];
			break;
		case 2://2 BANKI PRG = zajmuj¹ ca³a pamiêc na ROMy
			for (int i = 0; i < 0x8000; i++)
				*CPU_memory[0x8000 + i] = ROM[rom_start + i];
			break;
		default:
			std::cout << "\nThe file is broken!";
			return false;
		}




		// Fixed vertical or horizontal mirroring
		if (VERTICAL_MIRRORING)//Vertical mirroring: $2000 equals $2800 and $2400 equals $2C00
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::vertical);
		else if (HORIZONTAL_MIRRORING)//Horizontal mirroring: $2000 equals $2400 and $2800 equals
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::horizontal);


		return true;
	}

public:
	mapper_3(const std::string& path, CPU& cpu, PPU& ppu) : mapper_base(path, cpu, ppu) { initialize_mapper(); };
	~mapper_3() {};

	virtual void control_mapper(const unsigned char& value = 0, const unsigned short& address = 0)
	{
		const unsigned temp = GET_CHR_ROM_INDEX((value & 0x3));	//CNROM only implements the lowest 2 bits
		for (int i = 0; i < 0x2000; i++)
			* PPU_memory[i] = ROM[temp + i];
	}

};