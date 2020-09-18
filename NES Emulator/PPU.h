#pragma once
#include<SFML/Graphics.hpp>
#include "mapper_base.h"



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
#define SHOW_LEFTMOST_BACKGROUND (bool )(*reg.PPUMASK & 0x02)
#define SHOW_LEFTMOST_SPRITES (bool )(*reg.PPUMASK & 0x04)
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


#define RENDERING_ENABLED (SHOW_BACKGROUND || SHOW_SPRITES)

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


enum nametable_mirroring_mode { horizontal, vertical, one_screen_low, one_screen_high, four_screen };


class PPU
{
	CPU* cpu;
	Screen* screen;

	unsigned char* CPU_memory[0xFFFF];	//WskaŸniki, w których uporz¹dkowana jest ca³a pamiêæ. Jest to kopia wskaŸników z klasy CPU, która pozwoli na swobodny dostêp do pamiêci.
	unsigned char* memory[0x4000];//WskaŸniki, do których jest odpowiednio przypisana pamiêæ(z uwzglêdnieniem mirroringu)

	registers_PPU reg;//Rejestry
	memory_map_PPU mem;//W³aœciwa pamiêæ. Zostaje póŸniej przypisana do wskaŸników "memory"

	friend void mapper_base::initialize_PPU_memory(const PPU& ppu);	//Udostêpnia pamiêæ do klasy bazowej mapperów umozliwiajac wczytywanie romow
	

	

	unsigned short VRAM_address;
	unsigned short VRAM_address_temp;

	unsigned char x_scroll;

	bool write_toggle;



	unsigned char processed_nametable_byte;
	unsigned char processed_attribute_table_byte;
	unsigned char processed_bitmap_low;
	unsigned char processed_bitmap_high;





	unsigned short bitmap_latch[2];	//[0] = low_bitmap, [1] = high_bitmap
	inline void save_bitmap_to_latch(const unsigned char& bitmap_low, const unsigned char& bitmap_high)
	{
		bitmap_latch[0] = (bitmap_latch[0] << 8) | bitmap_low;
		bitmap_latch[1] = (bitmap_latch[1] << 8) | bitmap_high;
	}
	
	unsigned char attribute_latch[2];
	inline void save_attribute_to_latch(const unsigned char& attribute)
	{
		attribute_latch[0] = attribute_latch[1];
		attribute_latch[1] = attribute;
	}

	
	unsigned char OAM[256];	//Primary OAM(holds 64 sprites for the frame)
	unsigned char secondary_OAM[32];	//Secondary OAM(holds 8 sprites for the current scanline)


	unsigned char sprites_bitmap_latch[8][2]; //[x][0] = low_bitmap, [x][1] = high_bitmap
	unsigned char sprites_attributes_latch[8];
	unsigned char sprites_x_coordinate_latch[8];

	unsigned char found_sprites;

	
	bool ready_to_render;



	unsigned short scanline;//numer scanline
	unsigned short cycles;//numer cyklu w scanline
	unsigned long long total_cycles;
	unsigned long long frame;//numer klatki



	void initialize_memory();
	void DMA();
	
	bool DMA_activated;


	void increment_address();
	void increment_address_horizontal();
	void increment_address_vertical();


	unsigned char internal_buffer_2007;

	

	bool NMI_output;
	bool NMI_occured;


	bool found_sprite_zero;

	void step();
	void sprite_evaluation();
	void increment_cycles();
	
public:
	PPU();
	~PPU();
	void initialize_registers(CPU &cpu_src);
	void connect_screen(Screen &scr) { screen = &scr; }
	void activate_DMA() { DMA_activated = true; DMA();  }

	
	void update();
	void render_pixel(const unsigned char& x, const unsigned char& y);
	
	const long long &get_total_cycles() { return total_cycles; }
	const unsigned short &get_cycles() { return cycles; }
	const unsigned short &get_scanline() { return scanline; }
	const unsigned long long &get_frame() { return frame;  }

	const bool isReadyToRender();
	void set_mirroring_mode(const nametable_mirroring_mode& mode);

	

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

	const unsigned char* get_OAM() const { return OAM; }
	const registers_PPU* get_registers() const { return &reg;  }
	memory_map_PPU  *get_memory_map()  { return &mem; }
	unsigned char** get_memory() { return memory; }


};

