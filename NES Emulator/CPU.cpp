#include "CPU.h"
#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<sstream>

#define MEMORY_OUTPUT_TEST


CPU::CPU()
{

	for (int i = 0; i < 0xFFFF; i++)
		memory[i] = nullptr;
	initialize_function_pointers();
	initializeMemory();
	initialize_CPU();





	start_NMI = false;


	//PC = ((*memory[0xFFFD] << 8) | *memory[0xFFFC]);
#ifdef DEBUG_CPU
	prepare_to_debug();
	emulator_debug_cycles = 0;
#endif // DEBUG_CPU

	NMI_delay = 0;
	first_controller = second_controller = nullptr;
}


CPU::~CPU() {}

void CPU::initialize_CPU()
{
	total_cycles = 0;
	for (int i = 0; i < 0xFFFF; i++)
		*memory[i] = 0;

	opcode = 0;
	
	
	memory_power_up_state();
}


void CPU::initializeMemory()			//Przypisuje wszystkie dane stanowiące pamięć do jednej, dużej tablicy wskaźników. Przypisuje wskaźniki stosu.
{

	//CPU MEMORY MAP
	//Adress	Size	Description
	//---------------------------------
	//$0000 	$800 	2KB of work RAM
	//$0800 	$800 	*Mirror of $000-$7FF
	//$1000 	$800 	*Mirror of $000-$7FF
	//$1800 	$800 	*Mirror of $000-$7FF
	//$2000 	8 		I/O  Registers (Mostly PPU)
	//$2008 	$1FF8 	*Mirror of $2000-$2007
	//$4000 	$20 	Registers (Mostly APU)
	//$4020 	$1FDF 	Cartridge Expansion ROM
	//$6000 	$2000 	SRAM
	//$8000 	$4000 	PRG-ROM lower
	//$C000 	$4000 	PRG-ROM upper

	for (int i = 0; i < 0x800; i++)
		memory[0x000 + i] = memory[0x800 + i] = memory[0x1000 + i] = memory[0x1800 + i] = &mem.RAM[i];
	for (int i = 0; i < 0x2000; i++)
		memory[0x2000 + i] = &mem.IO_registers_PPU[i % 8];
	for (int i = 0; i < 0x20; i++)
		memory[0x4000 + i] = &mem.IO_registers_APU[i];
	for (int i = 0; i < (0x6000 - 0x4020); i++)
		memory[0x4020 + i] = &mem.expansion_ROM[i];
	for (int i = 0; i < 0x2000; i++)
		memory[0x6000 + i] = &mem.SRAM[i];
	for (int i = 0; i < 0x4000; i++)
	{
		memory[0x8000 + i] = &mem.PRG_ROM_lower[i];
		memory[0xC000 + i] = &mem.PRG_ROM_upper[i];
	}

	for (int i = 0; i < 256; i++)
		stack[i] = &mem.RAM[0x100 + i];
}

void CPU::emulate_cycle()
{



	if(start_NMI)
	{
		if (NMI_delay == 0)
		{
			start_NMI = false;
			run_NMI();
		}
		else
			NMI_delay--;
	}

	if (start_IRQ)
	{
		if (IRQ_delay == 0)
		{
			start_IRQ = false;
			BRK();
		}
		else
			IRQ_delay--;
	}
	


	fetch_opcode();

#ifdef DEBUG_CPU
	debug_CPU();
	if (!(emulator_debug_cycles % 100000))
		std::cout << "\nCykl: " << emulator_debug_cycles<<", "<<(unsigned short)*memory[0x6000];//*/
	if (*memory[0x6000] != 0 && !(emulator_debug_cycles % 100000))
	{
		std::cout << "\n";
		for (int i = 0x6004;; i++)
		{
			std::cout << *memory[i];
			if (*memory[i] == 0)
				break;
		}
		std::cout << "\n";
	}

	emulator_debug_cycles++;
#endif // DEBUG_CPU

#ifdef MEMORY_OUTPUT_TEST

	static unsigned short length = 1;
	std::string temp = "";

	if (/**memory[0x6000] != 0 &&*/ !(emulator_debug_cycles % 100000))
	{
		//std::cout << std::endl <<std::hex<< (unsigned short&) *memory[0x6000] << std::endl;
		//std::cout << "\n";
		for (int i = 0x6004;; i++)
		{
			temp += *memory[i];;
			if (*memory[i] == 0)
				break;//*/
		}
		if (temp.length() != length)
		{
			length = temp.length();
			std::cout << "\n#####\n" << temp << "\n";
		}
		
	}
	emulator_debug_cycles++;
#endif
	
	instructions[opcode]();
	total_cycles += cycles;
	if (opcode != 0x00 && opcode != 0x4C && opcode != 0x6C && opcode != 0x20 && opcode != 0x60 && opcode != 0x40)
		PC += instruction_sizes[opcode];
}

void CPU::run_NMI()
{
	//Odkłada Program Counter na Stos:
	PC += 1;
	const unsigned short value = PC - 1;
	*stack[SP] = ((value & 0xFF00) >> 8);
	--SP;
	*stack[SP] = (value & 0x00FF);
	--SP;

	//Ustawia PC w taki sposób, aby wykonał się kod z pętli NMI, adres ten znajduje się w $FFFA–$FFFB
	PC = (*memory[0xFFFB] << 8) | *memory[0xFFFA];
	


	//Odkłada status procesora na stos:
	const unsigned char status = (flag.N << 7) + (flag.V << 6) + (1 << 5) + (0 << 4) + (flag.D << 3) + (flag.I << 2) + (flag.Z << 1) + (flag.C * 1);//Tworzy status - w emulatorze flagi są przechowywane nie w postaci jednego bajtu, a jako wartości bool
	*stack[SP] = status;			//One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. 
	--SP;							//In fact, that's the only way a program can tell a BRK from /IRQ.
								//Bit 4(w statusie": Clear if interrupt vectoring, set if BRK or PHP
	
	
	cycles = 7;
	total_cycles += cycles;
}

#pragma region ADDRESSING_MODES

const unsigned char CPU::get_adress_ZeroPage()
{
	const unsigned char result = *memory[PC + 1];
	return result;
}

const unsigned char CPU::get_adress_IndexedZeroPage_X()
{
	const unsigned char result = (*memory[PC + 1] + X);
	return result;
}

const unsigned char CPU::get_adress_IndexedZeroPage_Y()
{
	const unsigned char result = (*memory[PC + 1] + Y);
	return result;
}

const unsigned short CPU::get_adress_Absolute()
{
	const unsigned short result = ((*memory[PC + 2] << 8) | *memory[PC + 1]);
	return result;
}

const unsigned short CPU::get_adress_IndexedAbsolute_X()
{
	const unsigned short result = (((*memory[PC + 2] << 8) | *memory[PC + 1]) + X);
	return result;
}

const unsigned short CPU::get_adress_IndexedAbsolute_Y()
{
	const unsigned short result = (((*memory[PC + 2] << 8) | *memory[PC + 1]) + Y);
	return result;
}

const unsigned short CPU::get_adress_Indirect()
{
	//An original 6502 has does not correctly fetch the target address if the indirect vector falls on a page boundary (e.g. $xxFF where xx is any value from $00 to $FF). 
	//In this case fetches the LSB(Least Significant Bits) from $xxFF as expected but takes the MSB(Most Significant Bits) from $xx00.
	const unsigned short mem_loc = ((*memory[PC + 2] << 8) | *memory[PC + 1]);
	unsigned short result;
	if ((mem_loc & 0x00FF) != 0xFF)
		result = ((*memory[mem_loc + 1] << 8) | *memory[mem_loc]);
	else
		result = ((*memory[mem_loc & 0xFF00] << 8) | *memory[mem_loc]);
	return result;
}

const unsigned short CPU::get_adress_Immediate()
{
	const unsigned short result = (PC + 1);
	return result;
}

const unsigned short CPU::get_adress_Relative()
{
	const unsigned short result = (PC + 1);
	return result;
}

//Rejestr Y. Znany też jako Post-Indexed
const unsigned short CPU::get_adress_Indexed_Indirect_Y()
{
	const unsigned char temp_mem_loc = *memory[PC + 1];
	const unsigned short mem_loc = ((*memory[(temp_mem_loc + 1) & 0xFF] << 8) | *memory[temp_mem_loc]);
	const unsigned short result = (mem_loc + Y);//*/
	return result;
}

//Rejestr X. Znany też jako Pre-Indexed
const unsigned short CPU::get_adress_Indirect_Indexed_X()
{
	const unsigned char mem_loc = (*memory[PC + 1] + X);
	const unsigned short result = ((*memory[(mem_loc + 1) & 0x00FF] << 8) | *memory[mem_loc]);//W tym trybie adresowania właściwy adres nie może być pobrany spoza Zero Page. Jeśli mem_loc == 0xFF, to (mem_loc + 1) == 0x00.
	return result;
}

const bool CPU::checkPageCrossed(const unsigned short &adress, const unsigned short &old_adress) const
{
	return (((adress) & 0xFF00) != (old_adress & 0xFF00));
}

#pragma endregion


//Ustawia odpowiedni stan pamięci odpowiadający temu, jaki NES ma po włączeniu
//						
//							!!!Po zaimplementowaniu APU będzie trzeba to dokończyć!!!
//All 15 bits of noise channel LFSR = $0000.The first time the LFSR is clocked from the all - 0s state, it will shift in a 1.
void CPU::memory_power_up_state()
{
	//P = $34(IRQ disabled) <-- To są flagi. Informacje z NESdev wiki, ale:
	//Nintendulator ma jednak ustawione P na $24 - wtedy logi są zgodne. W celu uniknięcia błędów zostawiam tak jak jest w Nintendulatorze
	//Na początku flagi mają wartość 0x24;
	//       NV-B DIZC
	//0x24 = 0010 0100
	flag.C = false;
	flag.Z = false;
	flag.I = true;
	flag.D = false;
	flag.B = false;
	flag.V = false;
	flag.N = false;

	//A, X, Y = 0
	A =  X =  Y = 0;

	// S = $FD	<--To jest Stack Pointer
	SP = 0xFD;

	//$4017 = $00(frame irq enabled)
	*memory[0x4017] = 0x00;

	//$4015 = $00(all channels disabled)
	*memory[0x4015] = 0x00;

	//$4000 - $400F = $00(not sure about $4010 - $4013)
	for (int i = 0x4000; i <= 0x400F; i++)
		*memory[i] = 0x00;

	//Zawartość RAMU ustawiam na 0x00 na każdym bajcie
	//Internal memory($0000 - $07FF) has unreliable startup state.Some machines may have consistent RAM contents at power - on, but others do not.
	//Emulators often implement a consistent RAM startup state(e.g.all $00 or $FF, or a particular pattern)
	for (int i = 0; i <= 0x07FF; i++)
		*memory[i] = 0x00;
}

//Ustawia stany pamięci na takie, które odpowiadają tym w rzeczywistym NESie po restarcie
void CPU::memory_reset_state()
{
	//A, X, Y were not affected

	//S was decremented by 3 (but nothing was written to the stack)
	//S to Stack Pointer (u mnie SP)
	SP -= 3;

	//The I(IRQ disable) flag was set to true (status ORed with $04)
	flag.I = true;
	
	//The internal memory was unchanged
	
	//APU mode in $4017 was unchanged
	
	//APU was silenced($4015 = 0)
		*memory[0x4015] = 0;
}

void CPU::prepare_to_debug()
{
	//Normalny stan Program Countera to:
	//PC = ((*memory[0xFFFD] << 8) | *memory[0xFFFC]);
	//Nestest bez zaimplementowania PPU wymusza rozpoczęcie wykonywania programu z tej pozycji:
	//PC = 0xC000;

	std::ofstream clear_fout("../logs/cpu_debug.txt");		//Czyści logi z poprzedniego debugowania
	clear_fout.close();
}


void CPU::memory_write(const unsigned short &address, const unsigned char &value)
{
	//Mirrory pamięci w CPU!(0x2000 do 0x2007 sa az do 0x3fff wlacznie!)!!!
	//http://forums.nesdev.com/viewtopic.php?t=7647
	if (address >= 0x2000 && address <= 0x3fff)
	{
		unsigned short temp_address = 0x2000 + (address % 8);

		unsigned long long my_cycles = (total_cycles + instruction_cycles[opcode]) * 3;
		while (my_cycles > ppu->get_total_cycles())	//Nadgania cykle, żeby wyeliminować desynchronizację pomiędzy CPU i PPU, która powodowałaby nieprawidłowe wartości w rejestrach
			ppu->update();

		ppu->update_2002(value);

		if (temp_address == 0x2000)	//After power/reset, writes to this register are ignored for about 30000 cycles. 
			ppu->write_2000(value);
		else if (temp_address == 0x2004)
			ppu->write_2004(value);
		else if (temp_address == 0x2005)
			ppu->write_2005(value);
		else if (temp_address == 0x2006)
			ppu->write_2006(value);
		else if (temp_address == 0x2007)
			ppu->write_2007(value);
		else
			*memory[address] = value;
	}
	else if (address == 0x4014)	//Obsługa DMA w PPU
	{
		*memory[address] = value;
		ppu->activate_DMA();
		total_cycles += (total_cycles & 0x01 ? 514 : 513);	//+1 cycle if on an odd CPU cycle
	}
	else if (address == 0x4016)
	{
		*memory[address] = value;
		first_controller->control_polling(value & 0x01);
	}
	else if (address >= 0x8000)
		return;
	else
		*memory[address] = value;
}


unsigned char CPU::memory_read(const unsigned short &address)
{
	unsigned char result;// = 0;
	if ((address >= 0x2000 && address <= 0x3fff) || address == 0x4014)
	{						//wartość zwiększona o cykle potrzebne do wykonania tego opcodea

		unsigned long long my_cycles = (total_cycles + instruction_cycles[opcode]) * 3;
		while (my_cycles > ppu->get_total_cycles())	//Nadgania cykle, żeby wyeliminować desynchronizację pomiędzy CPU i PPU, która powodowałaby nieprawidłowe wartości w rejestrach
			ppu->update();


		unsigned short temp_address = 0x2000 + (address % 8);


		if (temp_address == 0x2002)
			result = ppu->read_2002();
		else if (temp_address == 0x2004)
			result = ppu->read_2004();
		else if (temp_address == 0x2007)
			result = ppu->read_2007();
		else
			result = *memory[address];
	}
	else if (address == 0x4016)
	{
		result = (first_controller->read_controller_latch() ? 0x01 : 0x00);
	}
	else
		result = *memory[address];

	return result;
}


const bool CPU::is_opcode_illegal()
{
	for (auto i : illegal_opcodes)
		if (i == opcode)
			return true;
	return false;
}


const unsigned char CPU::instructions_size()
{
	//0. Zero Page
	//1. Indexed Zero Page X
	//2. Indexed Zero Page Y
	//3. Absolute
	//4. Indexed Absolute X
	//5. Indexed Absolute Y
	//6. Indirect
	//7. Implied
	//8. Accumulator
	//9. Immediate
	//10. Relative
	//11. Indexed Indirect X
	//12. Indirect Indexed Y
	//13. Magic Constant
	switch (adressing_modes[opcode])
	{
	case 0:
		return 2;
	case 1:
		return 2;
	case 2:
		return 2;
	case 3:
		return 3;
	case 4:
		return 3;
	case 5:
		return 3;
	case 6:
		return 3;
	case 7:
		return 1;
	case 8:
		return 1;
	case 9:
		return 2;
	case 10:
		return 2;
	case 11:
		return 2;
	case 12:
		return 2;
	case 13:
		return 2;
	default:
		return -1;	//Pomoże wyłapać błędy
	}
}

void CPU::save_memory_state(unsigned short line_limiter)
{
	std::ofstream fout = std::ofstream("../logs/memory_state.txt", std::ios_base::out);

	for (unsigned int i = 0; i < 0x10000; i++)
	{
		if (i % (line_limiter + 1) == 0)
			fout << "\n"<<std::hex<<(i & ~0xf)<<"\t";
		fout <<(unsigned short) *memory[i]<<" ";
		
	}

	fout.close();
}

void CPU::debug_CPU()
{
	// NESTEST - schemat wyników na wyjściu:
	//
	// *-instruction address(PC register)
	// |
	// |     *-opcode + args
	// |     |
	// |     |         *-mneumonic + decoded args
	// |     |         |
	// |     |         |                               *-pre - execution registers
	// |     |         |                               |
	// C000  4C F5 C5  JMP $C5F5                       A : 00 X : 00 Y : 00 P : 24 SP : FD





	static std::string temp;
	static unsigned char counter;



	counter++;



	if (!(counter % 1000))
	{
		std::ofstream file("../logs/cpu_debug.txt", std::ios_base::app);
		file << temp;
		temp = "";
		file.close();

	}

	std::stringstream fout;


	fout << std::hex << std::uppercase;
	fout.width(4);
	fout.fill('0');
	fout << (const unsigned int&)PC;


	if (instructions_size() == 1)
	{
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)opcode;
		fout << "       ";
	}
	else if (instructions_size() == 2)
	{
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)opcode;
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << "    ";
	}
	else if (instructions_size() == 3)
	{
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)opcode;
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << " ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 2];
		fout << " ";
	}

	//fout<< "*";
	//return;

	switch (adressing_modes[opcode])
	{
	case 0://Zero Page
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "ZP";
		fout << mneumonic[opcode];
		fout << " $";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_ZeroPage();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_ZeroPage()];
		fout << "                    ";
	}
	break;
	case 1://Indexed Zero Page X
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "ZX";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << ",X @ ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_IndexedZeroPage_X();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_IndexedZeroPage_X()];
		fout << "             ";
	}
	break;
	case 2://Indexed Zero Page Y
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "ZY";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << ",Y @ ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_IndexedZeroPage_Y();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_IndexedZeroPage_Y()];
		fout << "             ";
	}
	break;
	case 3://Absolute
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "AA";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Absolute();
		if (opcode != 0x4C && opcode != 0x6C && opcode != 0x20 && opcode != 0x60)
		{
			fout << " = ";
			fout.width(2);
			fout.fill('0');
			fout << (const unsigned int&)* memory[get_adress_Absolute()];
			fout << "                  ";
		}
		else
			fout << "                       ";
	}
	break;
	case 4://Indexed Absolute X
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "AX";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Absolute();
		fout << ",X @ ";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_IndexedAbsolute_X();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_IndexedAbsolute_X()];
		fout << "         ";

	}
	break;
	case 5://Indexed Absolute Y
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "AY";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Absolute();
		fout << ",Y @ ";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_IndexedAbsolute_Y();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_IndexedAbsolute_Y()];
		fout << "         ";
	}
	break;
	case 6://Indirect
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "IN";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "($";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Absolute();
		fout << ") = ";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Indirect();
		fout << "              ";
	}
	break;
	case 7://Implied**
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "IM";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "                            ";
	}
	break;
	case 8://Accumulator**
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "AC";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "A";
		fout << "                           ";
	}
	break;
	case 9://Immediate**
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "IT";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "#$";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << "                        ";
	}
	break;
	case 10://Relative**
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "RL";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "$";
		fout.width(4);
		fout.fill('0');
		fout << ((const char&)(*memory[get_adress_Relative()]) + PC + 2);
		fout << "                       ";
	}
	break;
	case 11://Indexed Indirect X
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "IX";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "($";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << ",X) @ ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)((X + *memory[PC + 1]) & 0xFF);
		fout << " = ";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Indirect_Indexed_X();
		fout << " = ";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[get_adress_Indirect_Indexed_X()];
		fout << "    ";
	}
	break;
	case 12://Indirect Indexed Y
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << " ";
		//fout << "IY";
		fout << mneumonic[opcode];
		fout << " ";
		fout << "($";
		fout.width(2);
		fout.fill('0');
		fout << (const unsigned int&)* memory[PC + 1];
		fout << "),Y = ";
		fout.width(4);
		fout.fill('0');
		fout << ((*memory[(*memory[PC + 1] + 1) & 0xFF]) << 8 | *memory[*memory[PC + 1]]);
		fout << " @ ";
		fout.width(4);
		fout.fill('0');
		fout << (const unsigned int&)get_adress_Indexed_Indirect_Y();
		fout << " = ";
		fout.width(2);
		fout.fill('0');//*/
		fout << (const unsigned int&)* memory[get_adress_Indexed_Indirect_Y()];
		fout << "  ";
	}
	break;
	case 13:
	{
		fout << (is_opcode_illegal() ? '*' : ' ');
		//fout << "Output To Do";
		fout << "     ";
	}
	break;
	}


	fout << "A:";
	fout.width(2);
	fout.fill('0');
	fout << (const unsigned int&)A;
	fout << " X:";
	fout.width(2);
	fout.fill('0');
	fout << (const unsigned int&)X;
	fout << " Y:";
	fout.width(2);
	fout.fill('0');
	fout << (const unsigned int&)Y;
	fout << " P:";
	fout.width(2);
	fout.fill('0');
	fout << (const unsigned int&)((flag.N << 7) + (flag.V << 6) + (1 << 5) + (flag.B << 4) + (flag.D << 3) + (flag.I << 2) + (flag.Z << 1) + (flag.C * 1));
	fout << " SP:";
	fout.width(2);
	fout.fill('0');
	fout << (const unsigned int&)SP << " CYC:";
	fout.width(3);
	fout.fill(' ');
	fout << std::dec << ppu->get_cycles();
	//Cykle są mnożone przez 3, ponieważ log z Nintendulator, z którym porównuję mój log pokazuje cykle z PPU. 1 cykl PPU = 3 cykle CPU.
	fout << " SL:" << ppu->get_scanline();
	fout << " Total:" << total_cycles;
	fout << " Frame:" << ppu->get_frame();
	fout << "\n";


	temp += fout.str();
}//*/