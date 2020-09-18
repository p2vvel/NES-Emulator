#pragma once
#include "mapper_base.h"

//https://wiki.nesdev.com/w/index.php/NROM

class mapper_0 : public mapper_base
{
private:
	virtual const bool initialize_mapper()
	{
		//£adowanie PRG-ROM do RAMu
		switch (PRG_ROM_banks)
		{
		case 1://1 BANK PRG = mirroring
			for (int i = 0; i < 0x4000; i++)
				*CPU_memory[0x8000 + i] = *CPU_memory[0xC000 + i] = ROM[16 + i];
			break;
		case 2://2 BANKI PRG = zajmuj¹ ca³a pamiêc na ROMy
			for (int i = 0; i < 0x8000; i++)
				*CPU_memory[0x8000 + i] = ROM[16 + i];
			break;
		default:
			std::cout << "\nThe file is broken!";
			return false;
		}


		//£adowanie zawartoœci CHR-ROM do pamiêci PPU, jeœli CHR-ROM wystêpuje w pliku
		if (CHR_ROM_banks)
			for (int i = 0; i < 0x2000; i++)
			{
				*PPU_memory[i] = ROM[16 + (PRG_ROM_banks * 0x4000) + i];
			}



		//Fixed vertical or horizontal mirroring
		if (VERTICAL_MIRRORING)//Vertical mirroring: $2000 equals $2800 and $2400 equals $2C00
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::vertical);
		else if (HORIZONTAL_MIRRORING)//Horizontal mirroring: $2000 equals $2400 and $2800 equals
			ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::horizontal);
		

		return true;
	}

public:
	mapper_0(const std::string &path,  CPU &cpu,  PPU &ppu) : mapper_base(path, cpu, ppu) { initialize_mapper(); };
	~mapper_0() {};

	inline virtual void control_mapper(const unsigned char& value = 0, const unsigned short& address = 0) { return;  }

};