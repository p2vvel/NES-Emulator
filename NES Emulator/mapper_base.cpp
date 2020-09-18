#include "CPU.h"
#include "PPU.h"
#include "mapper_base.h"
#include "mapper_0.h"

mapper_base::mapper_base(const std::string &path,  CPU &cpu,  PPU &ppu)
{
	this->initialize_CPU_memory(cpu);
	this->initialize_PPU_memory(ppu);
	load_ROM(path);

	cpu_ptr = &cpu;
	ppu_ptr = &ppu;
}

mapper_base::~mapper_base()
{
	delete[] ROM;
}

void mapper_base::initialize_CPU_memory(const CPU &cpu)
{
	//unsigned char *CPU_memory[0xFFFF]; - deklaracja
	for(int i = 0; i < 0x10000; i++)
		CPU_memory[i] = cpu.memory[i];
}

void mapper_base::initialize_PPU_memory(const PPU &ppu)
{
	//unsigned char *PPU_memory[0x4000]; - deklaracja
	for (int i = 0; i < 0x4000; i++)
		PPU_memory[i] = ppu.memory[i];
}


/*
Starting 
Byte		Length(Bytes)	Contents
0			 3				 Should contain the string ‘NES’ to identify the file as an iNES file.
3			 1				 Should contain the value $1A, also used to identify file format.
4			 1				 Number of 16 KB PRG - ROM banks.The PRG - ROM (Program ROM) is the area of ROM used to store the program code.
5			 1				 Number of 8 KB CHR - ROM / VROM banks.The names CHR - ROM(Character ROM) and VROM are used synonymously to refer to the area of ROM used to store graphics information, the pattern tables.
6			 1				 ROM Control Byte 1:
								• Bit 0 - Indicates the type of mirroring used by the game
								where 0 indicates horizontal mirroring, 1 indicates
								vertical mirroring.
								• Bit 1 - Indicates the presence of battery - backed RAM at
								memory locations $6000 - $7FFF.
								• Bit 2 - Indicates the presence of a 512 - byte trainer at
								memory locations $7000 - $71FF.
								• Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
								mirroring should be used.
								• Bits 4 - 7 - Four lower bits of the mapper number.
7			 1				 ROM Control Byte 2:
								• Bits 0 - 3 - Reserved for future usage and should all be 0.
								• Bits 4 - 7 - Four upper bits of the mapper number.
8			 1				 Number of 8 KB RAM banks.For compatibility with previous versions of the iNES format, assume 1 page of RAM when this is 0.
9			 7				 Reserved for future usage and should all be 0.
*/
const short mapper_base::verify_header(const std::string &path)
{
	FILE *file;
	errno_t err = fopen_s(&file, path.c_str(), "rb");

	if (err != 0)
	{
		std::cout << "\nFailed to verify header!";
		return -1;
	}

	fseek(file, 0, 2);
	unsigned int size = ftell(file);
	rewind(file);

	if (size < 0xF)//Jeœli plik jest mniejszy od wielkoœci nag³ówka, to znaczy, ¿e nie bêdzie mo¿na go odczytaæ - zwracamy b³¹d
	{
		std::cout << "\nThe file is too small";
		return -1;
	}
	/*else if (size > 0x8000)
	{
		std::cout << "\nNot enough memory to load ROM!";
		return -1;
	}//*/

	unsigned char *header = new unsigned char[0xF];

	fread(header, sizeof header[0], 0xF, file);

	//Byte		Length(Bytes)	Contents
	//0			 3				 Should contain the string ‘NES’ to identify the file as an iNES file.
	//3			 1				 Should contain the value $1A, also used to identify file format.
	if (((char)header[0] != 'N' && (char &)header[1] != 'E' && (char &)header[2] != 'S') && header[3] != 0x1A)
	{
		std::cout << "\nThe file is not a proper NES ROM!";
		return -1;
	}


	//Byte		Length(Bytes)	Contents
	//6			 1				 ROM Control Byte 1:
	//							 • Bits 4 - 7 - Four lower bits of the mapper number.
	//7			 1				 ROM Control Byte 2:
	//							 • Bits 4 - 7 - Four upper bits of the mapper number.
	short mapper_number = (header[0x7] & 0xF0) | ((header[0x6] & 0xF0) >> 4);

	delete[] header;

	return mapper_number;
}

const bool mapper_base::load_ROM(const std::string &path)
{
	FILE *file;
	errno_t err = fopen_s(&file, path.c_str(), "rb");

	if(err != 0)
	{
		std::cout << "\nFailed to load ROM!";
		return false;
	}

	fseek(file, 0, 2);
	ROM_size = ftell(file);
	rewind(file);

	ROM = new unsigned char[ROM_size];

	fread(ROM, sizeof ROM[0], ROM_size, file);
	fclose(file);

	std::cout << "Succesfully loaded ROM\nPath: " << path << "\nSize: " << ROM_size << "\nMapper: " 
		<< ((ROM[0x7] & 0xF0) | ((ROM[0x6] & 0xF0) >> 4))<< "\nPRG-ROM banks: " << (short)PRG_ROM_banks << "\nCHR-ROM banks: " << (short)CHR_ROM_banks
		<< "\nMirroring: " << (HORIZONTAL_MIRRORING ?  "horizontal" : "vertical")<< "\nBattery-backed RAM: " << (BATTERY_BACKED_RAM ? "YES" : "NO") 
		<< "\nTrainer: " << (TRAINER ? "YES" : "NO") << "\nFour screen VRAM: " << (FOUR_SCREEN_VRAM ? "YES" : "NO");//*/
	
	return true;
}

