#pragma once
#include "mapper_base.h"


//https://wiki.nesdev.com/w/index.php/MMC1

class mapper_1 : public mapper_base
{
private:
	unsigned char write_counter;
	unsigned char load_register;
	unsigned char control_register;
	unsigned char chr_1_register;
	unsigned char chr_2_register;
	unsigned char prg_register;

	virtual const bool initialize_mapper()
	{
		write_counter = load_register = control_register = chr_1_register = chr_2_register = prg_register = 0;


		for (int i = 0; i < 0x4000; i++)
		{
			*CPU_memory[0x8000 + i] = ROM[GET_PRG_ROM_INDEX((0)) + i];
			*CPU_memory[0xc000 + i] = ROM[GET_PRG_ROM_INDEX((PRG_ROM_banks - 1)) + i];
		}

		return true;
	}

public:
	mapper_1(const std::string& path, CPU& cpu, PPU& ppu) : mapper_base(path, cpu, ppu) { initialize_mapper(); };
	~mapper_1() {};

	virtual void control_mapper(const unsigned char& value = 0, const unsigned short& address = 0)
	{
		if (value & 0x80)	//ustawiony bit 7 czysci rejestr przesuwny sluzacy do ladowania danych do wewnetrznych rejestrow
		{
			load_register = 0;
			write_counter = 0;
		}
		else
		{
			load_register |= (value & 0x01) << write_counter;

			if (write_counter == 4)
			{
				switch ((address & 0x6000) >> 13)	//tylko 13 i 14 bit biora udzia³ w wyborze wewnetrznego rejestru
				{
				case 0:	//Control (internal, $8000-$9FFF)
					control_register = load_register;
					switch (control_register & 0x03)	//mirroring mode
					{
					case 0:
						ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::one_screen_low);
						break;
					case 1:
						ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::one_screen_high);
						break;
					case 2:
						ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::vertical);
						break;
					case 3:
						ppu_ptr->set_mirroring_mode(nametable_mirroring_mode::horizontal);
						break;
					}
					break;

				case 1:	//CHR bank 0 (internal, $A000-$BFFF)
					chr_1_register = load_register;
					if (control_register & 0x10)	//1: switch two separate 4 KB banks
					{
						const unsigned char chr_bank_address = GET_CHR_ROM_INDEX((chr_1_register & 0x1f));
						for (int i = 0; i < 0x1000; i++)
						{
							*PPU_memory[i] = ROM[chr_bank_address + i];
						}

					}
					else	//0: switch 8 KB at a time
					{
						const unsigned char chr_bank_address = GET_CHR_ROM_INDEX((chr_1_register & 0x1e));
						for (int i = 0; i < 0x2000; i++)
						{
							*PPU_memory[i] = ROM[chr_bank_address + i];
						}
					}
					break;

				case 2:	//CHR bank 1 (internal, $C000-$DFFF)
					chr_2_register = load_register;
					if (control_register & 0x10)	//ignored in 8 KB mode
					{
						const unsigned char chr_bank_address = GET_CHR_ROM_INDEX((chr_2_register & 0x1f));
						for (int i = 0; i < 0x1000; i++)
						{
							*PPU_memory[0x1000 + i] = ROM[chr_bank_address + i];
						}
					}
					break;

				case 3:	//PRG bank (internal, $E000-$FFFF)
					prg_register = load_register;
					switch ((control_register >> 2) & 0x03)	//prg rom bank mode
					{
					case 0: case 1:	//0, 1: switch 32 KB at $8000, ignoring low bit of bank number
					{
						const unsigned char prg_bank_address = GET_PRG_ROM_INDEX((prg_register & 0xe));
						for (int i = 0; i < 0x8000; i++)
						{
							*CPU_memory[0x8000 + i] = ROM[prg_bank_address + i];
						}
					}
					break;
					case 2:	//2: fix first bank at $8000 and switch 16 KB bank at $C000
					{
						const unsigned char prg_bank_address = GET_PRG_ROM_INDEX((prg_register & 0xf));
						const unsigned char first_prg_bank_address = GET_PRG_ROM_INDEX(0);
						for (int i = 0; i < 0x4000; i++)
						{
							*CPU_memory[0x8000 + i] = ROM[first_prg_bank_address+i];
							*CPU_memory[0xc000 + i] = ROM[prg_bank_address+i];
						}
					}
					break;
					case 3:	//3: fix last bank at $C000 and switch 16 KB bank at $8000
					{
						const unsigned char prg_bank_address = GET_PRG_ROM_INDEX((prg_register & 0xf));
						const unsigned char last_prg_bank_address = GET_PRG_ROM_INDEX((PRG_ROM_banks - 1));
						for (int i = 0; i < 0x4000; i++)
						{
							*CPU_memory[0x8000 + i] = ROM[prg_bank_address + i];
							*CPU_memory[0xc000 + i] = ROM[last_prg_bank_address + i];
						}
					}
					break;
					}
					break;
				}

				load_register = 0;
			}


			write_counter = (write_counter + 1) % 5;
		}
	}

};
