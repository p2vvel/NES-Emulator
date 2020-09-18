#include "PPU.h"
#include "CPU.h"
#include "Screen.h"

#define STARY_SPRITE_EVALUATION

//#define NINTENULATOR_LIKE_FRAME_LENGTH





PPU::PPU()
{
	scanline = 241;
	cycles = 0;			//Patrz:
	total_cycles = 0;	//przy pierwszym przejsciu przez pętle przyjmie wartosc 0
	frame = 0;
	initialize_memory();

	output_attribute_table = 0;
	output_pattern_table_low = 0;
	output_pattern_table_high = 0;

	write_toggle = false;
	ready_to_render = false;
	DMA_activated = false;
}

PPU::~PPU()
{

}

void PPU::initialize_memory()
{
	//PPU Memory Map
	//Adress	Size	Description
	//-------------------------------------------
	//$0000 - $0FFF 	$1000 	Pattern table 0
	//$1000 - $1FFF 	$1000 	Pattern Table 1
	//$2000 - $23FF 	$0400 	Nametable 0
	//$2400 - $27FF 	$0400 	Nametable 1
	//$2800 - $2BFF 	$0400 	Nametable 2
	//$2C00 - $2FFF 	$0400 	Nametable 3
	//$3000 - $3EFF 	$0F00 	Mirrors of $2000 - $2EFF
	//$3F00 - $3F1F 	$0020 	Palette RAM indexes
	//$3F20 - $3FFF 	$00E0 	Mirrors of $3F00 - $3F1F

	for (int i = 0; i < 0x1000; i++)
	{
		memory[i] = &mem.pattern_table_0[i];
		memory[i + 0x1000] = &mem.pattern_table_1[i];
	}
	for (int i = 0; i < 0x400; i++)
	{
		memory[i + 0x2000] = &mem.nametable_0[i];
		memory[i + 0x2400] = &mem.nametable_1[i];
		memory[i + 0x2800] = &mem.nametable_2[i];
		memory[i + 0x2C00] = &mem.nametable_3[i];
	}
	for (int i = 0; i < 0xF00; i++)
	{
		memory[0x3000 + i] = memory[0x2000 + i];
	}
	for (int i = 0; i < 0x100; i++)
	{
		memory[0x3F00 + i] = &mem.palette_RAM_indexes[i % 0x20];
	}


	for (int x = 0; x < 0x4000; x++)
		* memory[x] = 0;

#ifdef DONKEY_KONG_PALETTE
	const unsigned char fake_palette[32] = { 0x0f, 0x15, 0x2c, 0x12, 0x0f, 0x27, 0x02, 0x17, 0x0f,  0x30,  0x36, 0x06, 0x0f, 0x30, 0x2c, 0x24,
								0x00, 0x02, 0x36, 0x16, 0x00, 0x30, 0x27, 0x24, 0x00, 0x16, 0x30, 0x37, 0x00, 0x06, 0x27, 0x02 };

	for (int i = 0; i < 0x20; i++)
		*memory[0x3F00 + i] = fake_palette[i];
#else
	const unsigned char power_up_palette_state[32] = {		//Żródło: https://forums.nesdev.com/viewtopic.php?t=567
		0x09,0x01,0x00,0x01,
		0x00,0x02,0x02,0x0D,
		0x08,0x10,0x08,0x24,
		0x00,0x00,0x04,0x2C,
		0x09,0x01,0x34,0x03,
		0x00,0x04,0x00,0x14,
		0x08,0x3A,0x00,0x02,
		0x00,0x20,0x2C,0x08
	};

	for (int i = 0; i < 0x20; i++)
		*memory[0x3F00 + i] = power_up_palette_state[i];//*/

#endif


	/*for (int i = 0x2000; i < 0x3000; i++)
		*memory[i] = (i % 32 );//*/

	

	/*const unsigned char power_up_palette_state[32] = {		//Żródło: https://forums.nesdev.com/viewtopic.php?t=567
		0x11, 0x12, 0x13, 0x14,
		0x15, 0x16, 0x17, 0x18,
		0x19, 0x1a, 0xb, 0x1c,
		0x1d, 0x0f, 0x0f, 0x0f,
		0x11, 0x0f, 0x0f, 0x0f,
		0x15, 0x0f, 0x0f, 0x0f,
		0x19, 0x0f, 0x0f, 0x0f,
		0x1d, 0x0f, 0x0f, 0x19
	};

	for (int i = 0; i < 0x20; i++)
		*memory[0x3F00 + i] = power_up_palette_state[i];//*/


}

void PPU::initialize_registers(CPU &cpu_src)
{
	//PPUCTRL		$2000 	VPHB SINN 	NMI enable(V), PPU master / slave(P), sprite height(H), background tile select(B), sprite tile select(S), increment mode(I), nametable select(NN)
	//PPUMASK 		$2001 	BGRs bMmG 	color emphasis(BGR), sprite enable(s), background enable(b), sprite left column enable(M), background left column enable(m), greyscale(G)
	//PPUSTATUS 	$2002 	VSO - ----vblank(V), sprite 0 hit(S), sprite overflow(O), read resets write pair for $2005 / 2006
	//OAMADDR		$2003 	aaaa aaaa 	OAM read / write address
	//OAMDATA		$2004 	dddd dddd 	OAM data read / write
	//PPUSCROLL		$2005 	xxxx xxxx 	fine scroll position(two writes : X, Y)
	//PPUADDR		$2006 	aaaa aaaa 	PPU read / write address(two writes : MSB, LSB)
	//PPUDATA		$2007 	dddd dddd 	PPU data read / write
	//OAMDMA		$4014 	aaaa aaaa 	OAM DMA high address

	reg.PPUCTRL = cpu_src.memory[0x2000];
	reg.PPUMASK = cpu_src.memory[0x2001];
	reg.PPUSTATUS = cpu_src.memory[0x2002];
	reg.OAMADDR = cpu_src.memory[0x2003];
	reg.OAMDATA = cpu_src.memory[0x2004];
	reg.PPUSCROLL = cpu_src.memory[0x2005];
	reg.PPUADDR = cpu_src.memory[0x2006];
	reg.PPUDATA = cpu_src.memory[0x2007];
	reg.OAMDMA = cpu_src.memory[0x4014];



	for (int i = 0; i < 0xFFFF; i++)
	{
		CPU_memory[i] = cpu_src.memory[i];
	}


	//Dodane później:

	VRAM_address = 0;
	VRAM_address_temp = 0;
	X_scroll = 0;
	write_toggle = false;
	for (int i = 0; i < 2; i++)
	{
		shift_registers_background_bitmap[i] = 0;
		shift_registers_background_attribute[i] = 0;
	}
	for(int i = 0; i < 256; i++)
		OAM[i] = 0xff;	//Przy zainicjalizowaniu wartością 0, wystąpi błąd przy pierwszym sprite evaluation
	for(int i = 0; i < 32; i++)
		secondary_OAM[i] = 0xff;
	for(int i = 0; i < 8; i++)
	{ 
		shift_registers_sprite_bitmap[i] = 0;
		shift_registers_sprite_attribute[i] = 0;
		shift_registers_sprite_x[i] = 0;
	}


	cpu = &cpu_src;
}


const bool PPU::isReadyToRender()
{
	if (ready_to_render)
	{
		ready_to_render = false;
		return true;
	}
	else
		return false;
}

//Napisanie jakiejs wartości (np. 0xZZ) do 0x4014 sprawia, że rozpoczyna się kopiowanie 256 elementów do OAM roozpoczynając od adresu 0xZZ00
void PPU::DMA()
{
	const unsigned short start_adress = (*reg.OAMDMA) << 8;
	for (int i = 0; (i + *reg.OAMADDR) < 256; i++)
	{
		OAM[i + *reg.OAMADDR] = *CPU_memory[start_adress + i];
	}

	DMA_activated = false;
}

//Operacja wykonywana przy każdym scanlinie, przygotowuje sprite'y i ich dane, aby wyświetlić je w następnej snanline
//Operacja oddzielna od renderingu!
//Each scanline, the PPU reads the spritelist(that is, Object Attribute Memory) to see which to draw :
//1. It clears the list of sprites to draw.
//2. It reads through OAM, checking which sprites will be on this scanline.It chooses the first eight it finds that do.
//3. If eight sprites were found, it checks(in a wrongly - implemented fashion) for further sprites on the scanline to see if the sprite overflow flag should be set.
//4. Using the details for the eight(or fewer) sprites chosen, it determines which pixels each has on the scanline and where to draw them.

#ifdef STARY_SPRITE_EVALUATION

void PPU::sprite_evaluation()
{
	//Dodaj tutaj obsługę tego[http://wiki.nesdev.com/w/index.php/PPU_registers]:
	/*Writes to OAMDATA during rendering (on the pre-render line and the visible lines 0-239, provided either sprite or background rendering is enabled) 
	do not modify values in OAM, but do perform a glitchy increment of OAMADDR, bumping only the high 6 bits (i.e., it bumps the [n] value in PPU sprite 
	evaluation - it's plausible that it could bump the low bits instead depending on the current status of sprite evaluation). This extends to DMA 
	transfers via OAMDMA, since that uses writes to $2004. For emulation purposes, it is probably best to completely ignore writes during rendering.*/

	static int n, m, found_sprites;//Przechowują stan liczników i ilośc znalezionych spriteów, również poza wywołaniami funkcji(nie są tracone po wyjściu z funkcji)

	if (scanline >= 0 && scanline <= 239)//During all visible scanlines, the PPU scans through OAM to determine which sprites to render on the next scanline
	{
		if (cycles == 1/*>= 1 && cycles <= 64*/)//1.
		{
			//Secondary OAM (32-byte buffer for current sprites on scanline) is initialized to $FF - attempting to read $2004 will return $FF.
			//secondary_OAM[(cycles - 1) >> 1] = 0xFF;

			//Żeby zaoszczędzić czas i nie nadpisywać tych samych pól kilka razy zrobię to raz w pętli:
			for (int i = 0; i < 32; i++)
				secondary_OAM[i] = 0xFF;
		}
		else if (cycles >= 65 && cycles <= 256)
		{
			if (cycles == 65)//Wyzerowanie liczników na początku tej części procesu sprite evaluation
				n = m = found_sprites = 0;

			//Zaczyna sprite evaluation od OAM[OAMADDR]
			//!!!The value of OAMADDR when sprite evaluation starts at tick 65 of the visible scanlines will determine where in OAM sprite evaluation starts, 
			//!!!and hence which sprite gets treated as sprite 0. The first OAM entry to be checked during sprite evaluation is the one starting at OAM[OAMADDR].


			if ((*reg.OAMADDR + (n * 4)) < 256)
			{
				if (found_sprites < 8)	//2.2b -> warunek
				{
					//2.1
					secondary_OAM[found_sprites * 4] = OAM[*reg.OAMADDR + (n * 4) + m];//read a sprite's Y-coordinate (OAM[n][0]), copying it to the next open slot in secondary OAM
					if (secondary_OAM[found_sprites * 4] >= ((scanline + 1) - (BIG_SPRITES ? 15 : 7)) && secondary_OAM[found_sprites * 4] <= ((scanline + 1) + (BIG_SPRITES ? 15 : 7)))//Sprawdza czy sprite jest w zakresie następnej scanline
					{
						for (int i = 1; i < 4; i++)
							secondary_OAM[(found_sprites * 4) + i] = OAM[*reg.OAMADDR + (n * 4) + i];//If Y-coordinate is in range, copy remaining bytes of sprite data (OAM[n][1] thru OAM[n][3]) into secondary OAM.

						++found_sprites;
					}

					++n;//2.2 Increment n
				}
				else if (found_sprites == 8)	//2.3 - Starting at m = 0, evaluate OAM[n][m] as a Y-coordinate. 
				{
					m = 0;

					if (OAM[*reg.OAMADDR + (n * 4) + m] >= ((scanline + 1) - 7) && OAM[*reg.OAMADDR + (n * 4) + m] <= ((scanline + 1) + 7))	//2.3a
					{
						/*If the value is in range, set the sprite overflow flag in $2002 and read the next 3 entries of OAM
						(incrementing 'm' after each byte and incrementing 'n' when 'm' overflows); if m = 3, increment n//*/
						
						*reg.PPUSTATUS |= 0x20;	//Ustawia flagę Sprite Overflow na true
						m = 3;
						++n;
					}
					else	//2.3b
					{
						/*If the value is not in range, increment n and m (without carry).
						If n overflows to 0, go to 4; otherwise go to 3//*/
						++m;
						++n;
					}
				}
			}
			else if (n == 64)
			{
				//2.4. Attempt (and fail) to copy OAM[n][0] into the next free slot in secondary OAM, and increment n (repeat until HBLANK is reached)
				//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
				//Czyli nic nie trzeba robić, wartość 'n' już nie będzie wykorzystywana w tej scanline
			}//*/
		}
		//Tę operację wykonam tylko raz, aby nie zajmować niepotrzebnie zasobów komputera - stąd zmieniona liczba cykli z <257, 320> na {257}
		else if (cycles == 257/*cycles >= 257 && cycles <= 320*/)//// Hblank begins after dot 256, and ends at dot 320 when the first tile of the next line is fetched
		{
			/*1-4: Read the Y-coordinate, tile number, attributes, and X-coordinate of the selected sprite from secondary OAM
			5-8: Read the X-coordinate of the selected sprite from secondary OAM 4 times (while the PPU fetches the sprite tile data)
			For the first empty sprite slot, this will consist of sprite #63's Y-coordinate followed by 3 $FF bytes;
			for subsequent empty sprite slots, this will be four $FF bytes//*/

			//To co u góry można pominąć, ale w tej scanline wypełnię niewykorzystane miejsce z secondary OAM zerami, żeby mieć tam przezroczyste spritey:
			// Unused sprites are loaded with an all-transparent bitmap.
			for (int i = 4 * found_sprites; i < 64; i++)
				secondary_OAM[i] = 0xFF;//0x00;
		}
		else if (cycles >= 321 && cycles <= 340 || cycles == 0)	//Background render pipeline initialization
		{
			//Read the first byte in secondary OAM (while the PPU fetches the first two background tiles for the next scanline)
			//↓↓↓↓↓
			//Nie trzeba nic robić w tych cyklach
		}
	}
}

#else

void PPU::sprite_evaluation()
{
	if (cycles == 1)
	{
		for (int i = 0; i < 32; i++)
			secondary_OAM[i] = 0xFF;
	}
	else if (cycles == 66)
	{
		int h;
		if (BIG_SPRITES)
			h = 16;
		else
			h = 8;
		int count = 0;
		for (int i = 0; i < 64; i++)
		{
			int y = OAM[i * 4 + 0];
			int a = OAM[i * 4 + 2];
			int x = OAM[i * 4 + 3];
			int row = scanline - y;
			if (row < 0 || row >= h)
				continue;

			if (count < 8)
			{
				secondary_OAM[count * 4] = OAM[(i * 4)];
				secondary_OAM[(count * 4) + 1] = OAM[(i * 4) + 1];
				secondary_OAM[(count * 4) + 2] = OAM[(i * 4) + 2];
				secondary_OAM[(count * 4) + 3] = OAM[(i * 4) + 3];
			}
			count++;
		}
		if (count > 8)
		{
			count = 8;
			*reg.PPUSTATUS |= 0x20;	//Ustawia flagę Sprite Overflow na true
		}
	}
}
#endif

void PPU::rendering()
{
	static unsigned char temp_nametable_data, temp_bitmap_data;	//Do zachowywania stanu pomiędzy wywołaniami 

	
	if (scanline >= 0 && scanline <= 239)	//Visible scanlines (0-239)
	{
		//Cykl 0 = Bezczynność
		if (cycles >= 1 && cycles <= 256)
		{
			//The data for each tile is fetched during this phase.Each memory access takes 2 PPU cycles to complete, and 4 must be performed per tile :
			//1.Nametable byte
			//2.Attribute table byte
			//3.Tile bitmap low
			//4.Tile bitmap high(+8 bytes from tile bitmap low)


			if ((cycles % 8) == 1)	//1.Nametable byte
			{
				//VRAM address is composed this way during rendering: 
				//yyy NN YYYYY XXXXX
				//||| || ||||| +++++--coarse X scroll
				//||| || +++++--------coarse Y scroll
				//||| ++--------------nametable select
				//+++-----------------fine Y scroll


				const unsigned short nametable_address = 0x2000 | (VRAM_address & 0x0FFF);	//Źródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
				temp_nametable_data = *memory[nametable_address];
			}
			else if ((cycles % 8) == 3)	//2.Attribute table byte
			{
				const unsigned short attribute_table_address = 0x23C0 | (VRAM_address & 0x0C00) | ((VRAM_address >> 4) & 0x38) | ((VRAM_address >> 2) & 0x07);	//Żródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
				const unsigned char attribute_table = *memory[attribute_table_address];

				output_attribute_table = get_shift_register_value<unsigned char>(shift_registers_background_attribute);
				shift_register<unsigned char>(shift_registers_background_attribute, attribute_table);

			}
			else if ((cycles % 8) == 5)	//3.Tile bitmap low
			{
				//Each tile in the pattern table is 16 bytes, made of two planes.The first plane controls bit 0 of the color; the second plane controls bit 1.
				//For example, if rows of a tile are numbered 0 through 7, row 1 of tile $69 in the left
				//pattern table is stored with plane 0 in $0691 and plane 1 in $0699.

				const unsigned short bitmap_low_address = PATTERN_TABLE_BACKGROUND_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(scanline % 8/*zakres od 0 do 7*/);
				
				const unsigned char bitmap_low = *memory[bitmap_low_address];
				
				temp_bitmap_data = bitmap_low;	//Zachowuje pobrane dane, aby wykorzystać, je kiedy już będę miał 'bitmap_high'
			}
			else if ((cycles % 8) == 7)	//4.Tile bitmap high(+8 bytes from tile bitmap low)
			{
				const unsigned short bitmap_high_address = (PATTERN_TABLE_BACKGROUND_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(scanline % 8/*zakres od 0 do 7*/)) + 8;
				const unsigned char bitmap_high = *memory[bitmap_high_address];

				output_bitmap = get_shift_register_value<unsigned short>(shift_registers_background_bitmap);
				shift_register<unsigned short>(shift_registers_background_bitmap, ((bitmap_high << 8) | temp_bitmap_data));
				
			
			}
			
			
			//Rendering pojedynczego piksela po każdym cyklu:
			render_pixel();


			

			//Rendering wszystkich spriteów:
			if (cycles == 256 && SHOW_SPRITES)	//Renderuję sprite-y dopiero pod koniec, ponieważ łatwiej jest to zaimplementować - o ich kolejności decyduje nie tylko bit "sprite priority" w OAM, ale też ich kolejność w OAM. Aby spritey o wyższym indeksie nie zasłoniły tych o niższym indeksie, renderuję je od końca
				render_sprites();

		}
		else if (cycles == 256 + 2 && SHOW_BACKGROUND && SHOW_SPRITES)
		{
			VRAM_address &= ~0x41f;
			VRAM_address |= VRAM_address_temp & 0x41f;
		}
		//else if (cycles >= 257 && cycles <= 320)
		//{
		//	
		//	//(cycles % 8) == 1	//Garbage nametable byte - nie trzeba nic robić
		//	if ((cycles % 8) == 3)	//Garbage nametable byte
		//	{
		//		//In addition to this, the X positions and attributes for each sprite are loaded from the secondary OAM into their respective counters/latches. 
		//		//This happens during the second garbage nametable fetch, with the attribute byte loaded during the first tick and the X coordinate during the second. 
		//		//shift_registers_sprite_attribute [((cycles - 257) / 8)] = OAM[(((cycles - 257) / 8) * 4) + 2];
		//		//shift_registers_sprite_x[((cycles - 257) / 8) * 4] = OAM[(((cycles - 257) / 8) * 4) + 3];
		//	}
		//	else if ((cycles % 8) == 5)	//Tile bitmap low
		//	{
		//		const unsigned char OAM_base_index = ((cycles - 257) / 8) * 4;
		//		temp_nametable_data = secondary_OAM[OAM_base_index + 1];	//pobiera index spritea
		//		const unsigned char Y_position = secondary_OAM[OAM_base_index];	//Pozycja Y spritea
		//		const bool sprite_flipped_horizontally = secondary_OAM[OAM_base_index + 2] & 0x40;
		//		const bool sprite_flipped_vertically = secondary_OAM[OAM_base_index + 2] & 0x80;
		//		/*if (!BIG_SPRITES)	//8x8
		//		{
		//			//For 8x8 sprites, this is the tile number of this sprite within the pattern table selected in bit 3 of PPUCTRL ($2000).
		//			unsigned short bitmap_low_address;
		//			if(!sprite_flipped_vertically)
		//				bitmap_low_address = PATTERN_TABLE_SPRITES_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(Y_position - scanline);
		//			else
		//				bitmap_low_address = PATTERN_TABLE_SPRITES_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(7 - Y_position - scanline);
		//			unsigned char bitmap_low = *memory[bitmap_low_address];
		//			if (sprite_flipped_horizontally)
		//				bitmap_low = flip_sprite_horizontally(bitmap_low);
		//			temp_bitmap_data = bitmap_low;
		//		}
		//		else	//8x16
		//		{
		//			//For 8x16 sprites, the PPU ignores the pattern table selection and selects a pattern table from bit 0 of this number.
		//			//--------
		//			//76543210
		//			//||||||||
		//			//|||||||+-Bank($0000 or $1000) of tiles
		//			//++++++ + --Tile number of top of sprite(0 to 254; bottom half gets the next tile)
		//			unsigned short bitmap_low_address;
		//			if (!sprite_flipped_vertically)
		//				bitmap_low_address = (temp_nametable_data & 0x01 ? 0x1000 : 0x0000) + ((temp_nametable_data & 0xfe) << 4) + (Y_position - scanline);
		//			else
		//				bitmap_low_address = (temp_nametable_data & 0x01 ? 0x1000 : 0x0000) + ((temp_nametable_data & 0xfe) << 4) + (15 - Y_position - scanline);
		//			unsigned char bitmap_low = *memory[bitmap_low_address];
		//			if (sprite_flipped_horizontally)
		//				bitmap_low = flip_sprite_horizontally(bitmap_low);
		//			temp_bitmap_data = bitmap_low;
		//		}//*/
		//		
		//	}
		//	else if ((cycles % 8) == 7)	//Tile bitmap high (+8 bytes from tile bitmap low)
		//	{
		//		const unsigned char OAM_base_index = ((cycles - 257) / 8) * 4;
		//		temp_nametable_data = secondary_OAM[OAM_base_index + 1];	//pobiera index spritea
		//		const unsigned char Y_position = secondary_OAM[OAM_base_index];	//Pozycja Y spritea
		//		const bool sprite_flipped_horizontally = secondary_OAM[OAM_base_index + 2] & 0x40;
		//		const bool sprite_flipped_vertically = secondary_OAM[OAM_base_index + 2] & 0x80;
		//		/*if (!BIG_SPRITES)	//8x8
		//		{
		//			//For 8x8 sprites, this is the tile number of this sprite within the pattern table selected in bit 3 of PPUCTRL ($2000).
		//			
		//			unsigned short bitmap_high_address; 
		//			if (!sprite_flipped_vertically)
		//				bitmap_high_address = (PATTERN_TABLE_SPRITES_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(Y_position - scanline)) + 8;
		//			else
		//				bitmap_high_address = (PATTERN_TABLE_SPRITES_ADDRESS | (temp_nametable_data << 4) | (unsigned char)(7 - Y_position - scanline)) + 8;
		//			
		//			unsigned char bitmap_high = *memory[bitmap_high_address];
		//			if (sprite_flipped_horizontally)
		//				bitmap_high = flip_sprite_horizontally(bitmap_high);
		//			const unsigned short new_bitmap = (bitmap_high << 8) | temp_bitmap_data;
		//			shift_registers_sprite_bitmap[(cycles - 257) / 8] = new_bitmap;
		//		}
		//		else	//8x16
		//		{
		//			//For 8x16 sprites, the PPU ignores the pattern table selection and selects a pattern table from bit 0 of this number.	
		//			//--------
		//			//76543210
		//			//||||||||
		//			//|||||||+-Bank($0000 or $1000) of tiles
		//			//++++++ + --Tile number of top of sprite(0 to 254; bottom half gets the next tile)
		//			unsigned short bitmap_high_address;
		//			if(!sprite_flipped_vertically)
		//				bitmap_high_address = (temp_nametable_data & 0x01 ? 0x1000 : 0x0000) + ((temp_nametable_data & 0xfe) << 4) + (Y_position - scanline) + 16;
		//			else
		//				bitmap_high_address = (temp_nametable_data & 0x01 ? 0x1000 : 0x0000) + ((temp_nametable_data & 0xfe) << 4) + (15 - Y_position - scanline) + 16;
		//			unsigned char bitmap_high = *memory[bitmap_high_address];
		//			if (sprite_flipped_horizontally)
		//				bitmap_high = flip_sprite_horizontally(bitmap_high);
		//			const unsigned short new_bitmap = (bitmap_high << 8) | temp_bitmap_data;
		//			shift_registers_sprite_bitmap[(cycles - 257) / 8] = new_bitmap;
		//		}//*/
		//	}//
		//}
		else if (cycles >= 321 && cycles <= 336)	//Wczytuje dwa pierwsze tilesy dla następnej scanline, ten sam kod, co w cyklach <1, 256>
		{

			if ((cycles % 8) == 1)	//1.Nametable byte
			{
				const unsigned short nametable_address = 0x2000 | (VRAM_address & 0x0FFF);	//Źródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
				temp_nametable_data = *memory[nametable_address];
			}
			else if ((cycles % 8) == 3)	//2.Attribute table byte
			{
				const unsigned short attribute_table_address = 0x23C0 | (VRAM_address & 0x0C00) | ((VRAM_address >> 4) & 0x38) | ((VRAM_address >> 2) & 0x07);	//Żródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
				const unsigned char attribute_table = *memory[attribute_table_address];

				shift_register<unsigned char>(shift_registers_background_attribute, attribute_table);
			}
			else if ((cycles % 8) == 5)	//3.Tile bitmap low
			{
				const unsigned short bitmap_low_address = PATTERN_TABLE_BACKGROUND_ADDRESS | (temp_nametable_data << 4) | /*(VRAM_address >> 12);*/(unsigned char)(scanline % 8/*zakres od 0 do 7*/);
				const unsigned char bitmap_low = *memory[bitmap_low_address];

				temp_bitmap_data = bitmap_low;	//Zachowuje pobrane dane, aby wykorzystać, je kiedy już będę miał 'bitmap_high'
			}
			else if ((cycles % 8) == 7)	//4.Tile bitmap high(+8 bytes from tile bitmap low)
			{
				const unsigned short bitmap_high_address = (PATTERN_TABLE_BACKGROUND_ADDRESS | (temp_nametable_data << 4) | /*(VRAM_address >> 12)) + 8;*/(unsigned char)(scanline % 8/*zakres od 0 do 7*/)) + 8;
				const unsigned char bitmap_high = *memory[bitmap_high_address];

				shift_register<unsigned short>(shift_registers_background_bitmap, ((bitmap_high << 8) | temp_bitmap_data));
			}	
		}

	}

}

void PPU::render_pixel()	//Ustawiam kolor piksela zgodnie z danymi pobranymi z odpowiedniego miejsca z palety systemowej
{
	if (SHOW_BACKGROUND)
	{
		unsigned char attribute_table_bits = (((output_attribute_table >> (((scanline % 32) < 16 ? 4 : 0)) >> (((cycles - 1) % 32) < 16 ? 2 : 0))) & 0x03);	//Attribute_byte = 2b(dół prawo) + 2b(dół lewo) + 2b(góra prawo) + 2b(góra lewo)
		const unsigned char color_index_address = 0x3f00 + (attribute_table_bits << 2) + (output_bitmap & (0x01 << (8 + (7 - (cycles - 1) % 8))) ? 0x02 : 0 ) + (output_bitmap & (0x01 << (7 - (cycles - 1) % 8)) ? 0x01 : 0);

		const unsigned char color_index = *memory[color_index_address];

		//Rysowanie zaczyna się od 1 cyklu, a numery pikseli od 0
		screen->setPixelColor(cycles - 1, scanline, color_palette[color_index]);	
	}
	else if(VRAM_address >= 0x3000 && VRAM_address <= 0x3fff)
		screen->setPixelColor(cycles - 1, scanline, color_palette[*memory[VRAM_address]]);
	else
		screen->setPixelColor(cycles - 1, scanline, color_palette[*memory[0x3f00]]);

	/*		SOURCE: https://forums.nesdev.com/viewtopic.php?f=2&t=15824
	"Turning off the background and sprite rendering bits in $2001 just prevents those things from being drawn,
	but the PPU is still active, and is still pushing (blank) frames to the TV"
	↓↓↓↓↓↓
	Correct. While rendering is off, the PPU displays the backdrop color over the whole picture.
	This is normally the color at $3F00 unless the current video memory address is $3F00-$3FFF, in which case it pushes that pixel instead.*/
}

void PPU::increment_cycles()
{
	total_cycles++;
	cycles++;			//Zawsze zwiększam cykl na początku, dlatego wielkości cykli w warunkach są w tej funkcji o 1 większe


	if (scanline == 261)
	{
		if (frame % 2 && cycles == 340)	//Dla nieparzystych klatek ostatni cykl jest pomijany
		{
			cycles = 0;
			scanline = 0;
			frame++;
			if (SHOW_BACKGROUND || SHOW_SPRITES)
				ready_to_render = true;
		}
		else if (!(frame % 2) && cycles == 341)	//W przypadku parzystych klatek wszystko jest normalnie
		{
			cycles = 0;
			scanline = 0;
			frame++;
			if (SHOW_BACKGROUND || SHOW_SPRITES)
				ready_to_render = true;
		}
	}
	else if (cycles == 341)	//Koniec scanline, początek nowej
	{
		scanline++;
		cycles = 0;
	}
}

const unsigned char PPU::flip_sprite_horizontally(const unsigned char & bitmap_part)//Do zmienienia, żeby używała pętli zamiast wpisywania na sztywno
{
	unsigned char temp = 0x00;
	temp |= (bitmap_part & 0x80) >> 7;
	temp |= (bitmap_part & 0x40) >> 5;
	temp |= (bitmap_part & 0x20) >> 3;
	temp |= (bitmap_part & 0x10) >> 1;
	temp |= (bitmap_part & 0x08) >> -1;
	temp |= (bitmap_part & 0x04) >> -3;
	temp |= (bitmap_part & 0x02) >> -5;
	temp |= (bitmap_part & 0x01) >> -7;
	return temp;
}

void PPU::render_sprites()
{
	struct buffer_changes	//Struktura, w której będą przetrzymywane informacje, czy sprite zasłoni tło
	{
		sf::Color new_color;
		bool changed;
	};

	buffer_changes scanline_changes[256];
	for (int i = 0; i < 256; i++)
	{
		scanline_changes[i].new_color = sf::Color(0, 0, 0);
		scanline_changes[i].changed = false;
	}



	for (int i = 7; i >= 0; i--)
	{
		const bool sprite_priority = shift_registers_sprite_attribute[i] & 0x20;	//Patrz na tabelę niżej
		const unsigned char sprite_palette = shift_registers_sprite_attribute[i] & 0x03;

		for (int x = shift_registers_sprite_x[i]; x < (x + 8); x++)
		{
			if (x >= 256)
			{
				break;	//Zabezpieczenie przed wychodzeniem spritea poza prawą krawędź ekranu
			}


			const unsigned short color_index_address = 0x3f10 + (sprite_palette << 2) + (shift_registers_sprite_bitmap[i] & (0x8000 >> (x - shift_registers_sprite_x[i])) ? 0x02 : 0x0) + (shift_registers_sprite_bitmap[i] & (0x80 >> (x - shift_registers_sprite_x[i])) ? 0x01 : 0x0);
			const unsigned char color_index = *memory[color_index_address];

			sf::Color sprite_pixel_color = color_palette[color_index];
			const bool transparent_sprite = color_index_address == 0x3f10 || color_index_address == 0x3f14 || color_index_address == 0x3f18 || color_index_address == 0x3f1c;

			sf::Color background_pixel_color = screen->getPixelColor(x, scanline);
			const bool transparent_background = (background_pixel_color == color_palette[*memory[0x3f00]]) || (background_pixel_color == color_palette[*memory[0x3f04]]) || (background_pixel_color == color_palette[*memory[0x3f08]]) || (background_pixel_color == color_palette[*memory[0x3f0c]]);


			//=============================================================================
			//|0 = pierwszy kolor z palety, 1-3 = drugi, trzeci lub czwarty kolor z palety|
			//-----------------------------------------------------------------------------
			//	|BG pixel 	|Sprite pixel 	|Priority 	|OUTPUT:
			//	|0 		|0 			|X=dowolny 	|BG($3F00)
			//	|0 		|1 - 3 		|X=dowolny	|Sprite
			//	|1 - 3 	|0 			|X=dowolny	|BG
			//	|1 - 3 	|1 - 3 		|0 		|Sprite
			//	|1 - 3 	|1 - 3 		|1 		|BG




			if (transparent_background && transparent_sprite)
			{
				//Bez zmian - zostaje kolor tła
				scanline_changes[x].changed = false;
			}
			else if (transparent_background && !transparent_sprite)
			{
				//Zmieniam kolor na kolor spritea
				scanline_changes[x].changed = true;
				scanline_changes[x].new_color = sprite_pixel_color;
			}
			else if (!transparent_background && transparent_sprite)
			{
				//Bez zmian - zostaje kolor tła
				scanline_changes[x].changed = false;
			}
			else if (!transparent_background && !transparent_sprite && sprite_priority)
			{
				//Żadne nie jest przezroczyste - sprite ma większy priorytet, więc zostaje kolor spritea
				scanline_changes[x].changed = true;
				scanline_changes[x].new_color = sprite_pixel_color;

			}
			else if (!transparent_background && !transparent_sprite && !sprite_priority)
			{
				//Żadne nie jest przezroczyste, a tło ma większy priorytet, więc kolor tła zostaje

				/*W NESie o tym, który sprite ma większy priorytet decyduje nie tylko 1 bit w trzecim bajcie secondary_OAM.
				Ma też znaczenie indeks spritea w secondary OAM(sprite 0 ma większy priorytet niż sprite 7, który jest ostatni w Secondary OAM.
				Jeśli zdarzy się, że któryś piksel spritea np. 0 ma taką samą współrzędną, co piksel np. spritea 1
				(lub dalszego w secondary OAM np. 2, 3,.. , 7), a sprite 0 ma tak ustawiony priorytet, by być pod tłem,
				to wtedy "zasłoni" on tego drugiego spritea i żaden z ich pikseli nie będzie widoczny - zostanie kolor tła*/

				scanline_changes[x].changed = false;
			}
		}
	}

	for (int i = 0; i < 256; i++)	//Ostateczne renderowanie spriteów - wpisanie ich do bufora
		if (scanline_changes[i].changed)
			screen->setPixelColor(i, scanline, scanline_changes[i].new_color);
}

void PPU::step()
{
	/*if(!(scanline % 100))
	std::cout << "Cycle: " << cycles << "\tScanline: " << scanline << "\tFrame: " << frame << "\n";//*/


	/*if (DMA_activated)
		DMA();//*/


	int y = 0;

	increment_cycles();


	rendering();
	sprite_evaluation();



	control_registers();
	update_VRAM_address();
	
	
}


//Koniec tilesa == inkrementuje adres
void PPU::update_VRAM_address()
{
	if (scanline >= 240 || !(SHOW_BACKGROUND || SHOW_SPRITES))
		VRAM_address = (VRAM_address + ADDRESS_INCREMENT) & 0x7fff;
	else//"if rendering is enabled..."
	{
		increment_X_coarse();
		increment_Y();
	}
}


void PPU::increment_X_coarse()
{
	//Kod skopiowany z: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Coarse_X_increment
	if ((VRAM_address & 0x001F) == 31) // if coarse X == 31
	{
		VRAM_address &= ~0x001F;       // coarse X = 0
		VRAM_address ^= 0x0400;           // switch horizontal nametable
	}
	else
	{
		VRAM_address += 1;          // increment coarse X
	}
}

void PPU::increment_Y()
{
	//Kod skopiowany z: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Y_increment
	if ((VRAM_address & 0x7000) != 0x7000)        // if fine Y < 7
	{
		VRAM_address += 0x1000; 	// increment fine Y
	}
	else
	{
		VRAM_address &= ~0x7000;                   // fine Y = 0
		int y = (VRAM_address & 0x03E0) >> 5;     // let y = coarse Y
		if (y == 29)
		{
			y = 0;                     // coarse Y = 0
			VRAM_address ^= 0x0800;               // switch VRAM_addressertical nametable
		}
		else if (y == 31)
		{
			y = 0;	// coarse Y = 0, nametable not switched
		}
		else
		{
			y += 1;                       // increment coarse Y
		}
		VRAM_address = (VRAM_address & ~0x03E0) | (y << 5);     // put coarse Y back into VRAM_address
	}
}

void PPU::control_registers()
{
	if (scanline == 261 && cycles == 1)
	{
		//Wyłącza VBlank:
		*reg.PPUSTATUS &= 0x7f;
		//Czyści flagę Sprite 0 hit:
		*reg.PPUSTATUS &= 0xbf;
		//Wyłącza flagę sprite overflow
		*reg.PPUSTATUS &= 0xdf;


		/*if OAMADDR is not less than eight when rendering starts, the eight bytes starting at OAMADDR & 0xF8
		are copied to the first eight bytes of OAM; it seems likely that this is related. On the Dendy,
		the latter bug is required for 2C02 compatibility. */
		if (*reg.OAMADDR >= 8)
			for (int i = 0; i < 8; i++)
				OAM[i] = OAM[(*reg.OAMADDR & 0xF8) + i];
	}
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	else if (cycles == 256 + 2 && SHOW_BACKGROUND && SHOW_SPRITES)
	{
		//Set bits related to horizontal position
		VRAM_address &= ~0x41f; //Unset horizontal bits
		VRAM_address |= VRAM_address_temp & 0x41f; //Copy
	}
	else if (cycles > 280 && cycles <= 304 && SHOW_BACKGROUND && SHOW_SPRITES)
	{
		//Set vertical bits
		VRAM_address &= ~0x7be0; //Unset bits related to horizontal
		VRAM_address |= VRAM_address_temp & 0x7be0; //Copy
	}
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	else if (scanline == 241 && cycles == 1)
	{
		//Ustawiam Vblank:
		*reg.PPUSTATUS |= 0x80;

		//Start of vertical blanking: Set NMI_occurred in PPU to true.
		
		if (NMI_GENERATE)
			cpu->set_NMI();//*/
	}


	//"if rendering is enabled..."
	if ((SHOW_BACKGROUND || SHOW_SPRITES))
	{
		if (cycles == 257)
		{
			//If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:

			//v: ...-.F..-...E-DCBA = t: ...-.F.. ...E-DCBA
			VRAM_address &= 0x7be0;
			const unsigned temp = VRAM_address_temp & 0x041f;
			VRAM_address |= temp;
		}
		else if (cycles >= 280 && cycles <= 304 && scanline == 261)
		{
			//If rendering is enabled, at the end of vblank, shortly AFTER the horizontal bits are copied 
			//from t to v at dot 257, the PPU will repeatedly copy the vertical bits from t to v 
			//from dots 280 to 304, completing the full initialization of v from t:

			//v: IHGF.ED CBA..... = t: IHG-F.ED CBA.-....
			VRAM_address &= 0x041f;
			const unsigned short temp = VRAM_address_temp & 0x7be0;
			VRAM_address |= temp;
		}
	}


	//OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines. 
	if (((scanline >= 0 && scanline <= 239) || scanline == 261) && cycles >= 257 && cycles <= 320)
	{
		*reg.OAMADDR = 0;
		//update_2002(0);	//??
	}
}


void PPU::write_2000(const unsigned char &value)
{

//	/*if (total_cycles < 30000)	//After power/reset, writes to this register are ignored for about 30000 cycles
//		return;//*/
	

	unsigned char old_ppuctrl = *reg.PPUCTRL;
	*reg.PPUCTRL = value;

	//t: ...-BA..-....-.... = d : ....-..BA
	VRAM_address_temp &= 0xF3FF;
	const unsigned short temp = value << 10;
	VRAM_address_temp |= temp;





//#################
//Inspiracja: Mesen
//#################
	//"By toggling NMI_output ($2000 bit 7) during vertical blank without reading $2002, a program can cause /NMI to be pulled low multiple times, causing multiple NMIs to be generated."
	bool original_vblank = old_ppuctrl & 0x80;


	if (!original_vblank && (*reg.PPUCTRL & 0x80) && (*reg.PPUSTATUS & 0x80) && (scanline != 261 || cycles != 0))
		cpu->set_NMI();

	if (scanline == 241 && cycles < 3 && !(*reg.PPUCTRL & 0x80))
		cpu->unset_NMI();


}

const unsigned char PPU::read_2002()
{
	//Odczyt 2002 czyści write_latch używane przez rejestry 2005 i 2007
	//w:                  = 0
	write_toggle = false;

	unsigned char result = *reg.PPUSTATUS;

	//reading 2002h clears the VBlank flag -> trzeba ustawić 7. bit na wartośc 0(zwracając wartość tego rejestru w jego miejscu zwracamy aktualną wartość NMI_occured
	*reg.PPUSTATUS &= 0x7F;	//Czyści 7. bit
	
	//unsigned char result = *reg.PPUSTATUS | (NMI_occured ? 0x80 : 0x00);
	//NMI_occured = false;




//#################
//Inspiracja: Mesen
//#################
	if (scanline == 241  && cycles < 3)
	{

		//"Reading on the same PPU clock or one later reads it as set, clears it, and suppresses the NMI for that frame."
		cpu->unset_NMI();

		
		if (cycles == 0)	//"Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame. "
			result &= 0x60;
	}


	return result;
}

const unsigned char PPU::read_2004()
{
	//Cycles 1 - 64: Secondary OAM(32 - byte buffer for current sprites on scanline) is initialized to $FF - attempting to read $2004 will return $FF - podczas sprite evaluation
	if (cycles >= 1 && cycles <= 64 && scanline >= 0 && scanline <= 239)
		return 0xFF;
	else
		return OAM[*reg.OAMADDR];
}

void PPU::write_2004(const unsigned char &value)
{
	if ((scanline == 261 || (scanline >= 0 && scanline <= 239)) && (SHOW_BACKGROUND || SHOW_SPRITES))	//For emulation purposes, it is probably best to completely ignore writes during rendering.
		return;
	else
	{
		*reg.OAMDATA = value;
		OAM[*reg.OAMADDR] = *reg.OAMDATA;
		*reg.OAMADDR+=1;
	}
}

void PPU::write_2005(const unsigned char &value)
{
	*reg.PPUSCROLL = value;

	if(!write_toggle)	//$2005 first write (w is 0)
	{
		//t: ....... ...HGFED = d : HGFED...
		VRAM_address_temp &= 0x7fe0;
		VRAM_address_temp |= (value >> 3);

		//x:              CBA = d: .....CBA
		X_scroll = value & 0x07;

		//w:                  = 1
		write_toggle = true;
	}
	else	//$2005 second write (w is 1)
	{
		//t: CBA..HG FED..... = d : HGFEDCBA
		VRAM_address_temp &= 0x0c1f;

		unsigned short temp = (value & 0x7) << 12;//CBA
		VRAM_address_temp |= temp;
		temp = (value & 0x38) << 2;//FED
		VRAM_address_temp |= temp;
		temp = (value & 0xc0) << 2;//HG

		//w:                  = 0
		write_toggle = false;;
	}
}

void PPU::write_2006(const unsigned char &value)
{
	*reg.PPUADDR = value;

	if(!write_toggle)	//$2006 first write (w is 0)
	{
		
		//t:.FEDCBA ........ = d : ..FEDCBA
		const unsigned short temp = (value & 0x3f) << 8;
		VRAM_address_temp |= temp;
		
		//t: X...... ........ = 0
		VRAM_address_temp &= 0x3fff;
		
		//w:                  = 1
		write_toggle = true;
	}
	else	//$2006 second write (w is 1)
	{
		//t: .......HGFEDCBA = d : HGFEDCBA
		VRAM_address_temp &= 0x7f00;
		VRAM_address_temp |= value;

		//v = t
		VRAM_address = VRAM_address_temp;

		//w : = 0
		write_toggle = false;

		if (VRAM_address == 0x12)
			int x = 0;
	}
}

void PPU::write_2007(const unsigned char & value)
{
	const unsigned short temp_address = VRAM_address & 0x3fff;	//Note that while the v register has 15 bits, the PPU memory space is only 14 bits wide.The highest bit is unused for access through $2007.
	*reg.PPUDATA = value;

	//if (!(VRAM_address >= 0x3f00 && VRAM_address <= 0x3fff))
		*memory[VRAM_address & 0x3fff] = value;

	if ((scanline == 261 || (scanline >= 0 && scanline <= 239)) && (SHOW_BACKGROUND || SHOW_SPRITES))
	{
		//During rendering (on the pre-render line and the visible lines 0-239, provided either background or sprite rendering is enabled), 
		//it will update v in an odd way, triggering a coarse X increment and a Y increment simultaneously (with normal wrapping behavior).

		//Coarse X increment
		increment_X_coarse();
		//Y increment
		increment_Y();
	}
	else//*/
	{
		//Outside of rendering, reads from or writes to $2007 will add either 1 or 32 
		//to v depending on the VRAM increment bit set via $2000.
		VRAM_address += ADDRESS_INCREMENT;
	}
}

const unsigned char PPU::read_2007()
{
	if ((scanline == 261 || (scanline >= 0 && scanline <= 239)) && (SHOW_BACKGROUND || SHOW_SPRITES))
	{
		//During rendering (on the pre-render line and the visible lines 0-239, provided either background or sprite rendering is enabled),
		//it will update v in an odd way, triggering a coarse X increment and a Y increment simultaneously (with normal wrapping behavior).

		const unsigned char old_address = VRAM_address;	//Kopiuje stary adres, przed jego zmianą

		//Coarse X increment
		increment_X_coarse();
		//Y increment
		increment_Y();


		return *memory[old_address];
	}
	else//*/
	{
		//Outside of rendering, reads from or writes to $2007 will add either 1 or 32 
		//to v depending on the VRAM increment bit set via $2000.
		const unsigned char old_address = VRAM_address;
		VRAM_address += ADDRESS_INCREMENT;
		return *memory[old_address];
	}
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#ifdef DUPA123
void PPU::step()
{
	if(cycles == 261)
	{
		if (cycles == 1)
			m_vblank /*= m_sprZeroHit*/ = false;
		else if (cycles == 256 + 2 && m_showBackground && m_showSprites)
		{
			//Set bits related to horizontal position
			m_dataAddress &= ~0x41f; //Unset horizontal bits
			m_dataAddress |= m_tempAddress & 0x41f; //Copy
		}
		else if (cycles > 280 && cycles <= 304 && m_showBackground && m_showSprites)
		{
			//Set vertical bits
			m_dataAddress &= ~0x7be0; //Unset bits related to horizontal
			m_dataAddress |= m_tempAddress & 0x7be0; //Copy
		}
		//                 if (cycles > 257 && cycles < 320)
		//                     m_spriteDataAddress = 0;
					   //if rendering is on, every other frame is one cycle shorter
		if (cycles >= 340 - (!m_evenFrame && m_showBackground && m_showSprites))
		{
			cycles = scanline = 0;
		}
	}
	else if (scanline >= 0 && scanline <= 239)
	{
		if (cycles > 0 && cycles <= 256)
		{
			Byte bgColor = 0, sprColor = 0;
			bool bgOpaque = false, sprOpaque = true;
			bool spriteForeground = false;

			int x = cycles - 1;
			int y = scanline;

			if (m_showBackground)
			{
				auto x_fine = (m_fineXScroll + x) % 8;
				if (!m_hideEdgeBackground || x >= 8)
				{
					//fetch tile
					auto addr = 0x2000 | (m_dataAddress & 0x0FFF); //mask off fine y
					//auto addr = 0x2000 + x / 8 + (y / 8) * (256 / 8);
					Byte tile = read(addr);

					//fetch pattern
					//Each pattern occupies 16 bytes, so multiply by 16
					addr = (tile * 16) + ((m_dataAddress >> 12/*y % 8*/) & 0x7); //Add fine y
					addr |= m_bgPage << 12; //set whether the pattern is in the high or low page
					//Get the corresponding bit determined by (8 - x_fine) from the right
					bgColor = (read(addr) >> (7 ^ x_fine)) & 1; //bit 0 of palette entry
					bgColor |= ((read(addr + 8) >> (7 ^ x_fine)) & 1) << 1; //bit 1

					bgOpaque = bgColor; //flag used to calculate final pixel with the sprite pixel

					//fetch attribute and calculate higher two bits of palette
					addr = 0x23C0 | (m_dataAddress & 0x0C00) | ((m_dataAddress >> 4) & 0x38)
						| ((m_dataAddress >> 2) & 0x07);
					auto attribute = read(addr);
					int shift = ((m_dataAddress >> 4) & 4) | (m_dataAddress & 2);
					//Extract and set the upper two bits for the color
					bgColor |= ((attribute >> shift) & 0x3) << 2;
				}
				//Increment/wrap coarse X
				if (x_fine == 7)
				{
					if ((m_dataAddress & 0x001F) == 31) // if coarse X == 31
					{
						m_dataAddress &= ~0x001F;          // coarse X = 0
						m_dataAddress ^= 0x0400;           // switch horizontal nametable
					}
					else
						m_dataAddress += 1;                // increment coarse X
				}
			}

			if (m_showSprites && (!m_hideEdgeSprites || x >= 8))
			{
				for (auto i : scanlineSprites)
				{
					Byte spr_x = m_spriteMemory[i * 4 + 3];

					if (0 > x - spr_x || x - spr_x >= 8)
						continue;

					Byte spr_y = m_spriteMemory[i * 4 + 0] + 1,
						tile = m_spriteMemory[i * 4 + 1],
						attribute = m_spriteMemory[i * 4 + 2];

					int length = (m_longSprites) ? 16 : 8;

					int x_shift = (x - spr_x) % 8, y_offset = (y - spr_y) % length;

					if ((attribute & 0x40) == 0) //If NOT flipping horizontally
						x_shift ^= 7;
					if ((attribute & 0x80) != 0) //IF flipping vertically
						y_offset ^= (length - 1);

					Address addr = 0;

					if (!m_longSprites)
					{
						addr = tile * 16 + y_offset;
						if (m_sprPage == High) addr += 0x1000;
					}
					else //8x16 sprites
					{
						//bit-3 is one if it is the bottom tile of the sprite, multiply by two to get the next pattern
						y_offset = (y_offset & 7) | ((y_offset & 8) << 1);
						addr = (tile >> 1) * 32 + y_offset;
						addr |= (tile & 1) << 12; //Bank 0x1000 if bit-0 is high
					}

					sprColor |= (read(addr) >> (x_shift)) & 1; //bit 0 of palette entry
					sprColor |= ((read(addr + 8) >> (x_shift)) & 1) << 1; //bit 1

					if (!(sprOpaque = sprColor))
					{
						sprColor = 0;
						continue;
					}

					sprColor |= 0x10; //Select sprite palette
					sprColor |= (attribute & 0x3) << 2; //bits 2-3

					spriteForeground = !(attribute & 0x20);

					//Sprite-0 hit detection
					if (!m_sprZeroHit && m_showBackground && i == 0 && sprOpaque && bgOpaque)
					{
						m_sprZeroHit = true;
					}

					break; //Exit the loop now since we've found the highest priority sprite
				}
			}

			Byte paletteAddr = bgColor;

			if ((!bgOpaque && sprOpaque) ||
				(bgOpaque && sprOpaque && spriteForeground))
				paletteAddr = sprColor;
			else if (!bgOpaque && !sprOpaque)
				paletteAddr = 0;
			//else bgColor

//                     m_screen.setPixel(x, y, sf::Color(colors[m_bus.readPalette(paletteAddr)]));
			m_pictureBuffer[x][y] = sf::Color(colors[m_bus.readPalette(paletteAddr)]);
		}
		else if (cycles == 256 + 1 && m_showBackground)
		{
			//Shamelessly copied from nesdev wiki
			if ((m_dataAddress & 0x7000) != 0x7000)  // if fine Y < 7
				m_dataAddress += 0x1000;              // increment fine Y
			else
			{
				m_dataAddress &= ~0x7000;             // fine Y = 0
				int y = (m_dataAddress & 0x03E0) >> 5;    // let y = coarse Y
				if (y == 29)
				{
					y = 0;                                // coarse Y = 0
					m_dataAddress ^= 0x0800;              // switch vertical nametable
				}
				else if (y == 31)
					y = 0;                                // coarse Y = 0, nametable not switched
				else
					y += 1;                               // increment coarse Y
				m_dataAddress = (m_dataAddress & ~0x03E0) | (y << 5);
				// put coarse Y back into m_dataAddress
			}
		}
		else if (cycles == 256 + 2 && m_showBackground && m_showSprites)
		{
			//Copy bits related to horizontal position
			m_dataAddress &= ~0x41f;
			m_dataAddress |= m_tempAddress & 0x41f;
		}

		//                 if (cycles > 257 && cycles < 320)
		//                     m_spriteDataAddress = 0;

		if (cycles >= 340)
		{
			//Find and index sprites that are on the next Scanline
			//This isn't where/when this indexing, actually copying in 2C02 is done
			//but (I think) it shouldn't hurt any games if this is done here

			scanlineSprites.resize(0);

			int range = 8;
			if (m_longSprites)
				range = 16;

			std::size_t j = 0;
			for (std::size_t i = m_spriteDataAddress / 4; i < 64; ++i)
			{
				auto diff = (scanline - m_spriteMemory[i * 4]);
				if (0 <= diff && diff < range)
				{
					scanlineSprites.push_back(i);
					++j;
					if (j >= 8)
					{
						break;
					}
				}
			}

			++scanline;
			cycles = 0;
		}

		if (scanline >= 240)
			m_pipelineState = PostRender;

		break;
	}
	else if(scanline == 240)
	{
		if (cycles >= 340)
		{
			++scanline;
			cycles = 0;
			m_pipelineState = VerticalBlank;

			for (int x = 0; x < m_pictureBuffer.size(); ++x)
			{
				for (int y = 0; y < m_pictureBuffer[0].size(); ++y)
				{
					m_screen.setPixel(x, y, m_pictureBuffer[x][y]);
				}
			}

			//Should technically be done at first dot of VBlank, but this is close enough
//                     m_vblank = true;
//                     if (m_generateInterrupt) m_vblankCallback();

		}
	}
	if(scanline >= 241 && scanline <= 260)
	{
		if (cycles == 1 && scanline == 240 + 1)
		{
			m_vblank = true;
			if (m_generateInterrupt) m_vblankCallback();
		}

		if (cycles >= 340)
		{
			++scanline;
			cycles = 0;
		}

		if (scanline >= 261)
		{
			m_pipelineState = PreRender;
			scanline = 0;
			m_evenFrame = !m_evenFrame;
			//                     m_vblank = false;
		}
	}
	else
		std::cout << "Well, this shouldn't have happened." << std::endl;
	}

	++cycles;
}
#endif