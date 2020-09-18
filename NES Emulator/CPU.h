#pragma once

#include<functional>
#include "PPU.h"
#include "mapper_base.h"
#include "controller_base.h"
#include "APU.h"

//#define DEBUG_CPU


static const unsigned char illegal_opcodes[] =
{
0x0B, 0x2B, 0x87, 0x97, 0x83, 0x8F, 0x6B, 0x4B, 0xAB, 0x9F, 0x93, 0xCB, 0xC7, 0xD7, 0xCF, 0xDF, 0xDB, 0xC3, 0xD3,0x04, 0x14, 0x34, 0x44, 0x54, 0x64, 0x74, 0x80, 0x82, 0x89, 0xC2, 0xD4, 0xE2, 0xF4,
0xE7, 0xF7, 0xEF, 0xFF, 0xFB, 0xE3, 0xF3, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x92, 0xB2, 0xD2, 0xF2, 0xBB, 0xA7, 0xB7, 0xAF, 0xBF, 0xA3, 0xB3, 0x1A, 0x3A, 0x5A, 0x7A, 0xDA, 0xFA,
0x27, 0x37, 0x2F, 0x3F, 0x3B, 0x23, 0x33, 0x67, 0x77, 0x6F, 0x7F, 0x7B, 0x63, 0x73, 0xEB, 0x07, 0x17, 0x0F, 0x1F, 0x1B, 0x03, 0x13, 0x47, 0x57, 0x4F, 0x5F, 0x5B, 0x43, 0x53, 0x9C, 0x0C, 0x1C, 0x3C, 0x5C, 0x7C, 0xDC, 0xFC,
0x8B, 0x9B
};

static const unsigned char instruction_cycles[256] =
{
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

static const unsigned char page_crossed_instruction_cycles[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};



static const unsigned char instruction_sizes[256] =
{
1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//0
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//1
3, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//2
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//3
1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//4
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//5
1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 0, 3, 3, 3,	//6
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//7
2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//8
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//9
2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//A
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,	//B
2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//C
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//D
2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,	//E
2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,//F

};


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
static const unsigned char adressing_modes[256] =
{
	7, 11, 7, 11, 0, 0, 0, 0, 7, 9, 8, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4,
	3, 11, 7, 11, 0, 0, 0, 0, 7, 9, 8, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4,
	7, 11, 7, 11, 0, 0, 0, 0, 7, 9, 8, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4,
	7, 11, 7, 11, 0, 0, 0, 0, 7, 9, 8, 9, 6, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4,
	9, 11, 9, 11, 0, 0, 0, 0, 7, 9, 7, 13, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 2, 2, 7, 5, 7, 5, 4, 4, 5, 5,
	9, 11, 9, 11, 0, 0, 0, 0, 7, 9, 7, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 2, 2, 7, 5, 7, 5, 4, 4, 5, 5,
	9, 11, 9, 11, 0, 0, 0, 0, 7, 9, 7, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4,
	9, 11, 9, 11, 0, 0, 0, 0, 7, 9, 7, 9, 3, 3, 3, 3,
	10, 12, 7, 12, 1, 1, 1, 1, 7, 5, 7, 5, 4, 4, 4, 4
};

static const std::string mneumonic[256] =
{
	"BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
	"BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
	"JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
	"PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
	"BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
	"SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
	"RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
	"BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
	"RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
	"BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
	"NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
	"DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
	"BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
	"TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
	"LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
	"CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
	"CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
	"INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
	"BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB",
	"INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",
	"BEQ", "SBC", "KIL", "ISB", "NOP", "SBC", "INC", "ISB",
	"SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB",
};

//MOS 6502:
//Little Endian
//W emulatorze jedynymi danymi "signed" maj¹ byæ adresy podawane przy instrukcjach Branch.






struct Flags_
{
	//Kolejnoœc flag w bicie:
	//7 6 5 4 3 2 1 0
	//N V - B D I Z C

	bool C;	//Carry Flag
	bool Z;	//Zero Flag
	bool I;	//Interrupt Disable
	bool D;	//Decimal Mode
	bool B;	//Break Command
	bool V;	//Overflow Flag
	bool N;	//Negative Flag;
};


struct memory_map_CPU			//Zawiera ca³¹ pamiêæ emulatora.
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

	unsigned char RAM[0x800];
	unsigned char IO_registers_PPU[0x8];
	unsigned char IO_registers_APU[0x20];
	unsigned char expansion_ROM[0x6000 - 0x4020];
	unsigned char SRAM[0x2000];
	unsigned char PRG_ROM_lower[0x4000];
	unsigned char PRG_ROM_upper[0x4000];
};



class CPU
{
	PPU *ppu;
	
	//Udostêpnia pamiêæ rejestrów do klasy PPU
	friend void PPU::initialize_registers(CPU &cpu);
	//Udostêpnia pamiêæ do klasy bazowej mapperów
	friend void mapper_base::initialize_CPU_memory(const CPU &cpu);
	//Udostêpnia pamiêæ rejestrów do klasy APU
	friend void APU::initialize_registers(CPU& cpu);

	memory_map_CPU mem;
	unsigned char *memory[0x10000/*FFFF*/];	//WskaŸniki, w których uporz¹dkowana jest ca³a pamiêæ.

	unsigned short PC;	//Program Counter

	unsigned char *stack[256];	//WskaŸniki do stosu. Stos znajduje sie pod adresem 0x100 pamiêci.
	unsigned char SP;	//stack pointer. "Porusza siê on odwrotnie", czyli kiedy zaczynamy emulacjê ma on zazwyczaj wartoœæ 0xFF. W miarê odk³adania danych na stos, SP ulega dekrementacji(maleje). Kiedy je zabieramy, powiêksza siê on.

	unsigned char A;	//Accumulator
	unsigned char X;	//X register
	unsigned char Y;	//Y register

	unsigned char opcode;

	unsigned int total_cycles;
	unsigned char cycles;
	unsigned short old_PC;



	


	Flags_ flag;

//#ifdef DEBUG_CPU
unsigned int emulator_debug_cycles;
//#endif // DEBUG_CPU


	void fetch_opcode() { opcode = *memory[PC]; }
	void initialize_CPU();
	void initializeMemory();
	const unsigned char get_adress_ZeroPage();
	const unsigned char get_adress_IndexedZeroPage_X();
	const unsigned char get_adress_IndexedZeroPage_Y();
	const unsigned short get_adress_Absolute();
	const unsigned short get_adress_IndexedAbsolute_X();
	const unsigned short get_adress_IndexedAbsolute_Y();
	const unsigned short get_adress_Indirect();
	const unsigned short get_adress_Immediate();
	const unsigned short get_adress_Relative();
	const unsigned short get_adress_Indexed_Indirect_Y();
	const unsigned short get_adress_Indirect_Indexed_X();
	const bool checkPageCrossed(const unsigned short & adress, const unsigned short & old_adress) const;
	const bool is_opcode_illegal();
	void debug_CPU();
	
	const unsigned char instructions_size();


	void memory_write(const unsigned short & address, const unsigned char & value);

	unsigned char memory_read(const unsigned short& address);

	//void memory_read(const unsigned short & address, unsigned char & destination);
	
	

#pragma region opcodes_functions
	void ADC(const unsigned short & adress);
	void AND(const unsigned short & adress);
	void ASL(const unsigned short & adress);
	void LSR(const unsigned short & adress);
	void LSR_accumulator();
	void ASL_accumulator();
	const bool BCC(const unsigned short & adress);
	const bool BCS(const unsigned short & adress);
	const bool BEQ(const unsigned short & adress);
	void BIT(const unsigned short & adress);
	const bool BMI(const unsigned short & adress);
	const bool BNE(const unsigned short & adress);
	const bool BPL(const unsigned short & adress);
	void BRK();
	const bool BVC(const unsigned short & adress);
	const bool BVS(const unsigned short & adress);
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP(const unsigned short & adress);
	void CPX(const unsigned short & adress);
	void CPY(const unsigned short & adress);
	void DEC(const unsigned short & adress);
	void DEX();
	void DEY();
	void EOR(const unsigned short & adress);
	void INC(const unsigned short & adress);
	void INX();
	void INY();
	void JMP(const unsigned short & adress);
	void JSR(const unsigned short & adress);
	void LDA(const unsigned short & adress);
	void NOP();
	void RLA(const unsigned short & adress);
	void RRA(const unsigned short & adress);
	void SLO(const unsigned short & adress);
	void SRE(const unsigned short & adress);
	void SXA(const unsigned short & adress);
	void SYA(const unsigned short & adress);
	void TOP(const unsigned short & adress);
	void XAS(const unsigned short & adress);
	void ORA(const unsigned short & adress);
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL(const unsigned short & adress);
	void ROR(const unsigned short & adress);
	void ROR_accumulator();
	void ROL_accumulator();
	void RTI();
	void RTS();
	void SBC(const unsigned short & adress);
	void SEC();
	void SED();
	void SEI();
	void STA(const unsigned short & adress);
	void STX(const unsigned short & adress);
	void STY(const unsigned short & adress);
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();
	void AAC(const unsigned short & adress);
	void AAX(const unsigned short & adress);
	void ARR(const unsigned short & adress);
	void ASR(const unsigned short & adress);
	void ATX(const unsigned short & adress);
	void AXA(const unsigned short & adress);
	void AXS(const unsigned short & adress);
	void DCP(const unsigned short & adress);
	void DOP(const unsigned short & adress);
	void ISC(const unsigned short & adress);
	void KIL();
	void LAR(const unsigned short & adress);
	void LAX(const unsigned short & adress);
	void LDX(const unsigned short & adress);
	void LDY(const unsigned short & adress);
	void SHX(const unsigned short& adress);
	void SHY(const unsigned short& adress);
#pragma endregion

#pragma region opcode_instructions
	void opcode_00();
	void opcode_01();
	void opcode_02();
	void opcode_03();
	void opcode_04();
	void opcode_05();
	void opcode_06();
	void opcode_07();
	void opcode_08();
	void opcode_09();
	void opcode_0A();
	void opcode_0B();
	void opcode_0C();
	void opcode_0D();
	void opcode_0E();
	void opcode_0F();
	void opcode_10();
	void opcode_11();
	void opcode_12();
	void opcode_13();
	void opcode_14();
	void opcode_15();
	void opcode_16();
	void opcode_17();
	void opcode_18();
	void opcode_19();
	void opcode_1A();
	void opcode_1B();
	void opcode_1C();
	void opcode_1D();
	void opcode_1E();
	void opcode_1F();
	void opcode_20();
	void opcode_21();
	void opcode_22();
	void opcode_23();
	void opcode_24();
	void opcode_25();
	void opcode_26();
	void opcode_27();
	void opcode_28();
	void opcode_29();
	void opcode_2A();
	void opcode_2B();
	void opcode_2C();
	void opcode_2D();
	void opcode_2E();
	void opcode_2F();
	void opcode_30();
	void opcode_31();
	void opcode_32();
	void opcode_33();
	void opcode_34();
	void opcode_35();
	void opcode_36();
	void opcode_37();
	void opcode_38();
	void opcode_39();
	void opcode_3A();
	void opcode_3B();
	void opcode_3C();
	void opcode_3D();
	void opcode_3E();
	void opcode_3F();
	void opcode_40();
	void opcode_41();
	void opcode_42();
	void opcode_43();
	void opcode_44();
	void opcode_45();
	void opcode_46();
	void opcode_47();
	void opcode_48();
	void opcode_49();
	void opcode_4A();
	void opcode_4B();
	void opcode_4C();
	void opcode_4D();
	void opcode_4E();
	void opcode_4F();
	void opcode_50();
	void opcode_51();
	void opcode_52();
	void opcode_53();
	void opcode_54();
	void opcode_55();
	void opcode_56();
	void opcode_57();
	void opcode_58();
	void opcode_59();
	void opcode_5A();
	void opcode_5B();
	void opcode_5C();
	void opcode_5D();
	void opcode_5E();
	void opcode_5F();
	void opcode_60();
	void opcode_61();
	void opcode_62();
	void opcode_63();
	void opcode_64();
	void opcode_65();
	void opcode_66();
	void opcode_67();
	void opcode_68();
	void opcode_69();
	void opcode_6A();
	void opcode_6B();
	void opcode_6C();
	void opcode_6D();
	void opcode_6E();
	void opcode_6F();
	void opcode_70();
	void opcode_71();
	void opcode_72();
	void opcode_73();
	void opcode_74();
	void opcode_75();
	void opcode_76();
	void opcode_77();
	void opcode_78();
	void opcode_79();
	void opcode_7A();
	void opcode_7B();
	void opcode_7C();
	void opcode_7D();
	void opcode_7E();
	void opcode_7F();
	void opcode_80();
	void opcode_81();
	void opcode_82();
	void opcode_83();
	void opcode_84();
	void opcode_85();
	void opcode_86();
	void opcode_87();
	void opcode_88();
	void opcode_89();
	void opcode_8A();
	void opcode_8B();
	void opcode_8C();
	void opcode_8D();
	void opcode_8E();
	void opcode_8F();
	void opcode_90();
	void opcode_91();
	void opcode_92();
	void opcode_93();
	void opcode_94();
	void opcode_95();
	void opcode_96();
	void opcode_97();
	void opcode_98();
	void opcode_99();
	void opcode_9A();
	void opcode_9B();
	void opcode_9C();
	void opcode_9D();
	void opcode_9E();
	void opcode_9F();
	void opcode_A0();
	void opcode_A1();
	void opcode_A2();
	void opcode_A3();
	void opcode_A4();
	void opcode_A5();
	void opcode_A6();
	void opcode_A7();
	void opcode_A8();
	void opcode_A9();
	void opcode_AA();
	void opcode_AB();
	void opcode_AC();
	void opcode_AD();
	void opcode_AE();
	void opcode_AF();
	void opcode_B0();
	void opcode_B1();
	void opcode_B2();
	void opcode_B3();
	void opcode_B4();
	void opcode_B5();
	void opcode_B6();
	void opcode_B7();
	void opcode_B8();
	void opcode_B9();
	void opcode_BA();
	void opcode_BB();
	void opcode_BC();
	void opcode_BD();
	void opcode_BE();
	void opcode_BF();
	void opcode_C0();
	void opcode_C1();
	void opcode_C2();
	void opcode_C3();
	void opcode_C4();
	void opcode_C5();
	void opcode_C6();
	void opcode_C7();
	void opcode_C8();
	void opcode_C9();
	void opcode_CA();
	void opcode_CB();
	void opcode_CC();
	void opcode_CD();
	void opcode_CE();
	void opcode_CF();
	void opcode_D0();
	void opcode_D1();
	void opcode_D2();
	void opcode_D3();
	void opcode_D4();
	void opcode_D5();
	void opcode_D6();
	void opcode_D7();
	void opcode_D8();
	void opcode_D9();
	void opcode_DA();
	void opcode_DB();
	void opcode_DC();
	void opcode_DD();
	void opcode_DE();
	void opcode_DF();
	void opcode_E0();
	void opcode_E1();
	void opcode_E2();
	void opcode_E3();
	void opcode_E4();
	void opcode_E5();
	void opcode_E6();
	void opcode_E7();
	void opcode_E8();
	void opcode_E9();
	void opcode_EA();
	void opcode_EB();
	void opcode_EC();
	void opcode_ED();
	void opcode_EE();
	void opcode_EF();
	void opcode_F0();
	void opcode_F1();
	void opcode_F2();
	void opcode_F3();
	void opcode_F4();
	void opcode_F5();
	void opcode_F6();
	void opcode_F7();
	void opcode_F8();
	void opcode_F9();
	void opcode_FA();
	void opcode_FB();
	void opcode_FC();
	void opcode_FD();
	void opcode_FE();
	void opcode_FF();
#pragma endregion



	void initialize_function_pointers();
	std::function<void(void)> instructions[256];

	//void run_opcode();


	bool start_NMI;
	unsigned char NMI_delay = 0;
	bool start_IRQ;
	unsigned char IRQ_delay = 0;




	controller_base *first_controller;
	controller_base *second_controller;

	mapper_base* mapper;
	
public:
	CPU();
	~CPU();

	
	void emulate_cycle();
	
	void memory_power_up_state();
	void memory_reset_state();

	void connect_PPU(PPU &ppu_src) { ppu = &ppu_src; }

	const long long &get_total_cycles() { return total_cycles;  }
	const unsigned char &get_temporary_cycles() { return cycles;  }

	void prepare_to_debug();
	void initializeProgramCounter() { PC = (*memory[0xFFFD] << 8) | *memory[0xFFFC]; }
	
	void run_NMI();
	void set_NMI(const unsigned char& delay = 0) { start_NMI = true; NMI_delay = delay; }
	void unset_NMI() { start_NMI = false; }
	const bool& NMI_state() const { return start_NMI;  }
	const unsigned char& NMI_delay_state() const { return NMI_delay;  }


	void set_IRQ(const unsigned char& delay = 0) { start_IRQ = true; IRQ_delay = delay; }
	void unset_IRQ() { start_IRQ = false; }
	const bool& IRQ_state() const { return start_IRQ; }
	const unsigned char& IRQ_delay_state() const { return IRQ_delay; }

	
	
	void save_memory_state(unsigned short line_limiter = 0xF);

	void connect_controller(controller_base* controller, const unsigned char& controller_number = 0)
	{
		if (controller_number == 0)
			first_controller = controller;
		else if (controller_number == 1)
			second_controller = controller;
	}



	void connect_mapper(mapper_base* mapper_src) { mapper = mapper_src; }
	

#ifdef DEBUG_CPU
	const unsigned int &get_debug_cycle() { return emulator_debug_cycles; }
#endif // DEBUG_CPU

};