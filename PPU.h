#pragma once
#include<SFML/Graphics.hpp>
#include "mapper_base.h"


//#define DONKEY_KONG_PALETTE

class Screen;
class palette_debug_window;

//Controller($2000) > write
//Common name : PPUCTRL
//7  bit  0
//---- ----
//VPHB SINN
//|||| ||||
//|||| ||++ - Base nametable address
//|||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
//|||| |+-- - VRAM address increment per CPU read / write of PPUDATA
//|||| |     (0: add 1, going across; 1: add 32, going down)
//|||| +----Sprite pattern table address for 8x8 sprites
//||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
//|||+------Background pattern table address(0: $0000; 1: $1000)
//|| +------ - Sprite size(0: 8x8; 1: 8x16)
//| +--------PPU master / slave select
//| (0: read backdrop from EXT pins; 1: output color on EXT pins)
//+ -------- - Generate an NMI at the start of the
//vertical blanking interval(0: off; 1: on)
#define NAMETABLE_ADDRESS (unsigned short )(0x2000 + (0x400 * (*reg.PPUCTRL & 0x03)))
#define ADDRESS_INCREMENT (unsigned char )((*reg.PPUCTRL & 0x04) ? 32 : 1)
#define PATTERN_TABLE_SPRITES_ADDRESS (unsigned short ) ((*reg.PPUCTRL & 0x08) ? 0x1000 : 0x0000)
#define PATTERN_TABLE_BACKGROUND_ADDRESS (unsigned short ) ((*reg.PPUCTRL & 0x10) ? 0x1000 : 0x0000)
#define BIG_SPRITES (bool )(*reg.PPUCTRL & 0x20)//0: 8x8; 1: 8x16
#define NMI_GENERATE (bool )(*reg.PPUCTRL & 0x80)


//Mask ($2001) > write
//Common name: PPUMASK
//7  bit  0
//---- ----
//BGRs bMmG
//|||| ||||
//|||| |||+-Greyscale(0: normal color, 1 : produce a greyscale display)
//|||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0 : Hide
//|||| |+-- - 1 : Show sprites in leftmost 8 pixels of screen, 0 : Hide
//|||| +---- 1 : Show background
//|||+------ 1 : Show sprites
//||+------Intensify greens(and darken other colors)
//|+-------Intensify blues(and darken other colors)
//+ --------Intensify reds(and darken other colors)
#define GREYSCALE (bool )(*reg.PPUMASK & 0x01)
#define LEFTMOST_BACKGROUND (bool )(*reg.PPUMASK & 0x02)
#define LEFTMOST_SPRITES (bool )(*reg.PPUMASK & 0x04)
#define SHOW_BACKGROUND (bool )(*reg.PPUMASK & 0x08)
#define SHOW_SPRITES (bool)(*reg.PPUMASK & 0x10)


//Status($2002) < read
//Common name : PPUSTATUS
//7  bit  0
//---- ----
//VSO. ....
//|||| ||||
//|||+-++++ - Least significant bits previously written into a PPU register
//|||			(due to register not being updated for this address)
//|| +------ - Sprite overflow.The intent was for this flag to be set
//||			 whenever more than eight sprites appear on a scanline, but a
//||			 hardware bug causes the actual behavior to be more complicated
//||			 and generate false positives as well as false negatives; see
//||			 PPU sprite evaluation.This flag is set during sprite
//||			 evaluation and cleared at dot 1 (the second dot) of the
//||			 pre - render line.
//| +--------Sprite 0 Hit.Set when a nonzero pixel of sprite 0 overlaps
//|			a nonzero background pixel; cleared at dot 1 of the pre - render
//|			line.Used for raster timing.
//+ -------- - Vertical blank has started(0: not in vblank; 1: in vblank).
//				Set at dot 1 of line 241 (the line *after* the post - render
//				line); cleared after reading $2002 and at dot 1 of the
//				pre - render line.
#define SPRITE_OVERFLOW (bool )(*reg.PPUSTATUS & 0x20)
#define SPRITE_ZERO_HIT (bool )(*reg.PPUSTATUS & 0x40)
#define VBLANK (bool )(*reg.PPUSTATUS & 0x80)


class CPU;

static const sf::Color color_palette[64] =
{
	sf::Color(124,124,124),
	sf::Color(0,0,252),
	sf::Color(0,0,188),
	sf::Color(68,40,188),
	sf::Color(148,0,132),
	sf::Color(168,0,32),
	sf::Color(168,16,0),
	sf::Color(136,20,0),
	sf::Color(80,48,0),
	sf::Color(0,120,0),
	sf::Color(0,104,0),
	sf::Color(0,88,0),
	sf::Color(0,64,88),
	sf::Color(0,0,0),
	sf::Color(0,0,0),
	sf::Color(0,0,0),
	sf::Color(188,188,188),
	sf::Color(0,120,248),
	sf::Color(0,88,248),
	sf::Color(104,68,252),
	sf::Color(216,0,204),
	sf::Color(228,0,88),
	sf::Color(248,56,0),
	sf::Color(228,92,16),
	sf::Color(172,124,0),
	sf::Color(0,184,0),
	sf::Color(0,168,0),
	sf::Color(0,168,68),
	sf::Color(0,136,136),
	sf::Color(0,0,0),
	sf::Color(0,0,0),
	sf::Color(0,0,0),
	sf::Color(248,248,248),
	sf::Color(60,188,252),
	sf::Color(104,136,252),
	sf::Color(152,120,248),
	sf::Color(248,120,248),
	sf::Color(248,88,152),
	sf::Color(248,120,88),
	sf::Color(252,160,68),
	sf::Color(248,184,0),
	sf::Color(184,248,24),
	sf::Color(88,216,84),
	sf::Color(88,248,152),
	sf::Color(0,232,216),
	sf::Color(120,120,120),
	sf::Color(0,0,0),
	sf::Color(0,0,0),
	sf::Color(252,252,252),
	sf::Color(164,228,252),
	sf::Color(184,184,248),
	sf::Color(216,184,248),
	sf::Color(248,184,248),
	sf::Color(248,164,192),
	sf::Color(240,208,176),
	sf::Color(252,224,168),
	sf::Color(248,216,120),
	sf::Color(216,248,120),
	sf::Color(184,248,184),
	sf::Color(184,248,216),
	sf::Color(0,252,252),
	sf::Color(248,216,248),
	sf::Color(0,0,0),
	sf::Color(0,0,0)
};



struct registers_PPU
{
	//PPU Registers
	//Common Name	Adress	Bits
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//PPUCTRL		$2000 	VPHB SINN 	NMI enable(V), PPU master / slave(P), sprite height(H), background tile select(B), sprite tile select(S), increment mode(I), nametable select(NN)
	//PPUMASK 		$2001 	BGRs bMmG 	color emphasis(BGR), sprite enable(s), background enable(b), sprite left column enable(M), background left column enable(m), greyscale(G)
	//PPUSTATUS 	$2002 	VSO - ----vblank(V), sprite 0 hit(S), sprite overflow(O), read resets write pair for $2005 / 2006
	//OAMADDR		$2003 	aaaa aaaa 	OAM read / write address
	//OAMDATA		$2004 	dddd dddd 	OAM data read / write
	//PPUSCROLL		$2005 	xxxx xxxx 	fine scroll position(two writes : X, Y)
	//PPUADDR		$2006 	aaaa aaaa 	PPU read / write address(two writes : MSB, LSB)
	//PPUDATA		$2007 	dddd dddd 	PPU data read / write
	//OAMDMA		$4014 	aaaa aaaa 	OAM DMA high address


	unsigned char *PPUCTRL;//0x2000 - write
	unsigned char *PPUMASK;//0x2001 - write
	unsigned char *PPUSTATUS;//0x2002 - read
	unsigned char *OAMADDR;//0x2003 -
	unsigned char *OAMDATA;//0x2004
	unsigned char *PPUSCROLL;//0x2005
	unsigned char *PPUADDR;//0x2006
	unsigned char *PPUDATA;//0x2007
	unsigned char *OAMDMA;//0x4014
};


struct memory_map_PPU
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

	unsigned char pattern_table_0[0x1000];
	unsigned char pattern_table_1[0x1000];
	unsigned char nametable_0[0x400];
	unsigned char nametable_1[0x400];
	unsigned char nametable_2[0x400];
	unsigned char nametable_3[0x400];
	unsigned char palette_RAM_indexes[0x20];
};


class PPU
{

	unsigned char* CPU_memory[0xFFFF];	//WskaŸniki, w których uporz¹dkowana jest ca³a pamiêæ. Jest to kopia wskaŸników z klasy CPU, która pozwoli na swobodny dostêp do pamiêci.


	unsigned char* memory[0x4000];//WskaŸniki, do których jest odpowiednio przypisana pamiêæ(z uwzglêdnieniem mirroringu)

	CPU *cpu;
	Screen *screen;

	//Udostêpnia pamiêæ do klasy bazowej mapperów
	friend void mapper_base::initialize_PPU_memory(const PPU &ppu);
	




	//The PPU contains the following :
	//a)	Background:
		//v - Current VRAM address(15 bits)
	unsigned short VRAM_address;//	VRAM address,
		//t	- Temporary VRAM address(15 bits); can also be thought of as the address of the top left onscreen tile.
	unsigned short VRAM_address_temp;//temporary VRAM address, 
		//x	- Fine X scroll(3 bits)
	unsigned char X_scroll;//fine X scroll, 
		//w	- First or second write toggle(1 bit)
	bool write_toggle;//and first / second write toggle - This controls the addresses that the PPU reads during background rendering.See PPU scrolling.
				//Warunkuje, czy wykonaæ pierwszy czy ostatni z dwóch zapisów do $2005 i $2006
	unsigned short shift_registers_background_bitmap[2];//	2 16 - bit shift registers - These contain the bitmap data for two tiles.Every 8 cycles, the bitmap data for the next tile is loaded into the upper 8 bits of this shift register.Meanwhile, the pixel to render is fetched from one of the lower 8 bits.
	unsigned char shift_registers_background_attribute[2];//	2 8 - bit shift registers - These contain the palette attributes for the lower 8 pixels of the 16 - bit shift register.These registers are fed by a latch which contains the palette attribute for the next tile.Every 8 cycles, the latch is loaded with the palette attribute for the next tile.
	//b)	Sprites:
	//	OAM i secondary OAM:
	//	Byte 0: Y position
	//	Byte 1: tile index number
	//	Byte 2: Attributes
	//	Byte 3: X position
	unsigned char OAM[256];//	Primary OAM(holds 64 sprites for the frame)
	unsigned char secondary_OAM[32];//	Secondary OAM(holds 8 sprites for the current scanline)
	unsigned short shift_registers_sprite_bitmap[8];//	8 pairs of 8 - bit shift registers - These contain the bitmap data for up to 8 sprites, to be rendered on the current scanline.Unused sprites are loaded with an all - transparent bitmap.
	unsigned char shift_registers_sprite_attribute[8];//	8 latches - These contain the attribute bytes for up to 8 sprites.
	unsigned char shift_registers_sprite_x[8];//	8 counters - These contain the X positions for up to 8 sprites.


	unsigned char output_attribute_table;
	unsigned short output_bitmap;
	unsigned char output_pattern_table_low;
	unsigned char output_pattern_table_high;



	template<typename T>
	void shift_register(T *shift_register, const T &new_value)
	{
		shift_register[0] = shift_register[1];
		shift_register[1] = new_value;
	}

	template<typename T>
	const T &get_shift_register_value(const T *shift_register)
	{
		return shift_register[0];
	}

	
	bool ready_to_render;


	//unsigned char temp_nametable_data;


	registers_PPU reg;//Rejestry
	memory_map_PPU mem;//W³aœciwa pamiêæ. Zostaje póŸniej przypisana do wskaŸników "memory"


	unsigned short scanline;//numer scanline
	unsigned short cycles;//numer cyklu w scanline
	unsigned long long total_cycles;
	unsigned long long frame;//numer klatki


	unsigned char x_position;	//Do scrollingu
	unsigned char y_position;	//Do scrollingu

	
	
	void initialize_memory();
	void DMA();
	void sprite_evaluation();	//Wyszukuje spritey dla nastêpnej scanline
	void rendering();	//Renderuje aktualn¹ scanline

	void render_pixel();

	
	
	void render_sprites();	//Odpowiednio "wrysowuje" spritey w miejsce t³a dla aktualnej scanline

	const unsigned char flip_sprite_horizontally(const unsigned char &bitmap_part);
	//trzeba powtórzyæ to dla low_bitmap i high_bitmap

	void increment_cycles();


	bool DMA_activated;



	void increment_X_coarse();

	void increment_Y();

	void control_registers();

	void update_VRAM_address();
	
	
public:
	PPU();
	~PPU();
	void initialize_registers(CPU &cpu_src);
	void connect_screen(Screen &scr) { screen = &scr; }
	void activate_DMA() { DMA_activated = true; DMA();  }

	void step();
	const long long &get_total_cycles() { return total_cycles; }
	const unsigned short &get_cycles() { return cycles; }

	const unsigned short &get_scanline() { return scanline; }
	const unsigned long long &get_frame() { return frame;  }

	const bool isReadyToRender();
	


	const bool show_background() { return SHOW_BACKGROUND; }
	const bool show_sprites() { return SHOW_SPRITES; }
	

	void update_2002(const unsigned char &value) { *reg.PPUSTATUS = (*reg.PPUSTATUS & 0xe0) | (value & 0x1f); }	//Aktualizuje rejestr $2002 - jego piêæ najmniej znacz¹cych bitów, to najmniej znacz¹ce bity ostatnich danych wpisanych do rejestrów

	const unsigned char read_2002();
	const unsigned char read_2004();
	const unsigned char read_2007();
	void write_2000(const unsigned char &value);
	void write_2004(const unsigned char &value);
	void write_2005(const unsigned char &value);
	void write_2006(const unsigned char &value);
	void write_2007(const unsigned char &value);


	const unsigned char* get_palette() const { return (mem.palette_RAM_indexes); };

	const unsigned char* get_nametable(const unsigned char& nametable_index) const
	{
		switch (nametable_index)
		{
		case 0:
			return mem.nametable_0;
		case 1:
			return mem.nametable_1;
		case 2:
			return mem.nametable_2;
		case 3:
			return mem.nametable_3;
		default:
			return nullptr;
		}
	}

	const unsigned char* get_pattern_table(const unsigned char& pattern_table_index) const
	{
		switch (pattern_table_index)
		{
		case 0:
			return mem.pattern_table_0;
		case 1:
			return mem.pattern_table_1;
		default:
			return nullptr;
		}
	}

	 memory_map_PPU  *get_memory_map()  { return &mem; }


	void PPU::save_memory_state(unsigned short line_limiter = 0xf)
	{
		std::ofstream fout = std::ofstream("../logs/memory_state_PPU.txt", std::ios_base::out);

		for (unsigned int i = 0; i < 0x4000; i++)
		{
			
			if (i % (line_limiter + 1) == 0)
				fout << std::uppercase<<(i != 0 ? "\n" : "")<<"0x00"<< std::hex<<0x00D6D3C8 + (i & ~0xf)<<"  ";// << (i & ~0xf) << "\t";
			fout << (*memory[i] < 0x10 ? "0" : "")<<std::hex<<std::nouppercase<<(unsigned short)* memory[i] << " ";

		}//*/

		for (int i = 0; i < 0x4000; i++)
		{
			/*//if (*memory[i] != '\0')
				fout << *memory[i];
			else
				fout << (unsigned char)0x20;//*/
		}

		fout.close();
	}
};

