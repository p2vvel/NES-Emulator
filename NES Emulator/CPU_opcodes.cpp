#include "CPU.h"
#include<iostream>




//Add With Carry - dodaje do akumulatora warto�� pobran� z pami�ci i flag� Carry.
void CPU::ADC(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)memory_value + flag.C;
	flag.V = ((A ^ sum) & (memory_value ^ sum) & 0x80);//Kod skopiowany z internetu, ale dzia�a
	
	A += flag.C + memory_value;
	flag.C = sum >> 8;
	
	flag.N = (A & 0x80);
	flag.Z = !A;
}


//Logical AND - wykonuje bitowe dzia�anie AND na akumulatorze, wykorzystuj�c warto�� pobran� z pami�ci
void CPU::AND(const unsigned short &adress)
{
	A &= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}

//Arithmetic Shift Left - przesuwa bity o jedn� pozycj� w lewo. Ostatni bit jest zapisywany we fladze Carry
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


//Branch if Carry Clear - przenosi program w inne miejsce pami�ci, je�li Carry Flag jest czyste
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

//Branch if Carry Set - przenosi program w inne miejsce pami�ci je�li Carry Flag jest ustawione
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

//Branch if Equal - przenosi program w inne miejsce, je�li Zero Flag jest ustawione
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


//Bit Test - wykonuje dzia�anie bitowe AND na warto�ci z pami�ci i akumulatorze, ale nie zapisuje ich nigdzie. Przechowuje jedynie warto�ci ostatniego i przedostatniego bitu
//tego dzia�ania kolejno we flagach Negative i Overflow.
void CPU::BIT(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	flag.N = (memory_value & 0x80);
	flag.V = (memory_value & 0x40);
	flag.Z = !(A & memory_value);
}



//Branch if Minus - przenosi program w inne miejsce pami�ci je�li Negative Flag jest ustawione
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

//Branch if Not Equal - przenosi program w inne miejsce pami�ci je�li Zero Flag jest czyste
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

//Branch if Positive - przenosi program w inne miejsce w pami�ci je�li Negative Flag jest czyste
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

//Force Interrupt - wymusza na procesorze wygenerowania ��danie przerwania. Program Counter i statusy procesora s� odk�adane na stos.
//Nast�pnie flaga Break Command jest ustawiana, a do Program Counter �adowana jest zawarto�� z pami�ci 0xFFFE - 0xFFFF.
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

	*stack[SP] = ((PC & 0xFF00) >> 8);//Najpierw najbardziej znacz�ce bity z PC, przy �ci�ganiu ich ze stosu kolejno�� b�dzie odwrotna(Little Endian)
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

//Branch if Overflow Clear - przenosi program w inne miejsce, je�li Overflow Flag jest czyste
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

//Branch if Overflow Set - przenosi program w inne miejsce w pami�ci, je�li Overflow Flag jest ustawione
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

//Clear Carry Flag - czy�ci Carry Flag(ustawia 0)
void CPU::CLC()
{
	flag.C = false;
}

//Clear Decimal Mode - czy�ci Decimal Mode(ustawia na 0)
void CPU::CLD()
{
	//The state of the decimal flag is uncertain when the CPU is powered up and it is not reset when an interrupt is generated. 
	//In both cases you should include an explicit CLD to ensure that the flag is cleared before performing addition or subtraction.
	flag.D = false;
}

//Clear Interrupt Disable - Czy�ci Interrupt Disable(ustawia na 0)
void CPU::CLI()
{
	flag.I = false;
}

//Clear Overflow Flag - czy�ci Overflow Flag(ustawia na 0)
void CPU::CLV()
{
	flag.V = false;
}

//Compare - por�wnuje warto�� akuulatora z warto�ci� povran� z pami�ci, wynik zapisuje we flagach
void CPU::CMP(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = A - memory_value;
	flag.C = (A >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Compare X Register - por�wnuje warto�� Rejestru X z warto�ci� povran� z pami�ci, wynik zapisuje we flagach
void CPU::CPX(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = X - memory_value;
	flag.C = (X >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Compare Y Register - por�wnuje warto�� Rejestru Y z warto�ci� povran� z pami�ci, wynik zapisuje we flagach
void CPU::CPY(const unsigned short &adress)
{
	const unsigned char memory_value = memory_read(adress);

	const unsigned char value = Y - memory_value;
	flag.C = (Y >= memory_value);
	flag.Z = !value;
	flag.N = (value & 0x80);
}


//Decrement Memory - odejmuje 1 od wartosci znajdujacej sie w pami�ci
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

//Exclusive OR - wykonuje bitow� operacj� XOR na akumulatorze, przy wykorzystaniu pobranej warto�ci z pami�ci
void CPU::EOR(const unsigned short &adress)
{
	A ^= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}



//Increment Memory - dodaje 1 do wartosci z pami�ci
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


//Jump - ustawia Program counter na wartosc podan� w argumencie
void CPU::JMP(const unsigned short &adress)
{
	PC = adress;
}


//Jump to Subroutine - odk�ada adres "powrotny"(PC - 1) na stos, nast�pnie ustawia adres podany jako operand jako PC
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


//Load Accumulator - �aduje do akumulatora zawarto�� pami�ci, kt�rej adres zosta� podany jako operand
void CPU::LDA(const unsigned short &adress)
{
	A = memory_read(adress);

	flag.N = (A & 0x80);
	flag.Z = !(A);
}


//Load X Register - �aduje do Rejestru X zawarto�� pami�ci, kt�rej adres zosta� podany jako operand
void CPU::LDX(const unsigned short &adress)
{
	X = memory_read(adress);

	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//Load Y Register - �aduje do Rejestru Y zawarto�� pami�ci, kt�rej adres zosta� podany jako operand
void CPU::LDY(const unsigned short &adress)
{
	Y = memory_read(adress);

	flag.N = (Y & 0x80);
	flag.Z = !(Y);
}


//Logical Shift Right - przesuwa bity znajduj�ce si� w pami�ci pod pobranym adresem o jedn� pozycj� w prawo. Pierwszy bit przed przesuni�ciem(bit 0) zostaje zapisany w Carry Flag.
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


//No operation - ten opcode nie robi nic poza zwi�kszeniem licznika PC
void CPU::NOP()
{
	//Puste
}

//Logigal Inclusive OR - wykonuje na akumulatorze dzia�anie OR, wykorzystuj�c przy tym zawarto�� pobran� z pami�ci o podanym adresie
void CPU::ORA(const unsigned short &adress)
{
	A |= memory_read(adress);
	flag.Z = !A;
	flag.N = (A & 0x80);
}


//Push Accumulator - odk�ada warto�� przechowywan�(jej kopi�) w akumulatorze na stos
void CPU::PHA()
{
	*stack[SP] = A;
	--SP;
}

//Push Processor Status - odk�ada na stos kopi� status�w procesora(odk�ada bit, w kt�rym zawarte s� wszystkie flagi np.Carry, Negative)
void CPU::PHP()
{
	//One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. In fact, that's the only way a program can tell a BRK from /IRQ.


	const unsigned char status = (flag.N << 7) + (flag.V << 6) + (1 << 5) + (flag.B << 4) + (flag.D << 3) + (flag.I << 2) + (flag.Z << 1) + (flag.C * 1);//Tworzy status - w emulatorze flagi s� przechowywane nie w postaci jednego bajtu, a jako warto�ci bool
	
	 //One thing that BRK and PHP do that /IRQ and /NMI don't is that BRK and PHP push the value of P OR #$10. In fact, that's the only way a program can tell a BRK from /IRQ.
	 //Bit 4(w statusie": Clear if interrupt vectoring, set if BRK or PHP
	*stack[SP] = (status | 0x10);
	--SP;
}

//Pull Accumulator - pobera warto�c ze stosu, nast�pnie przypisuje j� do akumulator
void CPU::PLA()
{
	++SP;
	A = *stack[SP];
	flag.Z = !A;
	flag.N = (A & 0x80);
}

//Pull Processor Status - pobiera ze stosu jeden bajt, nast�pnie przypisuje go do status�w procesora
void CPU::PLP()
{
	++SP;
	const unsigned char status = *stack[SP];

	flag.N = (status & 0x80);
	flag.V = (status & 0x40);
	flag.B = false;//(status & 0x10);	//Bit 4 (flaga Break Command) wyst�puje w NESie jedynie na stosie, je�li zostanie wywo�ana instrukcja PLP lub BRK. Nigdzie indziej.
	flag.D = (status & 0x08);
	flag.I = (status & 0x04);
	flag.Z = (status & 0x02);
	flag.C = (status & 0x01);
}

//Rotate Left - przesuwa bity w pami�ci lub akumulatorze o jedno miejsce w lewo. Bit 0 jest zape�niany warto�ci� Carry Flag, a stary bit 7 zostaje now� warto�ci� Carry Flag
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

//Rotate Right - przesuwa bity w pami�ci lub akumulatorze o jedno miejsce w prawo. Bit 7 jest zape�niany warto�ci� Carry Flag, a stary bit 0 zostaje now� warto�ci� Carry Flag
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

//Return from Interrupt - jest wykorzystywane na koniec przerwania. Pobiera flagi procesora ze stosu, a nas�pnie Program Counter
void CPU::RTI()
{
	++SP;
	const unsigned char status = *stack[SP];

	flag.N = (status & 0x80);
	flag.V = (status & 0x40);
	flag.B = false;	//Bit 4 (flaga Break Command) wyst�puje w NESie jedynie na stosie, je�li zostanie wywo�ana instrukcja PLP lub BRK. Nigdzie indziej.
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

//Return from Subroutine - ta instrukcja jest u�ywana na koniec 'podzadania', do powrotu do poprzedniego miejsca programu.
//Jej zadaniem jest pobranie ze stosu warto�ci 16bit, a nast�pnie przypisanie do Program Counter tej warto�ci powi�kszonej o 1 -> PC = (warto�� + 1)
//Dodawanie 1 do PC wynika z instrukcji JMP, kt�ra wrzuca na stos (PC - 1), zamiast samego PC;
void CPU::RTS()
{
	++SP;
	const unsigned char low_byte = *stack[SP];
	++SP;
	const unsigned char high_byte = *stack[SP];

	const unsigned adress = ((high_byte << 8) | low_byte);
	PC = adress + 1;
}


//Substract with Carry - odejmuje od akumulatora wartosc pobran� z pami��i razem z Carry Flag(A = A - flag.C - *memory[adress])
void CPU::SBC(const unsigned short &adress)
{
	//SBC zosta�o zaimplementowane jako ADC, w kt�rym warto�� pobrana z pami�ci zosta�a "potraktowana" XORem przy u�yciu 0xFF.
	//Dzia�a to dzi�ki temu, �e przy dzia�aniu (*memory[adress] ^ 0xFF), liczba zmienia znak(w tym przypadku na ujemny), , a jej wartosc absolutna jest wi�ksza o jeden od wczesniejszej wartosci absolutnej.
	//Np.  |*memory[adress] + 1| =   |(memory[adress] ^ 0xFF)|
	//Pro�ciej-> SBC(MEMORY) = ADC((MEMORY ^ 0xFF))

	unsigned char memory_value = memory_read(adress);
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)(memory_value ^ 0xFF) + flag.C;
	flag.V = ((A ^ sum) & ((memory_value ^ 0xFF) ^ sum) & 0x80);//Kod skopiowany z internetu, ale dzia�a
														  //
	A += flag.C + (memory_value ^ 0xFF);
	flag.C = sum >> 8;

	flag.N = (A & 0x80);
	flag.Z = !A;
}


//Set Carry Flag - ustawia Carry Flag(na warto�� 1)
void CPU::SEC()
{
	flag.C = 1;
}

//Set Decimal Flag - ustawia Decimal Flag(na warto�� 1)
void CPU::SED()
{
	flag.D = 1;
}

//Set Interrupt Disable - ustawia Disable Interrupt Flag (na warto�� 1)
void CPU::SEI()
{
	flag.I = 1;
}

//Store Accumulator - przechowuje akumulator w pami�ci pod pobranym adresem
void CPU::STA(const unsigned short &adress)
{
	memory_write(adress, A);
}

//Store X Register - przechowuje Rejestr X w pami�ci pod pobranym adresem
void CPU::STX(const unsigned short &adress)
{
	memory_write(adress, X);
}


//Store Y Register - przechowuje Rejestr Y w pami�ci pod pobranym adresem
void CPU::STY(const unsigned short &adress)
{
	memory_write(adress, Y);
}

//Transfer Accumulator to X - kopiuje zawarto�� akumulatora do Rejestru X
void CPU::TAX()
{
	X = A;
	flag.N = (X & 0x80);
	flag.Z = !X;
}

//Transfer Accumulator to Y - kopiuje zawarto�� akumulatora do Rejestru Y
void CPU::TAY()
{
	Y = A;
	flag.N = (Y & 0x80);
	flag.Z = !Y;
}

//Transfer Stack Pointer to X - kopiuje zawarto�� Stack Pointer do Rejestru X
void CPU::TSX()
{
	X = SP;//(0x1F & SP);
	flag.N = (X & 0x80);
	flag.Z = !X;
}

//Transfer X to Accumulator - kopiuje zawarto�� Rejestru X do akumulatora
void CPU::TXA()
{
	A = X;
	flag.N = (A & 0x80);
	flag.Z = !A;
}

//Transfer X to Stack Pointer - kopiuje zawarto�� Rejestru X do Stack Pointer
void CPU::TXS()
{
	SP = X;
}

//Transfer Y to Accumulator - kopuje zawarto�� Rejestru Y do akumulatora
void CPU::TYA()
{
	A = Y;
	flag.N = (A & 0x80);
	flag.Z = !A;
}




//****************************************************************************************************
//------------------------------------------ILLEGAL  OPCODES------------------------------------------
//****************************************************************************************************


//AAC - wykonuje dzia�anie AND na akumulatorze, wykorzystuj�c zawarto�� pami�ci o pobranym adresie. Je�li wynik jest ujemny(ostatni bit to 1), ustawia Carry Flag(na warto�� 1)
void CPU::AAC(const unsigned short &adress)//ANC
{
	A &= memory_read(adress);
	flag.Z = !(A);
	flag.C = A & 0x80;
	flag.N = A & 0x80;
}

//AAX - wykonuje AND na Rejestrze X i akumulatorze, wynik przechowuje w pami�ci pod pobranym adresem
void CPU::AAX(const unsigned short &adress)//SAX, AXS
{
	memory_write(adress, X & A);
	//flag.N = (0x80 & (*memory[adress]));
	//flag.Z = !(*memory[adress]);
}


//ARR - wykonuje AND na akumulatorze, przy u�yciu zawarto�ci pami�ci z pobranego adresu. Nast�pnie "obraca bity" w prawo o jedn� pozycje, sprawdza bity 5 i 6, zapisuj�c wyniki we flagach;
void CPU::ARR(const unsigned short &adress)
{
	A &= memory_read(adress);
	A >>= 1;
	A |= (flag.C ? 0x80 : 0x0);		//Wykonuje "obr�t" bitu - przenosi poprzednio pierwszy bit na koniec
	
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

//ASR - wykonuje bitowe AND na akumulatoze za pomoc� zawarto�ci pami�ci z pobranego adresu. Nast�pnie przesuwa zawartosc akumulatora o jeden bit w prawo
void CPU::ASR(const unsigned short &adress)//ALR
{
	A &= memory_read(adress);
	flag.C = (A & 0x01);
	A >>= 1;
	flag.N = (A & 0x80);
	flag.Z = !(A);
}

//ATX - wykonuje na akumulatorze bitowe AND, wykorzystuj�c do tego zawartosc pamieci z pobranego adresu. Nast�pnie nosi zawartosc akumulatora do Rejestru X
void CPU::ATX(const unsigned short &adress)//LXA, OAL
{
	A &= memory_read(adress);
	X = A;
	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//AXA - wykonuje na rejestrze X bitowe AND, wykorzystuj�c do tego akumulator. Nast�pnie wykonuje na wyniku tego dzia�ania nast�pne AND, wykorzystuj�c do tego 7. Wynik zapisuje w pami�ci
void CPU::AXA(const unsigned short &adress)//SHA
{
	X &= A;
	memory_write(adress, X & 0x7);
}


//AXS - wykonuje bitowe AND na Rejestrze X, wykorzystuj�c akumulator. Nast�pnie odejmuje od Rejestru X warto�� pobran� z pami�ci
void CPU::AXS(const unsigned short &adress)	//SBX, SAX
{
	unsigned char memory_value = memory_read(adress);
	X &= A;
	flag.C = (X >= memory_value);//Performs CMP and DEX at the same time, so that the MINUS sets the flag like CMP, not SBC
	X -= memory_value;
	flag.N = (X & 0x80);
	flag.Z = !(X);
}

//DCP - odejmuje 1 od pami�ci, kt�rej adres zosta� podany jako operand
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


//DOP - ta instrukcja nic nie robi, jej jednym dzia�aniem jest zwi�kszenie Program Counter i cycles
void CPU::DOP(const unsigned short &adress)//NOP, SKB
{
	//Puste
}


//ISC - podnosi zawartosc pami�ci o 1, nast�pnie odejmuje od akumulatora warto�� tej pami�ci(with borrow)
void CPU::ISC(const unsigned short &adress)//ISB, INS
{
	unsigned char memory_value = memory_read(adress);
	memory_value += 1;
	memory_write(adress, memory_value);
	//Cz�� instrukcji skopiowana z SBC:
	const unsigned short sum = (const unsigned short &)A + (const unsigned short &)(memory_value ^ 0xFF) + flag.C;
	flag.V = ((A ^ sum) & ((memory_value ^ 0xFF) ^ sum) & 0x80);
	A += flag.C + (memory_value ^ 0xFF);
	flag.C = sum >> 8;

	flag.N = (A & 0x80);
	flag.Z = !A;
}

//KIL - zatrzymuje procesor. Wprowadza go w niesko�czon� p�tl�, z kt�rej mo�na wyj�� jedynie przez reset
void CPU::KIL()//JAM, HLT
{
	//Puste - KIL wyst�puje jedynie w trybie adresowania Implied, wi�c nie trzeba nic robi�. �eby wprowadzi� procesor w p�tle PC nie b�dzie zwi�kszany.
}


//LAR - wykonuje bitowe and na pami�ci, przy u�yciu Stack Pointer, przenosi wynik do akumulatora, Rejestru X i Stack Pointer
void CPU::LAR(const unsigned short &adress)//LAE, LAS
{
	X = A = SP = SP & (memory_read(adress));
	flag.N = (A & 0x80);
	flag.Z = !(A);
}

//LAX - "zape�nia" akumulator i Rejestr X warto�ci� pobran� z pami�ci o podanym adresie
void CPU::LAX(const unsigned short &adress)
{
	A = X = memory_read(adress);
	flag.Z = !(A);
	flag.N = (A & 0x80);
}

//NOP - ta funkcja wystepuje nast�pny raz, jej dzia�anie jest takie samo jak w przypadku "legalnej" wersji, wi�c nie ma potrzeby implementacji tej funkcji


//RLA - obraca zawarto�c pami�ci o jeden bit w lewo, nast�pnie wykonuje bitowe AND na akumulatorze, za pamoc� zawarto�ci pami�ci
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

//RRA - "obraca" zawartosc pamieci o jeden bit w prawo, nast�pnie dodaje t� zawartosc do akumulator(with Carry). Wykonuje najpierw ROR, potem ADC. 
void CPU::RRA(const unsigned short &adress)
{
	unsigned char memory_value = memory_read(adress);
	const bool Carry_value = flag.C;
	flag.C = memory_value & 0x01;
	memory_value >>= 1;
	memory_value |= (Carry_value ? 0x80 : 0x00);
	memory_write(adress, memory_value);


	const unsigned short result = (const unsigned short&)A + (const unsigned short&) memory_value + flag.C;
	
	flag.V = ~(A ^ memory_value) & (A ^ result) & 0x80;//Kod bezczelnie ukradziony z kodu �r�d�owego nintendulatora
	flag.C = result & 0x100;	//Kod bezczelnie ukradziony z kodu �r�d�owego nintendulatora

	A = result;
	flag.Z = !A;
	flag.N = (A & 0x80);
}


//SBC - to taki sam opcode, jak legalny SBC. Robi to Samo
//Nie zamieszczam tutaj kodu, poniewa� nie jest to potrzebne


//SLO - przesuwa w lewo zawarto�� pami�ci o jeden bit, nast�pnie wykonuje bitowe OR na akumulatorze, za pomoc� zawarto�ci pami�ci
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


//SRE - przesuwa o jeden bit w prawo zawarto�� pami�ci, nast�pnie za pomoc� tej pami�ci wykonuje XOR na akumulatorze
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



//SXA - wykonuje bitowe AND na Rejestrze X przy pomocy najbardziej znacz�cych bit�w adresu argumentu + 1. Wynik zapisuje w pami�ci pod podanym adresem(MEMORY = X &(MEMORY_ADRESS_HIGH_BYTE + 1))
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


//SYA - wykonuje bitowe AND na Rejestrze Y przy pomocy najbardziej znacz�cych bit�w adresu argumentu + 1. Wynik zapisuje w pami�ci pod podanym adresem(MEMORY = Y &(MEMORY_ADRESS_HIGH_BYTE + 1))
void CPU::SYA(const unsigned short &adress)//SHY, SAY
{
	memory_write(adress, Y & (((adress & 0xFF00) >> 8) + 1));
}

//TOP - nie robi nic poza zwi�kszaniem PC i cycles
void CPU::TOP(const unsigned short &adress)//NOP, SKW
{
	//Puste
}


//XAA, ANE - do zainmplementowania


//XAS - wykonuje bitowe AND na Rejestrze X i akumulatorze, a Wynik przechowuje w Stack Pointer. 
//Nast�pnie wykonuje bitowe AND na Stack Pointer za pomoc� najbardziej znacz�cych bajt�w adresu podanego jako argument + 1
//SP = X & A; M = S & (MEMORY_HIGH_BYTE + 1)
void CPU::XAS(const unsigned short &adress)
{
	SP = X & A;
	memory_write(adress, SP & (((adress & 0xFF00) >> 8) + 1));
}

