#include "CPU.h"
#include<iostream>

//Do zrobienia opcode 8B - XAA z innym trybem adresowania



void CPU::opcode_00()//BRK - Implied
{
	BRK();
	cycles = 7;
}

void CPU::opcode_01()//ORA - Indexed Indirect X
{
	ORA(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_02()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_03()//SLO - Indirect Indexed X
{
	SLO(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_04()//DOP - Zero Page
{
	DOP(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_05()//ORA - Zero Page
{
	ORA(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_06()//ASL - Zero Page
{
	ASL(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_07()//SLO - Zero Page
{
	SLO(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_08()//PHP - Implied
{
	PHP();
	cycles = 3;
}

void CPU::opcode_09()//ORA - Immediate
{
	ORA(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_0A()//ASL - accumulator
{
	ASL_accumulator();
	cycles = 2;
}

void CPU::opcode_0B()//AAC, ANC - Immediate
{
	AAC(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_0C()//TOP - Absolute
{
	TOP(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_0D()//ORA - Absolute
{
	ORA(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_0E()//ASL - Absolute
{
	ASL(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_0F()//SLO - Absolute
{
	SLO(get_adress_Absolute());
	cycles = 6;
}


void CPU::opcode_10()//BPL - Relative
{
	const unsigned short old_adress = PC;
	if (BPL(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}


void CPU::opcode_11()//ORA - Indirect Indexed Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	ORA(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}


void CPU::opcode_12()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_13()//SLO - Indexed Indirect Y
{
	SLO(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}


void CPU::opcode_14()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_15()//ORA - Indexed Zero Page X
{
	ORA(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_16()//ASL - Indexed Zero Page X
{
	ASL(get_adress_IndexedZeroPage_X());
	cycles = 6;
}


void CPU::opcode_17()//SLO - Indexed Zero Page X
{
	SLO(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_18()//CLC - Implied
{
	CLC();
	cycles = 2;
}

void CPU::opcode_19()//ORA - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	ORA(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_1A()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_1B()//SLO - Indexed Absolute Y
{
	SLO(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_1C()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_1D()//ORA - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	ORA(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_1E()//ASL - Indexed Absolute X
{
	ASL(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_1F()//SLO - Indexed Absolute X
{
	SLO(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_20()//JSR - Absolute
{
	JSR(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_21()//AND - Indexed Indirect X
{
	AND(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_22()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_23()//RLA - Indirect Indexed X
{
	RLA(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_24()//BIT - Zero Page
{
	BIT(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_25()//AND - Zero Page
{
	AND(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_26()//ROL - Zero Page
{
	ROL(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_27()//RLA - Zero PAge
{
	RLA(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_28()//PLP - Implied
{
	PLP();
	cycles = 4;
}

void CPU::opcode_29()//AND - Immediate
{
	AND(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_2A()//ROL - Accumulator
{
	ROL_accumulator();
	cycles = 2;
}

void CPU::opcode_2B()//AAC, ANC - Immediate
{
	AAC(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_2C()//BIT - Absolute
{
	BIT(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_2D()//AND - Absolute
{
	AND(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_2E()//ROL - Absolute
{
	ROL(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_2F()//RLA - Absolute
{
	RLA(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_30()//BMI - Relative
{
	const unsigned short old_adress = PC;
	if (BMI(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_31()//AND - Indirect Indexed Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	AND(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_32()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_33()//RLA - Indexed Indirect Y
{
	RLA(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}

void CPU::opcode_34()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_35()//AND - Indexed Zero Page X
{
	AND(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_36()//ROL - Indexed Zero Page X
{
	ROL(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_37()//RLA - Indexed Zero Page X
{
	RLA(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_38()//SEC - Implied
{
	SEC();
	cycles = 2;
}

void CPU::opcode_39()//AND - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	AND(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_3A()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_3B()//RLA - Indexed Absolute Y
{
	RLA(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_3C()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_3D()//AND - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	AND(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_3E()//ROL - Indexed Absolute X
{
	ROL(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_3F()//RLA - Indexed Absolute X
{
	RLA(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_40()//RTI - Implied
{
	RTI();
	cycles = 6;
}

void CPU::opcode_41()//EOR - Indexed Indirect X
{
	EOR(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_42()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_43()//SRE - Indirect Indexed X
{
	SRE(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_44()//DOP - Zero Page
{
	DOP(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_45()//EOR - Zero Page
{
	EOR(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_46()//LSR - Zero Page
{
	LSR(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_47()//SRE - Zero Page
{
	SRE(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_48()//PHA - Implied
{
	PHA();
	cycles = 3;
}

void CPU::opcode_49()//EOR - Immediate
{
	EOR(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_4A()//LSR - Accumulator
{
	LSR_accumulator();
	cycles = 2;
}

void CPU::opcode_4B()//ASR - Immediate
{
	ASR(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_4C()//JUMP - Absolute
{
	JMP(get_adress_Absolute());
	cycles = 3;
}

void CPU::opcode_4D()//EOR - Absolute
{
	EOR(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_4E()//LSR - Absolute
{
	LSR(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_4F()//SRE - Absolute
{
	SRE(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_50()//BVC - Relative
{
	const unsigned short old_adress = PC;
	if (BVC(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_51()//EOR - Indirect Indexed Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	EOR(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_52()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_53()//SRE - Indexed Indirect Y
{
	SRE(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}

void CPU::opcode_54()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_55()//EOR - Indexed Zero Page X
{
	EOR(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_56()//LSR - Indexed Zero Page X
{
	LSR(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_57()//SRE - Indexed Zero Page X
{
	SRE(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_58()//CLI - Implied
{
	CLI();
	cycles = 2;
}

void CPU::opcode_59()//EOR - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	EOR(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_5A()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_5B()//SRE - Indexed Absolute Y
{
	SRE(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_5C()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}
void CPU::opcode_5D()//EOR - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	EOR(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_5E()//LSR - Indexed Absolute X
{
	LSR(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_5F()//SRE - Indexed Absolute X
{
	SRE(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_60()//RTS - Implied
{
	RTS();
	cycles = 6;
}

void CPU::opcode_61()//ADC - Indexed Indirect X
{
	ADC(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_62()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_63()//RRA - Indirect Indexed X
{
	RRA(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_64()//DOP - Zero Page
{
	DOP(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_65()//ADC - Zero Page
{
	ADC(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_66()//ROR - Zero Page
{
	ROR(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_67()//RRA - Zero Page
{
	RRA(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_68()//PLA - Implied
{
	PLA();
	cycles = 4;
}

void CPU::opcode_69()//ADC - Immediate
{
	ADC(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_6A()//ROR - Accumulator
{
	ROR_accumulator();
	cycles = 2;
}

void CPU::opcode_6B()//ARR - Immediate
{
	ARR(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_6C()//JUMP - Indirect
{
	JMP(get_adress_Indirect());
	cycles = 5;
}

void CPU::opcode_6D()//ADC - Absolute
{
	ADC(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_6E()//ROR - Absolute
{
	ROR(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_6F()//RRA - Absolute
{
	RRA(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_70()//BVS - Relative
{
	const unsigned short old_adress = PC;
	if (BVS(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_71()//ADC - Indirect Indexed Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
	ADC(adress);
}

void CPU::opcode_72()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_73()//RRA - Indexed Indirect Y
{
	RRA(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}

void CPU::opcode_74()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_75()//ADC - Indexed Zero Page X
{
	ADC(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_76()//ROR - Indexed Zero Page X
{
	ROR(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_77()//RRA - Indexed Zero Page X
{
	RRA(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_78()//SEI - Implied
{
	SEI();
	cycles = 2;
}

void CPU::opcode_79()//ADC - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	ADC(adress);
	if (checkPageCrossed(adress - Y, adress))		//Sprawdza, czy nie przekroczono strony(0xFF = 1 Bajt)
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_7A()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_7B()//RRA - Indexed Absolute Y
{
	RRA(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_7C()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_7D()//ADC - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	ADC(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_7E()//ROR - Indexed Absolute X
{
	ROR(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_7F()//RRA - Indexed Absolute X
{
	RRA(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_80()//DOP - Immediate
{
	DOP(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_81()//STA - Indirect Indexed X
{
	cycles = 6;
	STA(get_adress_Indirect_Indexed_X());
}

void CPU::opcode_82()//DOP - Immediate
{
	DOP(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_83()//AAX - Indirect Indexed X
{
	AAX(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_84()//STY - Zero Page
{
	cycles = 3;
	STY(get_adress_ZeroPage());
}

void CPU::opcode_85()//STA - Zero Page
{
	cycles = 3;
	STA(get_adress_ZeroPage());
}

void CPU::opcode_86()//STX - Zero Page
{
	cycles = 3;
	STX(get_adress_ZeroPage());
}

void CPU::opcode_87()//AAX - Zero Page
{
	AAX(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_88()//DEY - Implied
{
	DEY();
	cycles = 2;
}

void CPU::opcode_89()//DOP - Immediate
{
	DOP(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_8A()//TXA - Implied
{
	TXA();
	cycles = 2;
}

void CPU::opcode_8B()
{
	//XAA - Do zrobienia
}

void CPU::opcode_8C()//STY - Absolute
{
	cycles = 4;
	STY(get_adress_Absolute());
}

void CPU::opcode_8D()//STA - Absolute
{
	cycles = 4;
	STA(get_adress_Absolute());
}

void CPU::opcode_8E()//STX - Absolute
{
	cycles = 4;
	STX(get_adress_Absolute());
}

void CPU::opcode_8F()//AAX - Absolute
{
	AAX(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_90()//BCC - Relative
{
	const unsigned short old_adress = PC;
	if (BCC(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_91()//STA - Indexed Indirect Y
{
	cycles = 6;
	STA(get_adress_Indexed_Indirect_Y());
}

void CPU::opcode_92()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_93()//AXA - Indexed Indirect Y
{
	AXA(get_adress_Indexed_Indirect_Y());
	cycles = 6;
}

void CPU::opcode_94()//STY - Indexed Zero Page X
{
	cycles = 4;
	STY(get_adress_IndexedZeroPage_X());
}

void CPU::opcode_95()//STA - Indexed Zero Page X
{
	cycles = 4;
	STA(get_adress_IndexedZeroPage_X());
}

void CPU::opcode_96()//STX - Indexed Zero Page Y
{
	cycles = 4;
	STX(get_adress_IndexedZeroPage_Y());
}

void CPU::opcode_97()//AAX - Indexed Zero Page Y
{
	AAX(get_adress_IndexedZeroPage_Y());
	cycles = 4;
}

void CPU::opcode_98()//TYA - Implied
{
	TYA();
	cycles = 2;
}

void CPU::opcode_99()//STA - Indexed Absolute Y
{
	cycles = 5;
	STA(get_adress_IndexedAbsolute_Y());
}

void CPU::opcode_9A()//TXS - Implied
{
	TXS();
	cycles = 2;
}

void CPU::opcode_9B()//XAS - Indexed Absolute Y
{
	XAS(get_adress_IndexedAbsolute_Y());
	cycles = 5;
}

void CPU::opcode_9C()//SHY - Indexed Absolute X
{	
	SHY(get_adress_IndexedAbsolute_X());
	cycles = 5;
}

void CPU::opcode_9D()//STA - Indexed Absolute X
{
	cycles = 5;
	STA(get_adress_IndexedAbsolute_X());
}

void CPU::opcode_9E()//SHX - Indexed Absolute X
{
	SHX(get_adress_IndexedAbsolute_Y());
	cycles = 5;
}

void CPU::opcode_9F()//AXA - Indexed Absolute Y
{
	AXA(get_adress_IndexedAbsolute_Y());
	cycles = 5;
}

void CPU::opcode_A0()//LDY - Immediate
{
	cycles = 2;
	LDY(get_adress_Immediate());
}

void CPU::opcode_A1()//LDA - Indexed Indirect X
{
	cycles = 6;
	LDA(get_adress_Indirect_Indexed_X());
}

void CPU::opcode_A2()//LDX - Immediate
{
	cycles = 2;
	LDX(get_adress_Immediate());
}

void CPU::opcode_A3()//LAX - Indirect Indexed X
{
	LAX(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_A4()//LDY - Zero Page
{
	cycles = 3;
	LDY(get_adress_ZeroPage());
}

void CPU::opcode_A5()//LDA - Zero Page
{
	cycles = 3;
	LDA(get_adress_ZeroPage());
}

void CPU::opcode_A6()//LDX - Zero Page
{
	cycles = 3;
	LDX(get_adress_ZeroPage());
}

void CPU::opcode_A7()//LAX - Zero Page
{
	LAX(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_A8()//TAY - Implied
{
	TAY();
	cycles = 2;
}

void CPU::opcode_A9()//LDA - Immediate
{
	cycles = 2;
	LDA(get_adress_Immediate());
}

void CPU::opcode_AA()//TAX - Implied
{
	TAX();
	cycles = 2;
}

void CPU::opcode_AB()//LAX - Immediate
{
	LAX(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_AC()//LDY - Absolute
{
	cycles = 4;
	LDY(get_adress_Absolute());
}

void CPU::opcode_AD()//LDA - Absolute
{
	cycles = 4;
	LDA(get_adress_Absolute());
}

void CPU::opcode_AE()//LDX - Absolute
{
	cycles = 4;
	LDX(get_adress_Absolute());
}

void CPU::opcode_AF()//LAX - Absolute
{
	LAX(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_B0()//BCS - Relative
{
	const unsigned short old_adress = PC;
	if (BCS(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_B1()//LDA - Indexed Indirect Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	if (checkPageCrossed(adress - Y, adress))//Sprawdza, czy strona nie zosta³a przekroczona przy tworzeniu adresu
		cycles = 5 + 1;
	else
		cycles = 5;
	LDA(adress);
	
}

void CPU::opcode_B2()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_B3()//LAX - Indexed Indirect Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	LAX(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_B4()//LDY - Indexed Zero Page X
{
	cycles = 4;
	LDY(get_adress_IndexedZeroPage_X());
}

void CPU::opcode_B5()//LDA - Indexed Zero Page X
{
	cycles = 4;
	LDA(get_adress_IndexedZeroPage_X());
}

void CPU::opcode_B6()//LDX - Indexed Zero Page Y
{
	cycles = 4;
	LDX(get_adress_IndexedZeroPage_Y());
}

void CPU::opcode_B7()//LAX - Indexed Zero Page Y
{
	LAX(get_adress_IndexedZeroPage_Y());
	cycles = 4;
}

void CPU::opcode_B8()//CLV - Implied
{
	CLV();
	cycles = 2;
}

void CPU::opcode_B9()//LDA - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
	LDA(adress);
}

void CPU::opcode_BA()//TSX - Implied
{
	TSX();
	cycles = 2;
}

void CPU::opcode_BB()//LAR - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	LAR(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_BC()//LDY - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
	LDY(adress);
}

void CPU::opcode_BD()//LDA - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;

	

	LDA(adress);
}

void CPU::opcode_BE()//LDX - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
	LDX(adress);
}

void CPU::opcode_BF()//LAX - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	LAX(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_C0()//CPY - Immediate
{
	CPY(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_C1()//CMP - Indexed Indirect X
{
	CMP(get_adress_Indirect_Indexed_X());
	cycles = 6;
}

void CPU::opcode_C2()//DOP - Immediate
{
	DOP(get_adress_Immediate());
	cycles = 2;
}
void CPU::opcode_C3()//DCP - Indirect Indexed X
{
	DCP(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_C4()//CPY - Zero Page
{
	CPY(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_C5()//CMP - Zero Page
{
	CMP(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_C6()//DEC - Zero Page
{
	DEC(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_C7()//DCP - Zero Page
{
	DCP(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_C8()//INY - Implied
{
	INY();
	cycles = 2;
}

void CPU::opcode_C9()//CMP - Immediate
{
	CMP(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_CA()//DEX - Implied
{
	DEX();
	cycles = 2;
}

void CPU::opcode_CB()//AXS = Immediate
{
	AXS(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_CC()//CPY - Absolute
{
	CPY(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_CD()//CMP - Absolute
{
	CMP(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_CE()//DEC - Absolute
{
	DEC(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_CF()//DCP - Absolute
{
	DCP(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_D0()//BNE - Relative
{
	const unsigned short old_adress = PC;
	if (BNE(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_D1()//CMP - Indirect Indexed Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	CMP(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_D2()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_D3()//DCP - Indexed Indirect Y
{
	DCP(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}

void CPU::opcode_D4()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_D5()//CMP - Indexed Zero Page X
{
	CMP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_D6()//DEC - Indexed Zero Page X
{
	DEC(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_D7()//DCP - Indexed Zero Page X
{
	DCP(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_D8()//CLD - Implied
{
	CLD();
	cycles = 2;
}

void CPU::opcode_D9()//CMP - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	CMP(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_DA()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_DB()//DCP - Indexed Absolute Y
{
	DCP(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_DC()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_DD()
//CMP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	CMP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_DE()//DEC - Indexed Absolute X
{
	DEC(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_DF()//DCP - Indexed Absolute X
{
	DCP(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_E0()//CPX - Immediate
{
	CPX(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_E1()//SBC - Indirect Indexed X
{
	const unsigned short old_adress = PC;
	const unsigned short adress = get_adress_Indirect_Indexed_X();
	SBC(adress);
	if (checkPageCrossed(adress, old_adress + instruction_sizes[opcode]))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_E2()//DOP - Immediate
{
	DOP(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_E3()//ISC - Indirect Indexed X
{
	ISC(get_adress_Indirect_Indexed_X());
	cycles = 8;
}

void CPU::opcode_E4()//CPX - Zero Page
{
	CPX(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_E5()//SBC - Zero Page
{
	SBC(get_adress_ZeroPage());
	cycles = 3;
}

void CPU::opcode_E6()//INC - Zero Page
{
	INC(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_E7()//ISC - Zero Page
{
	ISC(get_adress_ZeroPage());
	cycles = 5;
}

void CPU::opcode_E8()//INX - Implied
{
	INX();
	cycles = 2;
}

void CPU::opcode_E9()//SBC - Immediate
{
	SBC(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_EA()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_EB()//SBC - Immediate(illegal)
{
	SBC(get_adress_Immediate());
	cycles = 2;
}

void CPU::opcode_EC()//CPX - Absolute
{
	CPX(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_ED()//SBC - Absolute
{
	SBC(get_adress_Absolute());
	cycles = 4;
}

void CPU::opcode_EE()//INC - Absolute
{
	INC(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_EF()//ISC - Absolute
{
	ISC(get_adress_Absolute());
	cycles = 6;
}

void CPU::opcode_F0()//BEQ - Relative
{
	const unsigned short old_adress = PC;
	if (BEQ(get_adress_Relative()))
	{
		if (checkPageCrossed(PC + instruction_sizes[opcode], old_adress + instruction_sizes[opcode]))
			cycles = 2 + 1 + 1;//Branched and crossed page
		else
			cycles = 2 + 1;//Only branched
	}
	else
		cycles = 2;//Didn't branch
}

void CPU::opcode_F1()//SBC - Indexed Indirect Y
{
	const unsigned short adress = get_adress_Indexed_Indirect_Y();
	SBC(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 5 + 1;
	else
		cycles = 5;
}

void CPU::opcode_F2()//KIL - Implied
{
	//KIL();		//¯eby uzyskaæ efekt KIL, nie trzeba nawet wywo³ywaæ tej funkcji. Cycles nie trzeba aktualizowaæ - przyjmujemy, ¿e ta funkcja potrzebuje nieskoñczonej liczby cykli
}

void CPU::opcode_F3()//ISC - Indexed Indirect Y
{
	ISC(get_adress_Indexed_Indirect_Y());
	cycles = 8;
}

void CPU::opcode_F4()//DOP - Indexed Zero Page X
{
	DOP(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_F5()//SBC - Indexed Zero Page X
{
	SBC(get_adress_IndexedZeroPage_X());
	cycles = 4;
}

void CPU::opcode_F6()//INC - Indexed Zero Page X
{
	INC(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_F7()//ISC - Indexed Zero Page X
{
	ISC(get_adress_IndexedZeroPage_X());
	cycles = 6;
}

void CPU::opcode_F8()//SED - Implied
{
	SED();
	cycles = 2;
}

void CPU::opcode_F9()//SBC - Indexed Absolute Y
{
	const unsigned short adress = get_adress_IndexedAbsolute_Y();
	SBC(adress);
	if (checkPageCrossed(adress - Y, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_FA()//NOP - Implied
{
	NOP();
	cycles = 2;
}

void CPU::opcode_FB()//ISC - Indexed Absolute Y
{
	ISC(get_adress_IndexedAbsolute_Y());
	cycles = 7;
}

void CPU::opcode_FC()//TOP - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	TOP(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_FD()//SBC - Indexed Absolute X
{
	const unsigned short adress = get_adress_IndexedAbsolute_X();
	SBC(adress);
	if (checkPageCrossed(adress - X, adress))
		cycles = 4 + 1;
	else
		cycles = 4;
}

void CPU::opcode_FE()//INC - Indexed Absolute X
{
	INC(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::opcode_FF()//ISC - Indexed Absolute X
{
	ISC(get_adress_IndexedAbsolute_X());
	cycles = 7;
}

void CPU::initialize_function_pointers()
{
	instructions[0x0] = std::bind(&CPU::opcode_00, this);
	instructions[0x1] = std::bind(&CPU::opcode_01, this);
	instructions[0x2] = std::bind(&CPU::opcode_02, this);
	instructions[0x3] = std::bind(&CPU::opcode_03, this);
	instructions[0x4] = std::bind(&CPU::opcode_04, this);
	instructions[0x5] = std::bind(&CPU::opcode_05, this);
	instructions[0x6] = std::bind(&CPU::opcode_06, this);
	instructions[0x7] = std::bind(&CPU::opcode_07, this);
	instructions[0x8] = std::bind(&CPU::opcode_08, this);
	instructions[0x9] = std::bind(&CPU::opcode_09, this);
	instructions[0xA] = std::bind(&CPU::opcode_0A, this);
	instructions[0xB] = std::bind(&CPU::opcode_0B, this);
	instructions[0xC] = std::bind(&CPU::opcode_0C, this);
	instructions[0xD] = std::bind(&CPU::opcode_0D, this);
	instructions[0xE] = std::bind(&CPU::opcode_0E, this);
	instructions[0xF] = std::bind(&CPU::opcode_0F, this);
	instructions[0x10] = std::bind(&CPU::opcode_10, this);
	instructions[0x11] = std::bind(&CPU::opcode_11, this);
	instructions[0x12] = std::bind(&CPU::opcode_12, this);
	instructions[0x13] = std::bind(&CPU::opcode_13, this);
	instructions[0x14] = std::bind(&CPU::opcode_14, this);
	instructions[0x15] = std::bind(&CPU::opcode_15, this);
	instructions[0x16] = std::bind(&CPU::opcode_16, this);
	instructions[0x17] = std::bind(&CPU::opcode_17, this);
	instructions[0x18] = std::bind(&CPU::opcode_18, this);
	instructions[0x19] = std::bind(&CPU::opcode_19, this);
	instructions[0x1A] = std::bind(&CPU::opcode_1A, this);
	instructions[0x1B] = std::bind(&CPU::opcode_1B, this);
	instructions[0x1C] = std::bind(&CPU::opcode_1C, this);
	instructions[0x1D] = std::bind(&CPU::opcode_1D, this);
	instructions[0x1E] = std::bind(&CPU::opcode_1E, this);
	instructions[0x1F] = std::bind(&CPU::opcode_1F, this);
	instructions[0x20] = std::bind(&CPU::opcode_20, this);
	instructions[0x21] = std::bind(&CPU::opcode_21, this);
	instructions[0x22] = std::bind(&CPU::opcode_22, this);
	instructions[0x23] = std::bind(&CPU::opcode_23, this);
	instructions[0x24] = std::bind(&CPU::opcode_24, this);
	instructions[0x25] = std::bind(&CPU::opcode_25, this);
	instructions[0x26] = std::bind(&CPU::opcode_26, this);
	instructions[0x27] = std::bind(&CPU::opcode_27, this);
	instructions[0x28] = std::bind(&CPU::opcode_28, this);
	instructions[0x29] = std::bind(&CPU::opcode_29, this);
	instructions[0x2A] = std::bind(&CPU::opcode_2A, this);
	instructions[0x2B] = std::bind(&CPU::opcode_2B, this);
	instructions[0x2C] = std::bind(&CPU::opcode_2C, this);
	instructions[0x2D] = std::bind(&CPU::opcode_2D, this);
	instructions[0x2E] = std::bind(&CPU::opcode_2E, this);
	instructions[0x2F] = std::bind(&CPU::opcode_2F, this);
	instructions[0x30] = std::bind(&CPU::opcode_30, this);
	instructions[0x31] = std::bind(&CPU::opcode_31, this);
	instructions[0x32] = std::bind(&CPU::opcode_32, this);
	instructions[0x33] = std::bind(&CPU::opcode_33, this);
	instructions[0x34] = std::bind(&CPU::opcode_34, this);
	instructions[0x35] = std::bind(&CPU::opcode_35, this);
	instructions[0x36] = std::bind(&CPU::opcode_36, this);
	instructions[0x37] = std::bind(&CPU::opcode_37, this);
	instructions[0x38] = std::bind(&CPU::opcode_38, this);
	instructions[0x39] = std::bind(&CPU::opcode_39, this);
	instructions[0x3A] = std::bind(&CPU::opcode_3A, this);
	instructions[0x3B] = std::bind(&CPU::opcode_3B, this);
	instructions[0x3C] = std::bind(&CPU::opcode_3C, this);
	instructions[0x3D] = std::bind(&CPU::opcode_3D, this);
	instructions[0x3E] = std::bind(&CPU::opcode_3E, this);
	instructions[0x3F] = std::bind(&CPU::opcode_3F, this);
	instructions[0x40] = std::bind(&CPU::opcode_40, this);
	instructions[0x41] = std::bind(&CPU::opcode_41, this);
	instructions[0x42] = std::bind(&CPU::opcode_42, this);
	instructions[0x43] = std::bind(&CPU::opcode_43, this);
	instructions[0x44] = std::bind(&CPU::opcode_44, this);
	instructions[0x45] = std::bind(&CPU::opcode_45, this);
	instructions[0x46] = std::bind(&CPU::opcode_46, this);
	instructions[0x47] = std::bind(&CPU::opcode_47, this);
	instructions[0x48] = std::bind(&CPU::opcode_48, this);
	instructions[0x49] = std::bind(&CPU::opcode_49, this);
	instructions[0x4A] = std::bind(&CPU::opcode_4A, this);
	instructions[0x4B] = std::bind(&CPU::opcode_4B, this);
	instructions[0x4C] = std::bind(&CPU::opcode_4C, this);
	instructions[0x4D] = std::bind(&CPU::opcode_4D, this);
	instructions[0x4E] = std::bind(&CPU::opcode_4E, this);
	instructions[0x4F] = std::bind(&CPU::opcode_4F, this);
	instructions[0x50] = std::bind(&CPU::opcode_50, this);
	instructions[0x51] = std::bind(&CPU::opcode_51, this);
	instructions[0x52] = std::bind(&CPU::opcode_52, this);
	instructions[0x53] = std::bind(&CPU::opcode_53, this);
	instructions[0x54] = std::bind(&CPU::opcode_54, this);
	instructions[0x55] = std::bind(&CPU::opcode_55, this);
	instructions[0x56] = std::bind(&CPU::opcode_56, this);
	instructions[0x57] = std::bind(&CPU::opcode_57, this);
	instructions[0x58] = std::bind(&CPU::opcode_58, this);
	instructions[0x59] = std::bind(&CPU::opcode_59, this);
	instructions[0x5A] = std::bind(&CPU::opcode_5A, this);
	instructions[0x5B] = std::bind(&CPU::opcode_5B, this);
	instructions[0x5C] = std::bind(&CPU::opcode_5C, this);
	instructions[0x5D] = std::bind(&CPU::opcode_5D, this);
	instructions[0x5E] = std::bind(&CPU::opcode_5E, this);
	instructions[0x5F] = std::bind(&CPU::opcode_5F, this);
	instructions[0x60] = std::bind(&CPU::opcode_60, this);
	instructions[0x61] = std::bind(&CPU::opcode_61, this);
	instructions[0x62] = std::bind(&CPU::opcode_62, this);
	instructions[0x63] = std::bind(&CPU::opcode_63, this);
	instructions[0x64] = std::bind(&CPU::opcode_64, this);
	instructions[0x65] = std::bind(&CPU::opcode_65, this);
	instructions[0x66] = std::bind(&CPU::opcode_66, this);
	instructions[0x67] = std::bind(&CPU::opcode_67, this);
	instructions[0x68] = std::bind(&CPU::opcode_68, this);
	instructions[0x69] = std::bind(&CPU::opcode_69, this);
	instructions[0x6A] = std::bind(&CPU::opcode_6A, this);
	instructions[0x6B] = std::bind(&CPU::opcode_6B, this);
	instructions[0x6C] = std::bind(&CPU::opcode_6C, this);
	instructions[0x6D] = std::bind(&CPU::opcode_6D, this);
	instructions[0x6E] = std::bind(&CPU::opcode_6E, this);
	instructions[0x6F] = std::bind(&CPU::opcode_6F, this);
	instructions[0x70] = std::bind(&CPU::opcode_70, this);
	instructions[0x71] = std::bind(&CPU::opcode_71, this);
	instructions[0x72] = std::bind(&CPU::opcode_72, this);
	instructions[0x73] = std::bind(&CPU::opcode_73, this);
	instructions[0x74] = std::bind(&CPU::opcode_74, this);
	instructions[0x75] = std::bind(&CPU::opcode_75, this);
	instructions[0x76] = std::bind(&CPU::opcode_76, this);
	instructions[0x77] = std::bind(&CPU::opcode_77, this);
	instructions[0x78] = std::bind(&CPU::opcode_78, this);
	instructions[0x79] = std::bind(&CPU::opcode_79, this);
	instructions[0x7A] = std::bind(&CPU::opcode_7A, this);
	instructions[0x7B] = std::bind(&CPU::opcode_7B, this);
	instructions[0x7C] = std::bind(&CPU::opcode_7C, this);
	instructions[0x7D] = std::bind(&CPU::opcode_7D, this);
	instructions[0x7E] = std::bind(&CPU::opcode_7E, this);
	instructions[0x7F] = std::bind(&CPU::opcode_7F, this);
	instructions[0x80] = std::bind(&CPU::opcode_80, this);
	instructions[0x81] = std::bind(&CPU::opcode_81, this);
	instructions[0x82] = std::bind(&CPU::opcode_82, this);
	instructions[0x83] = std::bind(&CPU::opcode_83, this);
	instructions[0x84] = std::bind(&CPU::opcode_84, this);
	instructions[0x85] = std::bind(&CPU::opcode_85, this);
	instructions[0x86] = std::bind(&CPU::opcode_86, this);
	instructions[0x87] = std::bind(&CPU::opcode_87, this);
	instructions[0x88] = std::bind(&CPU::opcode_88, this);
	instructions[0x89] = std::bind(&CPU::opcode_89, this);
	instructions[0x8A] = std::bind(&CPU::opcode_8A, this);
	instructions[0x8B] = std::bind(&CPU::opcode_8B, this);
	instructions[0x8C] = std::bind(&CPU::opcode_8C, this);
	instructions[0x8D] = std::bind(&CPU::opcode_8D, this);
	instructions[0x8E] = std::bind(&CPU::opcode_8E, this);
	instructions[0x8F] = std::bind(&CPU::opcode_8F, this);
	instructions[0x90] = std::bind(&CPU::opcode_90, this);
	instructions[0x91] = std::bind(&CPU::opcode_91, this);
	instructions[0x92] = std::bind(&CPU::opcode_92, this);
	instructions[0x93] = std::bind(&CPU::opcode_93, this);
	instructions[0x94] = std::bind(&CPU::opcode_94, this);
	instructions[0x95] = std::bind(&CPU::opcode_95, this);
	instructions[0x96] = std::bind(&CPU::opcode_96, this);
	instructions[0x97] = std::bind(&CPU::opcode_97, this);
	instructions[0x98] = std::bind(&CPU::opcode_98, this);
	instructions[0x99] = std::bind(&CPU::opcode_99, this);
	instructions[0x9A] = std::bind(&CPU::opcode_9A, this);
	instructions[0x9B] = std::bind(&CPU::opcode_9B, this);
	instructions[0x9C] = std::bind(&CPU::opcode_9C, this);
	instructions[0x9D] = std::bind(&CPU::opcode_9D, this);
	instructions[0x9E] = std::bind(&CPU::opcode_9E, this);
	instructions[0x9F] = std::bind(&CPU::opcode_9F, this);
	instructions[0xA0] = std::bind(&CPU::opcode_A0, this);
	instructions[0xA1] = std::bind(&CPU::opcode_A1, this);
	instructions[0xA2] = std::bind(&CPU::opcode_A2, this);
	instructions[0xA3] = std::bind(&CPU::opcode_A3, this);
	instructions[0xA4] = std::bind(&CPU::opcode_A4, this);
	instructions[0xA5] = std::bind(&CPU::opcode_A5, this);
	instructions[0xA6] = std::bind(&CPU::opcode_A6, this);
	instructions[0xA7] = std::bind(&CPU::opcode_A7, this);
	instructions[0xA8] = std::bind(&CPU::opcode_A8, this);
	instructions[0xA9] = std::bind(&CPU::opcode_A9, this);
	instructions[0xAA] = std::bind(&CPU::opcode_AA, this);
	instructions[0xAB] = std::bind(&CPU::opcode_AB, this);
	instructions[0xAC] = std::bind(&CPU::opcode_AC, this);
	instructions[0xAD] = std::bind(&CPU::opcode_AD, this);
	instructions[0xAE] = std::bind(&CPU::opcode_AE, this);
	instructions[0xAF] = std::bind(&CPU::opcode_AF, this);
	instructions[0xB0] = std::bind(&CPU::opcode_B0, this);
	instructions[0xB1] = std::bind(&CPU::opcode_B1, this);
	instructions[0xB2] = std::bind(&CPU::opcode_B2, this);
	instructions[0xB3] = std::bind(&CPU::opcode_B3, this);
	instructions[0xB4] = std::bind(&CPU::opcode_B4, this);
	instructions[0xB5] = std::bind(&CPU::opcode_B5, this);
	instructions[0xB6] = std::bind(&CPU::opcode_B6, this);
	instructions[0xB7] = std::bind(&CPU::opcode_B7, this);
	instructions[0xB8] = std::bind(&CPU::opcode_B8, this);
	instructions[0xB9] = std::bind(&CPU::opcode_B9, this);
	instructions[0xBA] = std::bind(&CPU::opcode_BA, this);
	instructions[0xBB] = std::bind(&CPU::opcode_BB, this);
	instructions[0xBC] = std::bind(&CPU::opcode_BC, this);
	instructions[0xBD] = std::bind(&CPU::opcode_BD, this);
	instructions[0xBE] = std::bind(&CPU::opcode_BE, this);
	instructions[0xBF] = std::bind(&CPU::opcode_BF, this);
	instructions[0xC0] = std::bind(&CPU::opcode_C0, this);
	instructions[0xC1] = std::bind(&CPU::opcode_C1, this);
	instructions[0xC2] = std::bind(&CPU::opcode_C2, this);
	instructions[0xC3] = std::bind(&CPU::opcode_C3, this);
	instructions[0xC4] = std::bind(&CPU::opcode_C4, this);
	instructions[0xC5] = std::bind(&CPU::opcode_C5, this);
	instructions[0xC6] = std::bind(&CPU::opcode_C6, this);
	instructions[0xC7] = std::bind(&CPU::opcode_C7, this);
	instructions[0xC8] = std::bind(&CPU::opcode_C8, this);
	instructions[0xC9] = std::bind(&CPU::opcode_C9, this);
	instructions[0xCA] = std::bind(&CPU::opcode_CA, this);
	instructions[0xCB] = std::bind(&CPU::opcode_CB, this);
	instructions[0xCC] = std::bind(&CPU::opcode_CC, this);
	instructions[0xCD] = std::bind(&CPU::opcode_CD, this);
	instructions[0xCE] = std::bind(&CPU::opcode_CE, this);
	instructions[0xCF] = std::bind(&CPU::opcode_CF, this);
	instructions[0xD0] = std::bind(&CPU::opcode_D0, this);
	instructions[0xD1] = std::bind(&CPU::opcode_D1, this);
	instructions[0xD2] = std::bind(&CPU::opcode_D2, this);
	instructions[0xD3] = std::bind(&CPU::opcode_D3, this);
	instructions[0xD4] = std::bind(&CPU::opcode_D4, this);
	instructions[0xD5] = std::bind(&CPU::opcode_D5, this);
	instructions[0xD6] = std::bind(&CPU::opcode_D6, this);
	instructions[0xD7] = std::bind(&CPU::opcode_D7, this);
	instructions[0xD8] = std::bind(&CPU::opcode_D8, this);
	instructions[0xD9] = std::bind(&CPU::opcode_D9, this);
	instructions[0xDA] = std::bind(&CPU::opcode_DA, this);
	instructions[0xDB] = std::bind(&CPU::opcode_DB, this);
	instructions[0xDC] = std::bind(&CPU::opcode_DC, this);
	instructions[0xDD] = std::bind(&CPU::opcode_DD, this);
	instructions[0xDE] = std::bind(&CPU::opcode_DE, this);
	instructions[0xDF] = std::bind(&CPU::opcode_DF, this);
	instructions[0xE0] = std::bind(&CPU::opcode_E0, this);
	instructions[0xE1] = std::bind(&CPU::opcode_E1, this);
	instructions[0xE2] = std::bind(&CPU::opcode_E2, this);
	instructions[0xE3] = std::bind(&CPU::opcode_E3, this);
	instructions[0xE4] = std::bind(&CPU::opcode_E4, this);
	instructions[0xE5] = std::bind(&CPU::opcode_E5, this);
	instructions[0xE6] = std::bind(&CPU::opcode_E6, this);
	instructions[0xE7] = std::bind(&CPU::opcode_E7, this);
	instructions[0xE8] = std::bind(&CPU::opcode_E8, this);
	instructions[0xE9] = std::bind(&CPU::opcode_E9, this);
	instructions[0xEA] = std::bind(&CPU::opcode_EA, this);
	instructions[0xEB] = std::bind(&CPU::opcode_EB, this);
	instructions[0xEC] = std::bind(&CPU::opcode_EC, this);
	instructions[0xED] = std::bind(&CPU::opcode_ED, this);
	instructions[0xEE] = std::bind(&CPU::opcode_EE, this);
	instructions[0xEF] = std::bind(&CPU::opcode_EF, this);
	instructions[0xF0] = std::bind(&CPU::opcode_F0, this);
	instructions[0xF1] = std::bind(&CPU::opcode_F1, this);
	instructions[0xF2] = std::bind(&CPU::opcode_F2, this);
	instructions[0xF3] = std::bind(&CPU::opcode_F3, this);
	instructions[0xF4] = std::bind(&CPU::opcode_F4, this);
	instructions[0xF5] = std::bind(&CPU::opcode_F5, this);
	instructions[0xF6] = std::bind(&CPU::opcode_F6, this);
	instructions[0xF7] = std::bind(&CPU::opcode_F7, this);
	instructions[0xF8] = std::bind(&CPU::opcode_F8, this);
	instructions[0xF9] = std::bind(&CPU::opcode_F9, this);
	instructions[0xFA] = std::bind(&CPU::opcode_FA, this);
	instructions[0xFB] = std::bind(&CPU::opcode_FB, this);
	instructions[0xFC] = std::bind(&CPU::opcode_FC, this);
	instructions[0xFD] = std::bind(&CPU::opcode_FD, this);
	instructions[0xFE] = std::bind(&CPU::opcode_FE, this);
	instructions[0xFF] = std::bind(&CPU::opcode_FF, this);
}

