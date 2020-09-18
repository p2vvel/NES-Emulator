#include "PPU.h"
#include "CPU.h"

void PPU::write_2000(const unsigned char& value)
{
	unsigned char old_value = *reg.PPUCTRL;
	*reg.PPUCTRL = value;

	//t: ...BA.. ........ = d: ......BA
	VRAM_address_temp &= ~0x0C00;
	VRAM_address_temp |= (unsigned short)(value & 0x3) << 10;


	NMI_output = value & 0x80;


	//If the PPU is currently in vertical blank, and the PPUSTATUS ($2002) vblank flag is still set (1), changing the NMI flag in bit 7 of $2000 from 0 to 1 will immediately generate an NMI
	if ((*reg.PPUSTATUS & 0x80) && !(old_value & 0x80) && (*reg.PPUCTRL & 0x80))
			cpu->set_NMI(1);

	//Pisanie do tego rejestru w pobli¿u cyklu rozpoczynaj¹cego VBlank powoduje "rozbrojenie" NMI
	if (scanline == 241 && cycles < 4 && *reg.PPUSTATUS & 0x80)
		cpu->unset_NMI();
}

const unsigned char PPU::read_2002()
{
	write_toggle = false;


	unsigned char result = (NMI_occured ? 0x80 : 0x00) | (*reg.PPUSTATUS & 0x7f); //Return old status of NMI_occurred in bit 7
	NMI_occured = false;


	*reg.PPUSTATUS &= 0x7F;


	if (scanline == 241)
	{
		switch (cycles)
		{
		case 1:
			result &= 0x7f;
		case 2: case 3:
			cpu->unset_NMI();
		}
	}
	else if (scanline == 261)
		if (cycles == 1)
			result |= 0x80;



	return result;
}

const unsigned char PPU::read_2004()
{
	unsigned char result = OAM[*reg.OAMADDR];

	if (scanline >= 0 && scanline <= 239 && cycles >= 1 && cycles <= 64)	//Cycles 1-64: (...) attempting to read $2004 will return $FF
		result = 0xff;

	return result;
}

void PPU::write_2004(const unsigned char& value)
{
	unsigned char temp = value;
	if (RENDERING_ENABLED)
		temp = 0xff;
	if ((*reg.OAMADDR & 0x03) == 0x02)
		temp &= 0xe3;
	OAM[*reg.OAMADDR] = temp;
	*reg.OAMADDR += 1;
}

void PPU::write_2005(const unsigned char& value)
{
	if (!write_toggle)
	{
		VRAM_address_temp &= ~0x001F;
		VRAM_address_temp |= (value >> 3);

	
		write_toggle = true;

		//X scroll do zrobienia!
		x_scroll = value & 0x07;	//x: CBA = d: .....CBA
	}
	else
	{
		VRAM_address_temp &= ~0x73E0;
		VRAM_address_temp |= (unsigned short)(value & 0x07) << 12;
		VRAM_address_temp |= (unsigned short)(value & 0xF8) << 2;


		write_toggle = false;
	}
}

void PPU::write_2006(const unsigned char& value)
{
	if (!write_toggle)
	{
		VRAM_address_temp = (VRAM_address_temp & ~0xFF00) | (const unsigned short &)((value & 0x3F) << 8);

		write_toggle = true;
	}
	else
	{
		VRAM_address_temp = (VRAM_address_temp & ~0x00FF) | value;

		VRAM_address = VRAM_address_temp;

		write_toggle = false;
	}
}

void PPU::write_2007(const unsigned char& value)
{
	//Note that while the v register has 15 bits, the PPU memory space is only 14 bits wide. The highest bit is unused for access through $2007!
	*memory[VRAM_address & 0x3fff] = value;
	
	
	
	if (!RENDERING_ENABLED || scanline >= 239)
		increment_address();
	else
	{
		increment_address_horizontal();
		increment_address_vertical();
	}
}

const unsigned char PPU::read_2007()
{
	//Note that while the v register has 15 bits, the PPU memory space is only 14 bits wide. The highest bit is unused for access through $2007!


	unsigned char result;

	if ((VRAM_address & 0x3fff) < 0x3f00)
	{
		result = internal_buffer_2007;//When reading while the VRAM address is in the range 0-$3EFF, the read will return the contents of an internal read buffer
		internal_buffer_2007 = *memory[VRAM_address & 0x3fff];	//After the CPU reads and gets the contents of the internal buffer, the PPU will immediately update the internal buffer with the byte at the current VRAM address
	}
	else
	{
		internal_buffer_2007 = *memory[0x2fff & VRAM_address];	//IE: reading $3F00 will give you the palette entry at $3F00 and will put the byte in VRAM[$2F00] in the read buffer: http://forums.nesdev.com/viewtopic.php?t=1721
		result =  *memory[VRAM_address & 0x3fff];
	}

	if (!RENDERING_ENABLED || scanline >= 240)
		increment_address();
	else
	{
		increment_address_horizontal();
		increment_address_vertical();
	}

	return result;
}
