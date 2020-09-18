#include "PPU_debug.h"
#include "PPU.h"


palette_debug_window::palette_debug_window(const  PPU& ppu, const unsigned short& x)
{
	window = new sf::RenderWindow(sf::VideoMode(16 * x, 2 * x), "Palette debug window");
	window->setFramerateLimit(15);

	palette_screen = Screen(16, 2, x);
	connect_PPU(ppu);
};

palette_debug_window::palette_debug_window(const unsigned short& x)
{
	window = new sf::RenderWindow(sf::VideoMode(16 * x, 2 * x), "Palette debug window");
	window->setFramerateLimit(60);

	palette_screen = Screen(16, 2, x);
};

palette_debug_window::~palette_debug_window()
{
	if (window != nullptr)
		delete window;
	/*delete[] color_tiles; //*/
}

void palette_debug_window::update()
{
	if (window == nullptr)
		return;

	sf::Event ev;


	window->clear();
	for (int i = 0; i < 32; i++)
		palette_screen.setPixelColor((i % 16), (i / 16), color_palette[color_indexes[i]]);
	palette_screen.display(*window);
	window->display();


	window->pollEvent(ev);
	if (ev.type == sf::Event::Closed)
	{
		window->close();
		delete window;
		window = nullptr;
	}
}

void palette_debug_window::connect_PPU(const PPU& ppu)
{
	color_indexes = ppu.get_palette();
	//color_indexes = (ppu.get_memory_map()).palette_RAM_indexes);//ppu.get_palette_indexes();
}

//#################################################################
//#################################################################
//#################################################################




pattern_table_debug_window::pattern_table_debug_window()
{
	window = new sf::RenderWindow(sf::VideoMode(16 * 8 * 2 * 2 * 2, 16 * 8 * 2 * 2), "Pattern table debug window");
	window->setFramerateLimit(15);
	pattern_table_screen = Screen(16 * 8 * 2 * 2, 16 * 8 * 2, 4);
}

pattern_table_debug_window::pattern_table_debug_window(const  PPU& ppu) : pattern_table_debug_window()
{
	/*window = new sf::RenderWindow(sf::VideoMode(16 * 8 * 2 * 2, 16 * 8 * 2), "Pattern table debug window");
	window->setFramerateLimit(15);
	nametable_screen = Screen(256 * 2, 240 * 2);//*/
	connect_PPU(ppu);
}

pattern_table_debug_window::~pattern_table_debug_window()
{
	delete window;
}

void pattern_table_debug_window::connect_PPU(const  PPU& ppu)
{
	for (int i = 0; i < 2; i++)
		pattern_table_address[i] = ppu.get_pattern_table(i);
}

void pattern_table_debug_window::update()
{
	if (window == nullptr)
		return;

	sf::Event ev;

	window->clear();



	for (int i = 0; i < 256; i++)
	{
		unsigned short tile_x_pos = (i % 16);
		unsigned short tile_y_pos = (i / 16);

		//Left side = 0x0000 - 0x0FFF
		for (int y = 0; y < 8; y++)
		{
			unsigned char low_byte = pattern_table_address[0][(16 * i) + y];
			unsigned char high_byte = pattern_table_address[0][(16 * i) + 8 + y];

			for (int x = 0; x < 8; x++)
			{
				unsigned char temp = 0x80 >> x;
				unsigned char color = (high_byte & temp ? 0x2 : 0) + (low_byte & temp ? 0x1 : 0);

				switch (color)
				{
				case 0:
					color = 0x0f;
					break;
				case 1:
					color = 0x15;
					break;
				case 2:
					color = 0x2c;
					break;
				case 3:
					color = 0x12;
					break;
				}

				pattern_table_screen.setPixelColor((tile_x_pos * 8) + x, (tile_y_pos * 8) + y, color_palette[color]);
			}
		}

		//Right = 0x1000 - 0x1FFF
		for (int y = 0; y < 8; y++)
		{
			unsigned char low_byte = pattern_table_address[1][(16 * i) + y];
			unsigned char high_byte = pattern_table_address[1][(16 * i) + 8 + y];

			for (int x = 0; x < 8; x++)
			{
				unsigned char temp = 0x80 >> x;
				unsigned char color = (high_byte & temp ? 0x2 : 0) + (low_byte & temp ? 0x1 : 0);

				switch (color)
				{
				case 0:
					color = 0x0f;
					break;
				case 1:
					color = 0x15;
					break;
				case 2:
					color = 0x2c;
					break;
				case 3:
					color = 0x12;
					break;
				}

				pattern_table_screen.setPixelColor(((16 + tile_x_pos) * 8) + x, (tile_y_pos * 8) + y, color_palette[color]);
			}
		}
	}

	pattern_table_screen.display(*window);



	window->display();

	window->pollEvent(ev);
	if (ev.type == sf::Event::Closed)
	{
		window->close();
		delete window;
		window = nullptr;
	}
}

//#################################################################
//#################################################################
//#################################################################


nametable_debug_window::nametable_debug_window()
{
	window = new sf::RenderWindow(sf::VideoMode(256 * 2 * 2, 240 * 2 * 2), "Nametable debug window");
	nametable_screen = Screen(256 * 2, 240 * 2);
	window->setFramerateLimit(60);
}

nametable_debug_window::nametable_debug_window(PPU& ppu) : nametable_debug_window()
{
	connect_PPU(ppu);
}

nametable_debug_window::~nametable_debug_window()
{
	delete window;
}

void nametable_debug_window::connect_PPU(PPU& ppu)
{
	for (int i = 0; i < 4; i++)
		nametable_address[i] = ppu.get_nametable(i);

	for (int i = 0; i < 2; i++)
		pattern_table_address[i] = ppu.get_pattern_table(i);
	ppu_memory = ppu.get_memory();
}

void nametable_debug_window::update(const bool& four_nametables_mode)
{
	if (window == nullptr)
		return;

	sf::Event ev;

	window->clear();


	//nametable_address[j][i];



	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 960; i++)
		{
			unsigned char tile_index = (four_nametables_mode ? nametable_address[j][i] : *ppu_memory[0x2000 + (0x400 * j) + i]);
			unsigned short tile_x_pos = (i % 32);
			unsigned short tile_y_pos = (i / 32);

			unsigned char attribute_byte = *ppu_memory[0x2000 + (j * 0x400) + 0x3c0 + (((tile_y_pos / 4) * 8) + (tile_x_pos / 4))];

			const unsigned char attribute_offset = (tile_y_pos % 4 >= 2 ? 4 : 0) + (tile_x_pos % 4 >= 2  ? 2 : 0);	//Attribute_byte = 2b(dó³ prawo) + 2b(dó³ lewo) + 2b(góra prawo) + 2b(góra lewo)
			attribute_byte = (attribute_byte >> attribute_offset) & 0x3;

			for (int y = 0; y < 8; y++)
			{
				unsigned char low_byte = pattern_table_address[1][(16 * tile_index) + y];
				unsigned char high_byte = pattern_table_address[1][(16 * tile_index) + 8 + y];

				for (int x = 0; x < 8; x++)
				{
					unsigned char temp = 0x80 >> x;
					unsigned char color = (attribute_byte << 2) | (high_byte & temp ? 0x2 : 0) + (low_byte & temp ? 0x1 : 0);
					
					if (color % 4 == 0)
						color = 0;

					nametable_screen.setPixelColor(((tile_x_pos) * 8) + x + (j & 0x1 ? 256 : 0), (tile_y_pos * 8) + y + (j / 2 ? 240 : 0), color_palette[*ppu_memory[0x3f00 + color]]);
				}

			}
		}
	}




	nametable_screen.display(*window);


	window->display();

	window->pollEvent(ev);
	if (ev.type == sf::Event::Closed)
	{
		window->close();
		delete window;
		window = nullptr;
	}
}




//#################################################################
//#################################################################
//#################################################################



oam_debug_window::oam_debug_window()
{
	window = new sf::RenderWindow(sf::VideoMode(8 * 8 * 4, 8 * 16 * 4), "OAM debug window");
	window->setFramerateLimit(60);
	oam_screen = Screen(8 * 8, 8 * 16, 4);
}

oam_debug_window::oam_debug_window(PPU& ppu) : oam_debug_window()
{
	connect_PPU(ppu);
}

oam_debug_window::~oam_debug_window()
{
	delete window;
}


void oam_debug_window::connect_PPU(PPU& ppu)
{
	oam_address = ppu.get_OAM();
	ppu_memory = ppu.get_memory();
	ppuctrl = ppu.get_registers()->PPUCTRL;
	palette_address = ppu.get_palette();
}

void oam_debug_window::update(const bool& big_sprites)
{
	if (window == nullptr)
		return;

	sf::Event ev;

	window->clear();
	oam_screen.resetScreen();

	//OAM:
	//0. Y position
	//1. Tile index number
	//2. Attributes
	//3. X position

	for (int i = 0; i < 64; i++)
	{

		const unsigned char sprite_tile_index = oam_address[(i * 4) + 1];
		const unsigned char sprite_attributes = oam_address[(i * 4) + 2];

		if ((*ppuctrl & 0x20))	//BIG SPRITES: 16x8
		{
			for (unsigned char y = 0; y < 16; y++)
			{
				const unsigned short sprite_bitmap_low_address = ((sprite_tile_index & 0x01 ? 0x1000 : 0x0000) | ((unsigned short)(sprite_tile_index & 0xfe) << 4) | (y));
				const unsigned char bitmap_low = *ppu_memory[sprite_bitmap_low_address];
				const unsigned char bitmap_high = *ppu_memory[sprite_bitmap_low_address + 16];
				for (unsigned char x = 0; x < 8; x++)
				{
					const unsigned char bitmap_offset = 0x80 >> x;
					const unsigned char color = ((sprite_attributes & 0x03) << 2) | (bitmap_high & bitmap_offset ? 0x2 : 0) | (bitmap_low & bitmap_offset ? 0x1 : 0);
					oam_screen.setPixelColor((8 * (i % 8)) + x, (16 * (i / 8)) + y, color_palette[palette_address[color + 16]]);
				}
			}
		}
		else	//SPRITES 8x8
		{
			for (unsigned char y = 0; y < 8; y++)
			{
				const unsigned short sprite_bitmap_low_address = ((unsigned short)((*ppuctrl & 0x08) ? 0x1000 : 0x0000) | ((unsigned short)sprite_tile_index << 4) | (y));
				const unsigned bitmap_low = *ppu_memory[sprite_bitmap_low_address];
				const unsigned char bitmap_high = *ppu_memory[sprite_bitmap_low_address + 8];
				for (unsigned char x = 0; x < 8; x++)
				{
					const unsigned char bitmap_offset = 0x80 >> x;
					unsigned char color = 0;

					if(((bitmap_high & bitmap_offset ? 0x2 : 0) | (bitmap_low & bitmap_offset ? 0x1 : 0)))
						color = ((sprite_attributes & 0x03) << 2) | (bitmap_high & bitmap_offset ? 0x2 : 0) | (bitmap_low & bitmap_offset ? 0x1 : 0);
					
					oam_screen.setPixelColor((8 * (i % 8)) + x, (16 * (i / 8)) + y, color_palette[palette_address[(color ? color + 16 : 0)]]);
				}
			}

		}
	}


	oam_screen.display(*window);


	window->display();

	window->pollEvent(ev);
	if (ev.type == sf::Event::Closed)
	{
		window->close();
		delete window;
		window = nullptr;
	}
}
