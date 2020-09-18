#include "CPU.h"
#include<iostream>




//Add With Carry - dodaje do akumulatora wartoœæ pobran¹ z pamiêci i flagê Carry.
void CPU::ADC(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)memory_value + flag.C;
	flag.V = ((A ^ sum) & (memory_value ^ sum) & 0x80);//Kod skopiowany z internetu, ale dzia³a
	
	A += flag.C + memory_value;
	flag.C = sum >> 8;
	
	flag.N = (A & 0x80);
	flag.Z = !A;
}


//Logical AND - wykonuje bitowe dzia³anie AND na akumulatorze, wykorzystuj¹c wartoœæ pobran¹ z pamiêci
void CPU::AND(const unsigned short &adress)
{
	A &= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}

//Arithmetic Shift Left - przesuwa bity o jedn¹ pozycjê w lewo. Ostatni bit jest zapisywany we fladze Carry
void CPU::ASL(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	flag.C = (memory_value & 0x80);
	memory_value <<= 1;
	memory_write(adress, memory_value);
	flag.Z = !(memory_value);
	flag.N = (memory_value & 0x80);
}

void CPU::ASL_accumulator()
{
	flag.C = (A & 0x80);
	A <<= 1;
	flag.Z = !(A);
	flag.N = (A & 0x80);
}


//Branch if Carry Clear - przenosi program w inne miejsce pamiêci, jeœli Carry Flag jest czyste
const bool CPU::BCC(const unsigned short &adress)
{
	if (!flag.C)
	{
		PC += (const char &)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Branch if Carry Set - przenosi program w inne miejsce pamiêci jeœli Carry Flag jest ustawione
const bool CPU::BCS(const unsigned short &adress)
{
	if (flag.C)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Branch if Equal - przenosi program w inne miejsce, jeœli Zero Flag jest ustawione
const bool CPU::BEQ(const unsigned short &adress)
{
	if (flag.Z)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}


//Bit Test - wykonuje dzia³anie bitowe AND na wartoœci z pamiêci i akumulatorze, ale nie zapisuje ich nigdzie. Przechowuje jedynie wartoœci ostatniego i przedostatniego bitu
//tego dzia³ania kolejno we flagach Negative i Overflow.
void CPU::BIT(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	flag.N = (memory_value & 0x80);
	flag.V = (memory_value & 0x40);
	flag.Z = !(A & memory_value);
}



//Branch if Minus - przenosi program w inne miejsce pamiêci jeœli Negative Flag jest ustawione
const bool CPU::BMI(const unsigned short &adress)
{
	if (flag.N)
	{
		PC += (const char &)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Branch if Not Equal - przenosi program w inne miejsce pamiêci jeœli Zero Flag jest czyste
const bool CPU::BNE(const unsigned short &adress)
{
	if (!flag.Z)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Branch if Positive - przenosi program w inne miejsce w pamiêci jeœli Negative Flag jest czyste
const bool CPU::BPL(const unsigned short &adress)
{
	if (!flag.N)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Force Interrupt - wymusza na procesorze wygenerowania ¿¹danie przerwania. Program Counter i statusy procesora s¹ odk³adane na stos.
//Nastêpnie flaga Break Command jest ustawiana, a do Program Counter ³adowana jest zawartoœæ z pamiêci 0xFFFE - 0xFFFF.
void CPU::BRK()
{
	//Regardless of what ANY 6502 documentation says, BRK is a 2 byte opcode.The
	//first is #$00, and the second is a padding byte.This explains why interrupt
	//routines called by BRK always return 2 bytes after the actual BRK opcode,
	//and not just 1.
	//6502 coded breakpoint handlers that treat BRK as a 1 - byte opcode have to
	//manually decrement the return address stored on the stack before executing a RTI.
	//
	//BRK causes a non-maskable interrupt and increments the program counter by one. Therefore an RTI will go to the address of the BRK +2 
	//so that BRK may be used to replace a two-byte instruction for debugging and the subsequent RTI will be correct. 
	PC += 2;

	const unsigned char status = (flag.N << 7) + (flag.V << 6) + (1 << 5) + (1/*flag.B*/ << 4) + (flag.D << 3) + (flag.I << 2) + (flag.Z << 1) + (flag.C * 1);;

	*stack[SP] = ((PC & 0xFF00) >> 8);//Najpierw najbardziej znacz¹ce bity z PC, przy œci¹ganiu ich ze stosu kolejnoœæ bêdzie odwrotna(Little Endian)
	--SP;
	*stack[SP] = (PC & 0x00FF);
	--SP;
	
	//One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. In fact, that's the only way a program can tell a BRK from /IRQ.
	//Bit 4(w statusie": Clear if interrupt vectoring, set if BRK or PHP
	*stack[SP] = (status | 0x10);		//push P on stack(with B flag set)
	--SP;


	flag.I = true;

	PC = ((const unsigned short &)(*memory[0xFFFF] << 8) | (*memory[0xFFFE]));
}

//Branch if Overflow Clear - przenosi program w inne miejsce, jeœli Overflow Flag jest czyste
const bool CPU::BVC(const unsigned short &adress)
{
	if (!flag.V)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Branch if Overflow Set - przenosi program w inne miejsce w pamiêci, jeœli Overflow Flag jest ustawione
const bool CPU::BVS(const unsigned short &adress)
{
	if (flag.V)
	{
		PC += (const char&)(memory_read(adress));
		return true;
	}
	else
		return false;
}

//Clear Carry Flag - czyœci Carry Flag(ustawia 0)
void CPU::CLC()
{
	flag.C = false;
}

//Clear Decimal Mode - czyœci Decimal Mode(ustawia na 0)
void CPU::CLD()
{
	//The state of the decimal flag is uncertain when the CPU is powered up and it is not reset when an interrupt is generated. 
	//In both cases you should include an explicit CLD to ensure that the flag is cleared before performing addition or subtraction.
	flag.D = false;
}

//Clear Interrupt Disable - Czyœci Interrupt Disable(ustawia na 0)
void CPU::CLI()
{
	flag.I = false;
}

//Clear Overflow Flag - czyœci Overflow Flag(ustawia na 0)
void CPU::CLV()
{
	flag.V = false;
}

//Compare - porównuje wartoœæ akuulatora z wartoœci¹ povran¹ z pamiêci, wynik zapisuje we flagach
void CPU::CMP(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = A - memory_value;
	flag.C = (A >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Compare X Register - porównuje wartoœæ Rejestru X z wartoœci¹ povran¹ z pamiêci, wynik zapisuje we flagach
void CPU::CPX(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = X - memory_value;
	flag.C = (X >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Compare Y Register - porównuje wartoœæ Rejestru Y z wartoœci¹ povran¹ z pamiêci, wynik zapisuje we flagach
void CPU::CPY(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = Y - memory_value;
	flag.C = (Y >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Decrement Memory - odejmuje 1 od wartosci znajdujacej sie w pamiêci
void CPU::DEC(const unsigned short &adress)
{	
	unsigned char memory_value = memory_read(adress);
	memory_value -= 1;
	memory_write(adress, memory_value);
	flag.Z = !(memory_value);
	flag.N = (memory_value & 0x80);
}


//Decrement X Register - odejmuje 1 od Rejestru X
void CPU::DEX()
{
	X--;
	flag.Z = !(X);
	flag.N = (X & 0x80);
}

//Decrement Y Register - odejmuje 1 od Rejestru Y
void CPU::DEY()
{
	Y--;
	flag.Z = !(Y);
	flag.N = (Y & 0x80);
}

//Exclusive OR - wykonuje bitow¹ operacjê XOR na akumulatorze, przy wykorzystaniu pobranej wartoœci z pamiêci
void CPU::EOR(const unsigned short &adress)
{
	A ^= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}



//Increment Memory - dodaje 1 do wartosci z pamiêci
void CPU::INC(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	memory_value += 1;
	memory_write(adress, memory_value);
	flag.Z = !(memory_value);
	flag.N = (0x80 & memory_value);
}




//Increment X Register - dodaje 1 do Rejestru X
void CPU::INX()
{
	(X)++;
	flag.Z = !(X);
	flag.N = (0x80 & X);
}

//Increment Y Register - dodaje 1 do Rejestru Y
void CPU::INY()
{
	++(Y);
	flag.Z = !(Y);
	flag.N = (0x80 & Y);
}


//Jump - ustawia Program counter na wartosc podan¹ w argumencie
void CPU::JMP(const unsigned short &adress)
{
	PC = adress;
}


//Jump to Subroutine - odk³ada adres "powrotny"(PC - 1) na stos, nastêpnie ustawia adres podany jako operand jako PC
void CPU::JSR(const unsigned short &adress)
{
	PC += instruction_sizes[opcode];
	const unsigned short value = PC - 1;
	*stack[SP] = ((value & 0xFF00) >> 8);
	--SP;
	*stack[SP] = (value & 0x00FF);
	--SP;

	PC = adress;
}


//Load Accumulator - ³aduje do akumulatora zawartoœæ pamiêci, której adres zosta³ podany jako operand
void CPU::LDA(const unsigned short &adress)
{
	A = memory_read(adress);

	flag.N = (A & 0x80);
	flag.Z = !(A);
}


//Load X Register - ³aduje do Rejestru X zawartoœæ pamiêci, której adres zosta³ podany jako operand
void CPU::LDX(const unsigned short &adress)
{
	X = memory_read(adress);

	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//Load Y Register - ³aduje do Rejestru Y zawartoœæ pamiêci, której adres zosta³ podany jako operand
void CPU::LDY(const unsigned short &adress)
{
	Y = memory_read(adress);

	flag.N = (Y & 0x80);
	flag.Z = !(Y);
}


//Logical Shift Right - przesuwa bity znajduj¹ce siê w pamiêci pod pobranym adresem o jedn¹ pozycjê w prawo. Pierwszy bit przed przesuniêciem(bit 0) zostaje zapisany w Carry Flag.
void CPU::LSR(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	flag.C = (memory_value & 0x01);
	memory_value >>= 1;
	memory_write(adress, memory_value);
	flag.Z = !(memory_value);
	flag.N = (memory_value & 0x80);
}

void CPU::LSR_accumulator()
{
	flag.C = (A & 0x01);
	A >>= 1;
	flag.Z = !(A);
	flag.N = (A & 0x80);
}


//No operation - ten opcode nie robi nic poza zwiêkszeniem licznika PC
void CPU::NOP()
{
	//Puste
}

//Logigal Inclusive OR - wykonuje na akumulatorze dzia³anie OR, wykorzystuj¹c przy tym zawartoœæ pobran¹ z pamiêci o podanym adresie
void CPU::ORA(const unsigned short &adress)
{
	A |= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}


//Push Accumulator - odk³ada wartoœæ przechowywan¹(jej kopiê) w akumulatorze na stos
void CPU::PHA()
{
	*stack[SP] = A;
	--SP;
}

//Push Processor Status - odk³ada na stos kopiê statusów procesora(odk³ada bit, w którym zawarte s¹ wszystkie flagi np.Carry, Negative)
void CPU::PHP()
{
	//One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. In fact, that's the only way a program can tell a BRK from /IRQ.


	const unsigned char status = (flag.N << 7) + (flag.V << 6) + (1 << 5) + (flag.B << 4) + (flag.D << 3) + (flag.I << 2) + (flag.Z << 1) + (flag.C * 1);//Tworzy status - w emulatorze flagi s¹ przechowywane nie w postaci jednego bajtu, a jako wartoœci bool
	
	 //One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. In fact, that's the only way a program can tell a BRK from /IRQ.
	 //Bit 4(w statusie": Clear if interrupt vectoring, set if BRK or PHP
	*stack[SP] = (status | 0x10);
	--SP;
}

//Pull Accumulator - pobera wartoœc ze stosu, nastêpnie przypisuje j¹ do akumulator
void CPU::PLA()
{
	++SP;
	A = *stack[SP];
	flag.Z = !A;
	flag.N = (A & 0x80);
}

//Pull Processor Status - pobiera ze stosu jeden bajt, nastêpnie przypisuje go do statusów procesora
void CPU::PLP()
{
	++SP;
	const unsigned char status = *stack[SP];

	flag.N = (status & 0x80);
	flag.V = (status & 0x40);
	flag.B = false;//(status & 0x10);	//Bit 4 (flaga Break Command) wystêpuje w NESie jedynie na stosie, jeœli zostanie wywo³ana instrukcja PLP lub BRK. Nigdzie indziej.
	flag.D = (status & 0x08);
	flag.I = (status & 0x04);
	flag.Z = (status & 0x02);
	flag.C = (status & 0x01);
}

//Rotate Left - przesuwa bity w pamiêci lub akumulatorze o jedno miejsce w lewo. Bit 0 jest zape³niany wartoœci¹ Carry Flag, a stary bit 7 zostaje now¹ wartoœci¹ Carry Flag
void CPU::ROL(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const bool Carry_value = flag.C;
	flag.C = (memory_value & 0x80);
	memory_value <<= 1;
	memory_write(adress, memory_value);
	if ((memory_value & 0x01) != Carry_value)
	{
		if (Carry_value)
		{
			memory_value |= 0x01;
			memory_write(adress, memory_value);
		}
		else
		{
			memory_value &= 0xFE;
			memory_write(adress, memory_value);
		}
	}
	flag.Z = !(memory_value);
	flag.N = (memory_value & 0x80);
}

void CPU::ROL_accumulator()
{
	const bool Carry_value = flag.C;
	flag.C = (A & 0x80);
	A <<= 1;
	if ((A & 0x01) != Carry_value)
	{
		if (Carry_value)
			A |= 0x01;
		else
			A &= 0xFE;
	}
	flag.Z = !(A);
	flag.N = (A & 0x80);
}

//Rotate Right - przesuwa bity w pamiêci lub akumulatorze o jedno miejsce w prawo. Bit 7 jest zape³niany wartoœci¹ Carry Flag, a stary bit 0 zostaje now¹ wartoœci¹ Carry Flag
void CPU::ROR(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const bool Carry_value = flag.C;
	flag.C = (memory_value & 0x01);
	memory_value >>= 1;
	memory_write(adress, memory_value);

	if ((*memory[adress] & 0x80) != Carry_value)
	{
		
		if (Carry_value)
		{
			memory_value |= 0x80;
			memory_write(adress, memory_value);
		}
		else
		{
			memory_value &= 0x7f;
			memory_write(adress, memory_value);
		}
	}
	flag.Z = !(memory_value);
	flag.N = (memory_value & 0x80);
}

void CPU::ROR_accumulator()
{
	const bool Carry_value = flag.C;
	flag.C = (A & 0x01);
	A >>= 1;

	if ((A & 0x80) != Carry_value)
	{
		if (Carry_value)
			A |= 0x80;
		else
			A &= 0x7F;
	}
	flag.Z = !(A);
	flag.N = (A & 0x80);
}

//Return from Interrupt - jest wykorzystywane na koniec przerwania. Pobiera flagi procesora ze stosu, a nasêpnie Program Counter
void CPU::RTI()
{
	++SP;
	const unsigned char status = *stack[SP];

	flag.N = (status & 0x80);
	flag.V = (status & 0x40);
	flag.B = false;	//Bit 4 (flaga Break Command) wystêpuje w NESie jedynie na stosie, jeœli zostanie wywo³ana instrukcja PLP lub BRK. Nigdzie indziej.
	flag.D = (status & 0x08);
	flag.I = (status & 0x04);
	flag.Z = (status & 0x02);
	flag.C = (status & 0x01);

	++SP;
	const unsigned char low_byte = *stack[SP];
	++SP;
	const unsigned char high_byte = *stack[SP];

	PC = ((high_byte << 8) | low_byte);

}

//Return from Subroutine - ta instrukcja jest u¿ywana na koniec 'podzadania', do powrotu do poprzedniego miejsca programu.
//Jej zadaniem jest pobranie ze stosu wartoœci 16bit, a nastêpnie przypisanie do Program Counter tej wartoœci powiêkszonej o 1 -> PC = (wartoœæ + 1)
//Dodawanie 1 do PC wynika z instrukcji JMP, która wrzuca na stos (PC - 1), zamiast samego PC;
void CPU::RTS()
{
	++SP;
	const unsigned char low_byte = *stack[SP];
	++SP;
	const unsigned char high_byte = *stack[SP];

	const unsigned adress = ((high_byte << 8) | low_byte);
	PC = adress + 1;
}


//Substract with Carry - odejmuje od akumulatora wartosc pobran¹ z pamiêæi razem z Carry Flag(A = A - flag.C - *memory[adress])
void CPU::SBC(const unsigned short &adress)
{
	//SBC zosta³o zaimplementowane jako ADC, w którym wartoœæ pobrana z pamiêci zosta³a "potraktowana" XORem przy u¿yciu 0xFF.
	//Dzia³a to dziêki temu, ¿e przy dzia³aniu (*memory[adress] ^ 0xFF), liczba zmienia znak(w tym przypadku na ujemny), , a jej wartosc absolutna jest wiêksza o jeden od wczesniejszej wartosci absolutnej.
	//Np.  |*memory[adress] + 1| =   |(memory[adress] ^ 0xFF)|
	//Proœciej-> SBC(MEMORY) = ADC((MEMORY ^ 0xFF))

	unsigned char memory_value = memory_read(adress);
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)(memory_value ^ 0xFF) + flag.C;
	flag.V = ((A ^ sum) & ((memory_value ^ 0xFF) ^ sum) & 0x80);//Kod skopiowany z internetu, ale dzia³a
														  //
	A += flag.C + (memory_value ^ 0xFF);
	flag.C = sum >> 8;

	flag.N = (A & 0x80);
	flag.Z = !A;
}


//Set Carry Flag - ustawia Carry Flag(na wartoœæ 1)
void CPU::SEC()
{
	flag.C = 1;
}

//Set Decimal Flag - ustawia Decimal Flag(na wartoœæ 1)
void CPU::SED()
{
	flag.D = 1;
}

//Set Interrupt Disable - ustawia Disable Interrupt Flag (na wartoœæ 1)
void CPU::SEI()
{
	flag.I = 1;
}

//Store Accumulator - przechowuje akumulator w pamiêci pod pobranym adresem
void CPU::STA(const unsigned short &adress)
{
	memory_write(adress, A);
}

//Store X Register - przechowuje Rejestr X w pamiêci pod pobranym adresem
void CPU::STX(const unsigned short &adress)
{
	memory_write(adress, X);
}


//Store Y Register - przechowuje Rejestr Y w pamiêci pod pobranym adresem
void CPU::STY(const unsigned short &adress)
{
	memory_write(adress, Y);
}

//Transfer Accumulator to X - kopiuje zawartoœæ akumulatora do Rejestru X
void CPU::TAX()
{
	X = A;
	flag.N = (X & 0x80);
	flag.Z = !X;
}

//Transfer Accumulator to Y - kopiuje zawartoœæ akumulatora do Rejestru Y
void CPU::TAY()
{
	Y = A;
	flag.N = (Y & 0x80);
	flag.Z = !Y;
}

//Transfer Stack Pointer to X - kopiuje zawartoœæ Stack Pointer do Rejestru X
void CPU::TSX()
{
	X = SP;//(0x1F & SP);
	flag.N = (X & 0x80);
	flag.Z = !X;
}

//Transfer X to Accumulator - kopiuje zawartoœæ Rejestru X do akumulatora
void CPU::TXA()
{
	A = X;
	flag.N = (A & 0x80);
	flag.Z = !A;
}

//Transfer X to Stack Pointer - kopiuje zawartoœæ Rejestru X do Stack Pointer
void CPU::TXS()
{
	SP = X;
}

//Transfer Y to Accumulator - kopuje zawartoœæ Rejestru Y do akumulatora
void CPU::TYA()
{
	A = Y;
	flag.N = (A & 0x80);
	flag.Z = !A;
}




//****************************************************************************************************
//------------------------------------------ILLEGAL  OPCODES------------------------------------------
//****************************************************************************************************


//AAC - wykonuje dzia³anie AND na akumulatorze, wykorzystuj¹c zawartoœæ pamiêci o pobranym adresie. Jeœli wynik jest ujemny(ostatni bit to 1), ustawia Carry Flag(na wartoœæ 1)
void CPU::AAC(const unsigned short &adress)//ANC
{
	A &= memory_read(adress);
	flag.Z = !(A);
	flag.C = A & 0x80;
	flag.N = A & 0x80;
}

//AAX - wykonuje AND na Rejestrze X i akumulatorze, wynik przechowuje w pamiêci pod pobranym adresem
void CPU::AAX(const unsigned short &adress)//SAX, AXS
{
	memory_write(adress, X & A);
	//flag.N = (0x80 & (*memory[adress]));
	//flag.Z = !(*memory[adress]);
}


//ARR - wykonuje AND na akumulatorze, przy u¿yciu zawartoœci pamiêci z pobranego adresu. Nastêpnie "obraca bity" w prawo o jedn¹ pozycje, sprawdza bity 5 i 6, zapisuj¹c wyniki we flagach;
void CPU::ARR(const unsigned short &adress)
{
	A &= memory_read(adress);
	A >>= 1;
	A |= (flag.C ? 0x80 : 0x0);		//Wykonuje "obrót" bitu - przenosi poprzednio pierwszy bit na koniec
	
	//Check bit 5 and 6:
	//Bit 5 = 0x20 Bit 6 = 0x40
	//If both bits are 1: set C, clear V.
	//If both bits are 0 : clear C and V.
	//If only bit 5 is 1 : set V, clear C.
	//If only bit 6 is 1 : set C and V.
	flag.C = (A & 0x40);	//C is bit 6
	flag.V = (A & 0x40 ? 0x01 : 0x00) ^ (A & 0x20 ? 0x01 : 0x00);	//V is bit 6 xor bit 5
	flag.N = (A & 0x80);
	flag.Z = !(A);
}

//ASR - wykonuje bitowe AND na akumulatoze za pomoc¹ zawartoœci pamiêci z pobranego adresu. Nastêpnie przesuwa zawartosc akumulatora o jeden bit w prawo
void CPU::ASR(const unsigned short &adress)//ALR
{
	A &= memory_read(adress);
	flag.C = (A & 0x01);
	A >>= 1;
	flag.N = (A & 0x80);
	flag.Z = !(A);
}

//ATX - wykonuje na akumulatorze bitowe AND, wykorzystuj¹c do tego zawartosc pamieci z pobranego adresu. Nastêpnie nosi zawartosc akumulatora do Rejestru X
void CPU::ATX(const unsigned short &adress)//LXA, OAL
{
	A &= memory_read(adress);
	X = A;
	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//AXA - wykonuje na rejestrze X bitowe AND, wykorzystuj¹c do tego akumulator. Nastêpnie wykonuje na wyniku tego dzia³ania nastêpne AND, wykorzystuj¹c do tego 7. Wynik zapisuje w pamiêci
void CPU::AXA(const unsigned short &adress)//SHA
{
	X &= A;
	memory_write(adress, X & 0x7);
}


//AXS - wykonuje bitowe AND na Rejestrze X, wykorzystuj¹c akumulator. Nastêpnie odejmuje od Rejestru X wartoœæ pobran¹ z pamiêci
void CPU::AXS(const unsigned short &adress)	//SBX, SAX
{
	unsigned char memory_value = memory_read(adress);
	X &= A;
	flag.C = (X >= memory_value);//Performs CMP and DEX at the same time, so that the MINUS sets the flag like CMP, not SBC
	X -= memory_value;
	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//DCP - odejmuje 1 od pamiêci, której adres zosta³ podany jako operand
//------------------------------------------------------------------------
//This opcode DECs the contents of a memory location and then CMPs the result 
//with the A register.

void CPU::DCP(const unsigned short &adress)//DCM
{
	unsigned char memory_value = memory_read(adress);
	memory_value -= 1;
	memory_write(adress, memory_value);
	//Skopiowane z CMP:
	const unsigned char value = A - memory_value;
	flag.C = (A >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//DOP - ta instrukcja nic nie robi, jej jednym dzia³aniem jest zwiêkszenie Program Counter i cycles
void CPU::DOP(const unsigned short &adress)//NOP, SKB
{
	//Puste
}


//ISC - podnosi zawartosc pamiêci o 1, nastêpnie odejmuje od akumulatora wartoœæ tej pamiêci(with borrow)
void CPU::ISC(const unsigned short &adress)//ISB, INS
{
	unsigned char memory_value = memory_read(adress);
	memory_value += 1;
	memory_write(adress, memory_value);
	//Czêœæ instrukcji skopiowana z SBC:
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)(memory_value ^ 0xFF) + flag.C;
	flag.V = ((A ^ sum) & ((memory_value ^ 0xFF) ^ sum) & 0x80);
	A += flag.C + (memory_value ^ 0xFF);
	flag.C = sum >> 8;

	flag.N = (A & 0x80);
	flag.Z = !A;
}

//KIL - zatrzymuje procesor. Wprowadza go w nieskoñczon¹ pêtlê, z której mo¿na wyjœæ jedynie przez reset
void CPU::KIL()//JAM, HLT
{
	//Puste - KIL wystêpuje jedynie w trybie adresowania Implied, wiêc nie trzeba nic robiæ. ¯eby wprowadziæ procesor w pêtle PC nie bêdzie zwiêkszany.
}


//LAR - wykonuje bitowe and na pamiêci, przy u¿yciu Stack Pointer, przenosi wynik do akumulatora, Rejestru X i Stack Pointer
void CPU::LAR(const unsigned short &adress)//LAE, LAS
{
	X = A = SP = SP & (memory_read(adress));
	flag.N = (A & 0x80);
	flag.Z = !(A);
}

//LAX - "zape³nia" akumulator i Rejestr X wartoœci¹ pobran¹ z pamiêci o podanym adresie
void CPU::LAX(const unsigned short &adress)
{
	A = X = memory_read(adress);
	flag.Z = !(A);
	flag.N = (A & 0x80);
}

//NOP - ta funkcja wystepuje nastêpny raz, jej dzia³anie jest takie samo jak w przypadku "legalnej" wersji, wiêc nie ma potrzeby implementacji tej funkcji


//RLA - obraca zawartoœc pamiêci o jeden bit w lewo, nastêpnie wykonuje bitowe AND na akumulatorze, za pamoc¹ zawartoœci pamiêci
void CPU::RLA(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const bool bit_value =  memory_value >> 7;
	
	memory_value <<= 1;
	memory_value |= flag.C;
	memory_write(adress, memory_value);

	A &= memory_value;
	flag.C = bit_value;
	flag.Z = !(A);
	flag.N = (A & 0x80);
}

//RRA - "obraca" zawartosc pamieci o jeden bit w prawo, nastêpnie dodaje t¹ zawartosc do akumulator(with Carry). Wykonuje najpierw ROR, potem ADC. 
void CPU::RRA(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const bool Carry_value = flag.C;
	flag.C = memory_value & 0x01;
	memory_value >>= 1;
	memory_value |= (Carry_value ? 0x80 : 0x00);
	memory_write(adress, memory_value);


	const unsigned short result = (const unsigned short&)A + (const unsigned short&) memory_value + flag.C;
	
	flag.V = ~(A ^ memory_value) & (A ^ result) & 0x80;//Kod bezczelnie ukradziony z kodu Ÿród³owego nintendulatora
	flag.C = result & 0x100;	//Kod bezczelnie ukradziony z kodu Ÿród³owego nintendulatora

	A = result;
	flag.Z = !A;
	flag.N = (A & 0x80);
}


//SBC - to taki sam opcode, jak legalny SBC. Robi to Samo
//Nie zamieszczam tutaj kodu, poniewa¿ nie jest to potrzebne


//SLO - przesuwa w lewo zawartoœæ pamiêci o jeden bit, nastêpnie wykonuje bitowe OR na akumulatorze, za pomoc¹ zawartoœci pamiêci
void CPU::SLO(const unsigned short &adress)//ASO
{
	unsigned char memory_value = memory_read(adress);
	flag.C = memory_value  >> 7;
	memory_value <<= 1;
	memory_write(adress, memory_value);
	A |= memory_value;
	flag.N = (A & 0x80);
	flag.Z = !A;
}


//SRE - przesuwa o jeden bit w prawo zawartoœæ pamiêci, nastêpnie za pomoc¹ tej pamiêci wykonuje XOR na akumulatorze
void CPU::SRE(const unsigned short &adress)//LSE
{
	unsigned char memory_value = memory_read(adress);
	flag.C = memory_value & 0x01;
	memory_value >>= 1;
	memory_write(adress, memory_value);
	A ^= memory_value;
	flag.Z = !(A);
	flag.N = (A & 0x80);
}



//SXA - wykonuje bitowe AND na Rejestrze X przy pomocy najbardziej znacz¹cych bitów adresu argumentu + 1. Wynik zapisuje w pamiêci pod podanym adresem(MEMORY = X &(MEMORY_ADRESS_HIGH_BYTE + 1))
void CPU::SXA(const unsigned short &adress)//SHX, XAS
{
	memory_write(adress, X & (((adress & 0xFF00) >> 8) + 1));
}

//SHX - memory = X AND((address >> 8) + 1)
//https://forums.nesdev.com/viewtopic.php?f=3&t=3831&start=30
void CPU::SHX(const unsigned short& adress)
{
	if (checkPageCrossed(adress, PC))
		* memory[((X & ((adress >> 8) + 1)) << 8) | adress & 0xFF] = X & ((adress >> 8) + 1);
	else
		*memory[(((adress >> 8) + 1) << 8) | adress & 0xFF] = X & ((adress >> 8) + 1);
}

//SHY - memory = X AND((address >> 8) + 1)
//https://forums.nesdev.com/viewtopic.php?f=3&t=3831&start=30
void CPU::SHY(const unsigned short& adress)
{
	if (checkPageCrossed(adress, PC))
		* memory[((Y & ((adress >> 8) + 1)) << 8) | adress & 0xFF] = Y & ((adress >> 8) + 1);
	else
		*memory[(((adress >> 8) + 1) << 8) | adress & 0xFF] = Y & ((adress >> 8) + 1);
}


//SYA - wykonuje bitowe AND na Rejestrze Y przy pomocy najbardziej znacz¹cych bitów adresu argumentu + 1. Wynik zapisuje w pamiêci pod podanym adresem(MEMORY = Y &(MEMORY_ADRESS_HIGH_BYTE + 1))
void CPU::SYA(const unsigned short &adress)//SHY, SAY
{
	memory_write(adress, Y & (((adress & 0xFF00) >> 8) + 1));
}

//TOP - nie robi nic poza zwiêkszaniem PC i cycles
void CPU::TOP(const unsigned short &adress)//NOP, SKW
{
	//Puste
}


//XAA, ANE - do zainmplementowania


//XAS - wykonuje bitowe AND na Rejestrze X i akumulatorze, a Wynik przechowuje w Stack Pointer. 
//Nastêpnie wykonuje bitowe AND na Stack Pointer za pomoc¹ najbardziej znacz¹cych bajtów adresu podanego jako argument + 1
//SP = X & A; M = S & (MEMORY_HIGH_BYTE + 1)
void CPU::XAS(const unsigned short &adress)
{
	SP = X & A;
	memory_write(adress, SP & (((adress & 0xFF00) >> 8) + 1));
}

