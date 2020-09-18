#include "PPU.h"
#include "CPU.h"
#include "Screen.h"


#define Y_SCROLL (unsigned char)((VRAM_address >> 12) & 0x07)
#define NAMETABLE_FETCH_ADDRESS (0x2000 | (VRAM_address & 0x0FFF))	//Źródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
#define ATTRIBUTE_TABLE_FETCH_ADDRESS (0x23C0 | (VRAM_address & 0x0C00) | ((VRAM_address >> 4) & 0x38) | ((VRAM_address >> 2) & 0x07))	//Żródło: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
#define BITMAP_LOW_FETCH_ADDRESS (PATTERN_TABLE_BACKGROUND_ADDRESS | ((unsigned short)processed_nametable_byte << 4) | Y_SCROLL)
#define BITMAP_HIGH_FETCH_ADDRESS (BITMAP_LOW_FETCH_ADDRESS + 8)


PPU::PPU()
{
	scanline = 241;
	cycles = 0;			//Patrz:
	total_cycles = 0;	//przy pierwszym przejsciu przez pętle przyjmie wartosc 0
	frame = 0;
	initialize_memory();

	VRAM_address = 0;
	VRAM_address_temp = 0;
	x_scroll = 0;

	ready_to_render = false;
	DMA_activated = false;

	attribute_latch[0] = 0;
	attribute_latch[1] = 0;

	for (int x = 0; x < 2; x++)
		bitmap_latch[x] = 0;

	internal_buffer_2007 = 0;

	NMI_output = false;
	NMI_occured = false;

	found_sprite_zero = false;
	found_sprites = 0;
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
		if (i % 4 == 0 && (i % 0x20) >= 0x10)
			memory[0x3f00 + i] = &mem.palette_RAM_indexes[(i % 0x20) - 0x10];
		else//*/
			memory[0x3f00 + i] = &mem.palette_RAM_indexes[i % 0x20];
	}


	const unsigned char power_up_palette_state[32] = {		//Żródło: https://forums.nesdev.com/viewtopic.php?t=567
		0x09,0x01,0x00,0x01,0x00,0x02,0x02,0x0D,0x08,0x10,0x08,0x24,0x00,0x00,0x04,0x2C,
		0x09,0x01,0x34,0x03,0x00,0x04,0x00,0x14,0x08,0x3A,0x00,0x02,0x00,0x20,0x2C,0x08
	};

	for (int i = 0; i < 32; i++)
	{
		mem.palette_RAM_indexes[i] = power_up_palette_state[i];
	}

	for (int x = 0; x < 0x3f00; x++)
		* memory[x] = 0;




}

void PPU::initialize_registers(CPU& cpu_src)
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
		CPU_memory[i] = cpu_src.memory[i];

	//Dodane później:


	for (int i = 0; i < 256; i++)
		OAM[i] = 0xff;	//Przy zainicjalizowaniu wartością 0, wystąpi błąd przy pierwszym sprite evaluation
	for (int i = 0; i < 32; i++)
		secondary_OAM[i] = 0xff;

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

void PPU::increment_address()
{
	VRAM_address += ADDRESS_INCREMENT;
	VRAM_address &= 0x7fff;	//adres ma 15 bitów, unsigned short 16
}

void PPU::increment_address_horizontal()
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
	}//*/
}

void PPU::increment_address_vertical()
{
	//Kod skopiowany z: http://wiki.nesdev.com/w/index.php/PPU_scrolling#Y_increment
	if ((VRAM_address & 0x7000) != 0x7000)        // if fine Y < 7
	{
		VRAM_address += 0x1000; 	// increment fine Y
	}
	else
	{
		VRAM_address &= ~0x7000;                   // fine Y = 0
		unsigned short y = (VRAM_address & 0x03E0) >> 5;     // let y = coarse Y
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
	}//*/
}

void PPU::step()
{
	if (scanline >= 0 && scanline <= 239)//Visible scanlines
	{

		switch (cycles)
		{
			//Nametable byte
		case 1:
			processed_nametable_byte = *memory[NAMETABLE_FETCH_ADDRESS];
			break;

			//Nametable byte, shift registers are reloaded
		case 9:  case 17:  case 25:  case 33:  case 41:  case 49:  case 57:
		case 65:  case 73:  case 81:  case 89:  case 97:  case 105:  case 113:  case 121:
		case 129:  case 137:  case 145:  case 153:  case 161:  case 169:  case 177:  case 185:
		case 193:  case 201:  case 209:  case 217:  case 225:  case 233:  case 241:  case 249:
			processed_nametable_byte = *memory[NAMETABLE_FETCH_ADDRESS];
			save_attribute_to_latch(processed_attribute_table_byte);	//The shifters are reloaded during ticks 9, 17, 25, ..., 257. 
			save_bitmap_to_latch(processed_bitmap_low, processed_bitmap_high);
			break;





			//Attribute table byte
		case 3:  case 11:  case 19:  case 27:  case 35:  case 43:  case 51:  case 59:
		case 67:  case 75:  case 83:  case 91:  case 99:  case 107:  case 115:  case 123:
		case 131:  case 139:  case 147:  case 155:  case 163:  case 171:  case 179:  case 187:
		case 195:  case 203:  case 211:  case 219:  case 227:  case 235:  case 243:  case 251:

			//VRAM_address:
			//yyy NN YYYYY XXXXX
			//||| || ||||| +++++--coarse X scroll = 8x8 tile x postiion
			//||| || +++++--------coarse Y scroll = 8x8 tile y position
			//||| ++--------------nametable select
			//+++-----------------fine Y scroll
			//Attribute_byte = 2b(dół prawo) + 2b(dół lewo) + 2b(góra prawo) + 2b(góra lewo)
			//Each byte controls the palette of a 32×32 pixel or 4×4 tile part of the nametable and is divided into four 2-bit areas. Each area covers 16×16 pixels or 2×2 tiles	

			processed_attribute_table_byte = ((*memory[ATTRIBUTE_TABLE_FETCH_ADDRESS] >> ((((VRAM_address >> 5)/*y*/ & 0x02) ? 4 : 0) + ((VRAM_address & 0x02)/*x*/ ? 2 : 0))) << 2) & 0x0c;
			break;

			//Tile bitmap low
		case 5:  case 13:  case 21:  case 29:  case 37:  case 45:  case 53:  case 61:
		case 69:  case 77:  case 85:  case 93:  case 101:  case 109:  case 117:  case 125:
		case 133:  case 141:  case 149:  case 157:  case 165:  case 173:  case 181:  case 189:
		case 197:  case 205:  case 213:  case 221:  case 229:  case 237:  case 245:  case 253:
			processed_bitmap_low = *memory[BITMAP_LOW_FETCH_ADDRESS];
			break;

			//Tile bitmap high
		case 7:  case 15:  case 23:  case 31:  case 39:  case 47:  case 55:  case 63:
		case 71:  case 79:  case 87:  case 95:  case 103:  case 111:  case 119:  case 127:
		case 135:  case 143:  case 151:  case 159:  case 167:  case 175:  case 183:  case 191:
		case 199:  case 207:  case 215:  case 223:  case 231:  case 239:  case 247:  case 255:
			processed_bitmap_high = *memory[BITMAP_HIGH_FETCH_ADDRESS];
			break;

			//Inc hori(v)
		case 8:  case 16:  case 24:  case 32:  case 40:  case 48:  case 56:  case 64:
		case 72:  case 80:  case 88:  case 96:  case 104:  case 112:  case 120:  case 128:
		case 136:  case 144:  case 152:  case 160:  case 168:  case 176:  case 184:  case 192:
		case 200:  case 208:  case 216:  case 224:  case 232:  case 240:  case 248:
			if (RENDERING_ENABLED)
				increment_address_horizontal();//*/
			break;

			//Inc vert(v)
		case 256:
			if (RENDERING_ENABLED)
			{
				increment_address_horizontal();
				increment_address_vertical();
			}
			break;

			//If rendering is enabled, the PPU copies all bits related to horizontal position from t to v
		case 257:
			if (RENDERING_ENABLED)
			{
				VRAM_address = (VRAM_address & ~0x041F) | (VRAM_address_temp & 0x041F);
			}
			save_attribute_to_latch(processed_attribute_table_byte);	//The shifters are reloaded during ticks 9, 17, 25, ..., 257. 
			save_bitmap_to_latch(processed_bitmap_low, processed_bitmap_high);



			//Sprite fetches for next frame:
		case 258: case 259: case 260: case 261: case 262: case 263: case 264:
		case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272:
		case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280:
		case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288:
		case 289: case 290: case 291: case 292: case 293: case 294: case 295: case 296:
		case 297: case 298: case 299: case 300: case 301: case 302: case 303: case 304:
		case 305: case 306: case 307: case 308: case 309: case 310: case 311: case 312:
		case 313: case 314: case 315: case 316: case 317: case 318: case 319: case 320:
			if ((cycles - 257) / 8 < found_sprites)
			{
				unsigned char sprite_fetch_cycles = cycles - 257; //=<0, 63>
				unsigned char n = sprite_fetch_cycles / 8;



				switch (sprite_fetch_cycles % 8)
				{
				case 2:
					sprites_attributes_latch[n] = secondary_OAM[(n * 4) + 2];//X positions and attributes for each sprite are loaded from the secondary OAM into their respective counters / latches.This happens during the second garbage nametable fetch(...)
					break;													//(...)with the attribute byte loaded during the first tick(...)

				case 3:
					sprites_x_coordinate_latch[n] = secondary_OAM[(n * 4) + 3];//(...)and the X coordinate during the second
					break;

				case 4:	//Bitmap low fetch
				{
					const bool flip_vertically = secondary_OAM[(n * 4) + 2] & 0x80;
					unsigned short sprite_bitmap_low_address;

					if (BIG_SPRITES)
						sprite_bitmap_low_address = (secondary_OAM[(n * 4) + 1] & 0x01 ? 0x1000 : 0x0000) | ((unsigned short)(secondary_OAM[(n * 4) + 1] & 0xfe) << 4) | (flip_vertically ? ((scanline - secondary_OAM[n * 4]) >= 8 ? 15 - (scanline - secondary_OAM[n * 4]) : 23 - (scanline - secondary_OAM[n * 4])) : ((scanline - secondary_OAM[n * 4]) >= 8 ? (8 + scanline - secondary_OAM[n * 4]) : (scanline - secondary_OAM[n * 4])));
					else
						sprite_bitmap_low_address = PATTERN_TABLE_SPRITES_ADDRESS | ((unsigned short)secondary_OAM[(n * 4) + 1] << 4) | (flip_vertically ? (7 - (scanline - secondary_OAM[n * 4])) : (scanline - secondary_OAM[n * 4]));

					sprites_bitmap_latch[n][0] = *memory[sprite_bitmap_low_address];
				}
				break;

				case 6:	//Bitmap high fetch
				{
					const bool flip_vertically = secondary_OAM[(n * 4) + 2] & 0x80;
					unsigned short sprite_bitmap_high_address;

					if (BIG_SPRITES)
						sprite_bitmap_high_address = ((secondary_OAM[(n * 4) + 1] & 0x01 ? 0x1000 : 0x0000) | ((unsigned short)(secondary_OAM[(n * 4) + 1] & 0xfe) << 4) | (flip_vertically ? ((scanline - secondary_OAM[n * 4]) >= 8 ? 15 - (scanline - secondary_OAM[n * 4]) : 23 - (scanline - secondary_OAM[n * 4])) : ((scanline - secondary_OAM[n * 4]) >= 8 ? (8 + scanline - secondary_OAM[n * 4]) : (scanline - secondary_OAM[n * 4])))) + 8;
					else
						sprite_bitmap_high_address = (PATTERN_TABLE_SPRITES_ADDRESS | ((unsigned short)secondary_OAM[(n * 4) + 1] << 4) | (flip_vertically ? (7 - (scanline - secondary_OAM[n * 4])) : (scanline - secondary_OAM[n * 4]))) + 8;

					sprites_bitmap_latch[n][1] = *memory[sprite_bitmap_high_address];
				}
				break;
				}
			}
			*reg.OAMADDR = 0;	//OAMADDR is set to 0 during each of ticks 257 - 320  of the pre - render and visible scanlines
			break;




			//Background fetches for next frame:
		case 321:  case 329:
			processed_nametable_byte = *memory[NAMETABLE_FETCH_ADDRESS];
			break;

		case 323:  case 331:
			//processed_attribute_table_byte = *memory[ATTRIBUTE_TABLE_FETCH_ADDRESS];
			processed_attribute_table_byte = ((*memory[ATTRIBUTE_TABLE_FETCH_ADDRESS] >> ((((VRAM_address >> 5)/*y*/ & 0x02) ? 4 : 0) + ((VRAM_address & 0x02)/*x*/ ? 2 : 0))) << 2) & 0x0c;
			break;

		case 325: case 333:
			processed_bitmap_low = *memory[BITMAP_LOW_FETCH_ADDRESS];
			break;

		case 327:  case 335:
			processed_bitmap_high = *memory[BITMAP_HIGH_FETCH_ADDRESS];
			break;

		case 328:  case 336:
			if (RENDERING_ENABLED)
				increment_address_horizontal();
			save_bitmap_to_latch(processed_bitmap_low, processed_bitmap_high);
			save_attribute_to_latch(processed_attribute_table_byte);
			break;

		}


		if (cycles >= 1 && cycles <= 256)
			render_pixel(cycles - 1, scanline);
	}
	else if (scanline >= 241 && scanline <= 260)//Vblank scanlines
	{
		if (scanline == 241 && cycles == 1)
		{
			//Ustawiam Vblank:
			NMI_occured = true;
			*reg.PPUSTATUS |= 0x80;


			//Start of vertical blanking: Set NMI_occurred in PPU to true.
			if (NMI_output && NMI_occured)
				cpu->set_NMI();
		}
	}
	else if (scanline == 261)//Pre-render scanline
	{
		switch (cycles)
		{
			//Clear Vblank, Sprite 0, Overflow
		case 1:
			NMI_occured = false;
			*reg.PPUSTATUS &= 0x7f;	//Wyłącza VBlank
			*reg.PPUSTATUS &= 0xbf;	//Czyści flagę Sprite 0 hit
			*reg.PPUSTATUS &= 0xdf;	//Wyłącza flagę sprite overflow
			break;


			//Inc hori(v)
		case 8:  case 16:  case 24:  case 32:  case 40:  case 48:  case 56:  case 64:
		case 72:  case 80:  case 88:  case 96:  case 104:  case 112:  case 120:  case 128:
		case 136:  case 144:  case 152:  case 160:  case 168:  case 176:  case 184:  case 192:
		case 200:  case 208:  case 216:  case 224:  case 232:  case 240:  case 248:
			if (RENDERING_ENABLED)
				increment_address_horizontal();//*/
			break;

			//Inc vert(v)
		case 256:
			if (RENDERING_ENABLED)
			{
				increment_address_horizontal();
				increment_address_vertical();
			}//*/
			break;


			//If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:
		case 257:
			if (RENDERING_ENABLED)
			{
				VRAM_address = (VRAM_address & ~0x041F) | (VRAM_address_temp & 0x041F);
			}

		case 258: case 259: case 260: case 261: case 262: case 263: case 264:
		case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272:
		case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280:
		case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288:
		case 289: case 290: case 291: case 292: case 293: case 294: case 295: case 296:
		case 297: case 298: case 299: case 300: case 301: case 302: case 303: case 304:
		case 305: case 306: case 307: case 308: case 309: case 310: case 311: case 312:
		case 313: case 314: case 315: case 316: case 317: case 318: case 319: case 320:
			*reg.OAMADDR = 0;//OAMADDR is set to 0 during each of ticks 257 - 320 of the pre - render and visible scanlines

			//ver(v) = ver(t) if rendering is enabled.
			if (RENDERING_ENABLED && cycles >= 280 && cycles <= 304)
			{
				VRAM_address = (VRAM_address & ~0x7BE0) | (VRAM_address_temp & 0x7BE0);
			}
			break;


			//Fetches for next frame:
		case 321:  case 329:
			processed_nametable_byte = *memory[NAMETABLE_FETCH_ADDRESS];
			break;

		case 323:  case 331:
			//processed_attribute_table_byte = *memory[ATTRIBUTE_TABLE_FETCH_ADDRESS];
			processed_attribute_table_byte = ((*memory[ATTRIBUTE_TABLE_FETCH_ADDRESS] >> ((((VRAM_address >> 5)/*y*/ & 0x02) ? 4 : 0) + ((VRAM_address & 0x02)/*x*/ ? 2 : 0))) << 2) & 0x0c;
			break;


		case 325: case 333:
			processed_bitmap_low = *memory[BITMAP_LOW_FETCH_ADDRESS];
			break;

		case 327:  case 335:
			processed_bitmap_high = *memory[BITMAP_HIGH_FETCH_ADDRESS];
			break;

		case 328:  case 336:
			save_attribute_to_latch(processed_attribute_table_byte);
			save_bitmap_to_latch(processed_bitmap_low, processed_bitmap_high);
			if (RENDERING_ENABLED)
				increment_address_horizontal();//*/
			break;
		}
	}
}


void PPU::render_pixel(const unsigned char& x, const unsigned char& y)	//Ustawiam kolor piksela zgodnie z danymi pobranymi z odpowiedniego miejsca z palety systemowej
{
	static bool set_sprite_0_hit_flag_now = false;
	if (set_sprite_0_hit_flag_now)	//ustawia flage sprite 0 hit jeśli w poprzednim cyklu to ustalono
	{
		*reg.PPUSTATUS |= 0x40;
		set_sprite_0_hit_flag_now = false;
	}


	unsigned char background_color = 0;
	unsigned char sprite_color = 0;
	bool sprite_priority = false;	//Priority (0: in front of background; 1: behind background)


	//Background rendering:
	if (SHOW_BACKGROUND && !(!SHOW_LEFTMOST_BACKGROUND && x >= 0 && x <= 7))
	{
		unsigned short low_byte = bitmap_latch[0];
		unsigned short high_byte = bitmap_latch[1];
		unsigned short bitmap_offset = 0x8000 >> ((x % 8) + x_scroll);

		unsigned char palette_bits = ((x % 8) + x_scroll < 8 ? attribute_latch[0] : attribute_latch[1]);

		background_color = palette_bits | (high_byte & bitmap_offset ? 0x2 : 0) | (low_byte & bitmap_offset ? 0x1 : 0);

		//Addresses $3F04/$3F08/$3F0C can contain unique data, though these values are not used by the PPU when normally rendering = do renderingu tła używa tylko 0x3f00
		if (background_color % 4 == 0)
			background_color = 0;
	}


	//Sprite rendering:
	bool sprite_in_range = false;
	unsigned char sprite_index = 0;
	unsigned char i = 0;
	if (SHOW_SPRITES && !(!SHOW_LEFTMOST_SPRITES && x >= 0 && x <= 7))
	{
		do
		{
			for (; i < found_sprites; i++)
				if (x >= sprites_x_coordinate_latch[i] && x < (sprites_x_coordinate_latch[i] + 8))	//Szuka pierwszego spritea w obecnym cyklu(sprite o najnizszej pozycji w OAM ma wiekszy priorytet)
				{
					sprite_index = i;
					sprite_in_range = true;
					break;
				}

			if (sprite_in_range)
			{
				unsigned char bitmap_low = sprites_bitmap_latch[sprite_index][0];
				unsigned char bitmap_high = sprites_bitmap_latch[sprite_index][1];
				unsigned char attribute_byte = sprites_attributes_latch[sprite_index];
				unsigned char x_coordinate = sprites_x_coordinate_latch[sprite_index];
				unsigned char bitmap_offset = ((attribute_byte & 0x40) ? (0x01 << (x - x_coordinate)) : (0x80 >> (x - x_coordinate)));	//Flip horizontally
				sprite_priority = attribute_byte & 0x20;
				sprite_color = ((attribute_byte & 0x03) << 2) | (bitmap_high & bitmap_offset ? 0x2 : 0) | (bitmap_low & bitmap_offset ? 0x1 : 0);
			}
		} while (i++, (sprite_color & 0x03) == 0 && i < found_sprites);	//Jeżeli sprite, który jest najniższy w OAM(ma największy priorytet) zawiera w tym miejscu przezroczysty kolor, szukam kolejnego
				//inkrementuje i, na wypadek gdyby nie udało się znależć nieprzezroczystego spritea
	}




	if (sprite_in_range && found_sprite_zero && sprite_index == 0 &&	//wyświetla teraz sprite 0
		!(*reg.PPUSTATUS & 0x40) &&	//flaga sprite 0 hit nie jest jeszcze ustawiona
		(background_color & 0x03) && (sprite_color & 0x03) &&	//tło i sprite nie są przezroczyste
		x != 255)	//to nie jest ostatni piksel na scanline
	{
		set_sprite_0_hit_flag_now = true;	//Zamiast bezpośrednio ustawiać flagę, zaznaczam, że ustawię ją w następnym cyklu(żeby przejść testy sprite hit blargga), mógłbym też 1 cykl później przeładowywać rejestry(latch), który zawierają bitmapy, ale zostawiam jak jest teraz
	}




	const unsigned char temp_background_color = background_color & 0x03;
	const unsigned char temp_sprite_color = sprite_color & 0x03;


	if (!temp_background_color && !temp_sprite_color)
		screen->setPixelColor(x, y, color_palette[*memory[0x3f00 + background_color]]);
	else if (!temp_background_color && temp_sprite_color)
		screen->setPixelColor(x, y, color_palette[*memory[0x3f00 + 16 + sprite_color]]);
	else if (temp_background_color && !temp_sprite_color)
		screen->setPixelColor(x, y, color_palette[*memory[0x3f00 + background_color]]);
	else if (temp_background_color && temp_sprite_color && !sprite_priority)
		screen->setPixelColor(x, y, color_palette[*memory[0x3f00 + 16 + sprite_color]]);
	else if (temp_background_color && temp_sprite_color && sprite_priority)
		screen->setPixelColor(x, y, color_palette[*memory[0x3f00 + background_color]]);
}

void PPU::increment_cycles()
{
	total_cycles++;
	cycles++;			//Zawsze zwiększam cykl na początku, dlatego wielkości cykli w warunkach są w tej funkcji o 1 większe


	if (scanline == 261)
	{
		if (RENDERING_ENABLED)	//Rendering włączony = każda nieparzysta klatka jest krótsza o jeden cykl
		{
			if (frame % 2 /*&& cycles == 340*/)	//Dla nieparzystych klatek jeden cykl jest pomijany - wg testu "10-even_odd_timing" to 339 klatka
			{
				if (cycles == 341)
				{
					cycles = 0;
					scanline = 0;
					frame++;
					ready_to_render = true;
				}
				else if (cycles == 339)
					cycles = 340;
			}
			else if (!(frame % 2) && cycles == 341)	//W przypadku parzystych klatek wszystko jest normalnie
			{
				cycles = 0;
				scanline = 0;
				frame++;
				ready_to_render = true;
			}
		}
		else if (cycles == 341)	//Rendering wyłączony = wszystkie klatki mają taką samą długość(341 cykli)
		{
			cycles = 0;
			scanline = 0;
			frame++;
		}
	}
	else if (cycles == 341)	//Koniec scanline, początek nowej
	{
		scanline++;
		cycles = 0;
	}
}


void PPU::update()
{
	increment_cycles();

	step();


	sprite_evaluation();



}


void PPU::set_mirroring_mode(const nametable_mirroring_mode& mode)
{
	if (mode == nametable_mirroring_mode::vertical)//Vertical mirroring: $2000 equals $2800 and $2400 equals $2C00
	{
		for (int i = 0; i < 0x400; i++)
		{
			memory[0x2000 + i] = memory[0x2800 + i];
			memory[0x2400 + i] = memory[0x2c00 + i];
		}
		std::cout << "\nVERTICAL\n";
	}
	else if (mode == nametable_mirroring_mode::horizontal)//Horizontal mirroring: $2000 equals $2400 and $2800 equals
	{
		for (int i = 0; i < 0x400; i++)
		{
			memory[0x2000 + i] = memory[0x2400 + i];
			memory[0x2800 + i] = memory[0x2c00 + i];
		}
		std::cout << "\nHORIZONTAL\n";
	}
	else if (mode == nametable_mirroring_mode::one_screen)
	{
		for (int i = 0; i < 0x400; i++)
		{
			memory[0x2400 + i] = memory[0x2800 + i] = memory[0x2c00 + i] = memory[0x2000 + i];
		}
		std::cout << "\nONE_SCREEN";
	}
	else
		std::cout << "Unsupported mirroring type!";
}//*/


void PPU::sprite_evaluation()
{
	static unsigned char n = 0;
	static unsigned char m = 0;
	static unsigned char sprite_buffer = 0;
	static bool sprite_in_range = false;
	static bool n_overflowed = false;
	static bool set_overflow_flag = false;
	static unsigned char evaluation_sprites_counter = 0; //spritey znalezione podczas trwającego sprite evaluation(nastepne scanline), found_sprites to spritey ktore sa na tej scanline
	static bool evaluation_found_sprite_0 = false;	//oznacza czy podczas trwającego sprite evaluation znaleziono sprite 0(nastepna scanline), found_sprite_zero jest dla nastepnego scanline


	if (set_overflow_flag)	//Ustawia flagę sprite overflow po cykl, po wykryciu spritea, który ją wywołał - w taki sposób przechodzi test 03-timing.nes blargga
	{
		*reg.PPUSTATUS |= 0x20;	//ustawia flage sprite overflow
		set_overflow_flag = false;
	}

	if (cycles == 65)	//przygotowuje zmienne używane do sprite evaluation
	{
		n = 0;
		m = 0;
		evaluation_sprites_counter = 0;
		sprite_buffer = 0xff;
		sprite_in_range = false;
		n_overflowed = false;
	}
	else if (cycles == 256)
	{
		found_sprites = evaluation_sprites_counter;
		found_sprite_zero = evaluation_found_sprite_0;
	}

	if (RENDERING_ENABLED && scanline >= 0 && scanline <= 239)
		switch (cycles)
		{
		case 1:
			for (int i = 0; i < 32; i++)
				secondary_OAM[i] = 0xFF;
			break;

		case 65:
			n = 0;
			m = 0;
			evaluation_sprites_counter = 0;

			sprite_buffer = 0xff;
			sprite_in_range = false;
			n_overflowed = false;
			evaluation_found_sprite_0 = false;



			//Sprite evaluation
		case 66: case 67: case 68: case 69: case 70: case 71: case 72:
		case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80:
		case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88:
		case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96:
		case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104:
		case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112:
		case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120:
		case 121: case 122: case 123: case 124: case 125: case 126: case 127: case 128:
		case 129: case 130: case 131: case 132: case 133: case 134: case 135: case 136:
		case 137: case 138: case 139: case 140: case 141: case 142: case 143: case 144:
		case 145: case 146: case 147: case 148: case 149: case 150: case 151: case 152:
		case 153: case 154: case 155: case 156: case 157: case 158: case 159: case 160:
		case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168:
		case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176:
		case 177: case 178: case 179: case 180: case 181: case 182: case 183: case 184:
		case 185: case 186: case 187: case 188: case 189: case 190: case 191: case 192:
		case 193: case 194: case 195: case 196: case 197: case 198: case 199: case 200:
		case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208:
		case 209: case 210: case 211: case 212: case 213: case 214: case 215: case 216:
		case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224:
		case 225: case 226: case 227: case 228: case 229: case 230: case 231: case 232:
		case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240:
		case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248:
		case 249: case 250: case 251: case 252: case 253: case 254: case 255: case 256:

			if (cycles & 1)	//na nieparzystych cyklach, PPU odczytuje z OAM
				sprite_buffer = OAM[(n * 4) + m];
			else if (evaluation_sprites_counter < 8 && !n_overflowed)	//szuka do 8 spriteow w OAM
			{
				if (!sprite_in_range)	//szuka spriteow w zasiegu(w nastepnej scanline)
				{
					secondary_OAM[(evaluation_sprites_counter * 4) + m] = sprite_buffer;
					if (sprite_buffer < 240 && scanline >= sprite_buffer && scanline < sprite_buffer + (BIG_SPRITES ? 16 : 8))	//sprite w zasiegu
					{
						m += 1;	//m = 1
						sprite_in_range = true;

						if (n == 0)
							evaluation_found_sprite_0 = true;	//na nastepnej scanline trzeba sprawdzi sprite 0 hit
					}
					else
					{
						n = (n + 1) % 64;	//przechodzi do nastepnego spritea
						if (n == 0)	//n sie "przekrecilo", do 256 cyklu PPU bedzie probowało (nieudanie) odczytywac OAM[n][0] i inkrementowac n = nic nie trzeba robic
							n_overflowed = true;
					}
				}
				else if (sprite_in_range)	//kopiowanie bajtow znalezionego spritea z OAM do secondary_OAM
				{
					secondary_OAM[(evaluation_sprites_counter * 4) + m] = sprite_buffer;

					m = (m + 1) % 4;

					if (m == 0)	//m "przekreciło sie"
					{
						sprite_in_range = false;
						evaluation_sprites_counter += 1;

						n = (n + 1) % 64;
						if (n == 0)	//n sie "przekrecilo", do 256 cyklu PPU bedzie probowało (nieudanie) odczytywac OAM[n][0] i inkrementowac n = nic nie trzeba robic
							n_overflowed = true;
					}
				}
			}
			else if (evaluation_sprites_counter == 8 && !n_overflowed)	//8 spriteow znalezione, sprawdza kolejne, aby sterowac flaga sprite overflow
			{
				if (!sprite_in_range)
				{
					if (sprite_buffer < 240 && scanline >= sprite_buffer && scanline < sprite_buffer + (BIG_SPRITES ? 16 : 8))
					{
						set_overflow_flag = true;	//zamiast od razu ustawiać flagę sprite overflow, czeka jeszcze 1 cykl = w ten sposób przechodzi testy na sprite overflow (03-timing.nes by blargg)

						m += 1;//(m + 1) % 4;
						sprite_in_range = true;
					}
					else	//PPU hardware bug:
					{
						m = (m + 1) % 4;
						n = (n + 1) % 64;
						if (n == 0)
							n_overflowed = true;
					}
				}
				else if (sprite_in_range)	//odczytuje kolejne bajty spritea ktory przepelnil limit spriteow
				{
					m = (m + 1) % 4;	//nastepny bajt

					if (m == 0)
					{
						//evaluation_sprites_counter += 1;	//nie zwiekszam licznika spriteow - potem moge go latwo wykoszystac przy renderowaniu prawdziwych spriteow
						n = (n + 1) % 64;
					}
				}
			}
			break;
		}
}

