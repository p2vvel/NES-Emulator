#pragma once


#include<SFML/Graphics.hpp>
#include "Screen.h"

class PPU;
struct memory_map_PPU;	//Deklaracja tego, co w PPU, zeby plik naglowkowy PPU dolaczyc dopiero we wlasciwym pliku z deklaracjami(brak rekurencyjnego zawierania sie plikow)

class palette_debug_window
{
	sf::RenderWindow* window;
	Screen palette_screen;
	const unsigned char  * color_indexes;
public:
	palette_debug_window(const  PPU &ppu, const unsigned short& x = 40);
	palette_debug_window(const unsigned short& x = 40);
	~palette_debug_window();
	void connect_PPU(const  PPU& ppu);

	void update();

};



class pattern_table_debug_window
{
	sf::RenderWindow* window;
	Screen pattern_table_screen;
	const unsigned char* pattern_table_address[2];
public:
	pattern_table_debug_window();
	pattern_table_debug_window(const  PPU& ppu);
	~pattern_table_debug_window();
	void connect_PPU(const  PPU& ppu);

	void update();

};


class nametable_debug_window
{
	sf::RenderWindow* window;
	Screen nametable_screen;
	const unsigned char* nametable_address[4];
	const unsigned char* pattern_table_address[2];
	unsigned char** ppu_memory;
public:
	nametable_debug_window();
	nametable_debug_window(PPU& ppu);
	~nametable_debug_window();
	void connect_PPU(PPU& ppu);

	void update(const bool &four_nametables_mode = false);

};

class oam_debug_window
{
	sf::RenderWindow* window;
	Screen oam_screen;
	const unsigned char* oam_address;
	unsigned char** ppu_memory;
	unsigned char* ppuctrl;
	const unsigned char* palette_address;
public:
	oam_debug_window();
	oam_debug_window(PPU& ppu);
	~oam_debug_window();
	void connect_PPU(PPU& ppu);

	void update(const bool& big_sprites = false);

};