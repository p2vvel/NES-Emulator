#include<iostream>
#include<SFML/Graphics.hpp>
#include "CPU.h"
#include "PPU.h"
#include "mapper_base.h"
#include "mapper_0.h"
#include "Screen.h"
#include "PPU_debug.h"
#include "controller_base.h"



//#define DEBUG_PPU



int main(int argc, char* argv[])
{
	std::string ROM_path;
	if (argc > 1)
		ROM_path = argv[1];
	else
		ROM_path = "Super Mario Bros. (JU) (PRG0) [!].nes";//"donkey_kong.nes";//"allpads.nes";

	sf::RenderWindow win(sf::VideoMode(256 * 2, 240 * 2), "cojoNES");
	win.setFramerateLimit(60);
	Screen screen;
	CPU cpu;
	PPU ppu;
	controller_base* pad = new controller_base();
	cpu.connect_controller(pad);
	cpu.connect_PPU(ppu);
	ppu.connect_screen(screen);
	ppu.initialize_registers(cpu);
	mapper_base* mapper = nullptr;

	const short mapper_number = mapper_base::verify_header(ROM_path);
	if (mapper_number == -1)
	{
		"\nCould't load game!";
		std::cin.get();
		return -1;
	}
	else
	{
		switch (mapper_number)
		{
		case 0:
			mapper = new mapper_0(ROM_path, cpu, ppu);
			break;
		default:
			std::cout << "\nMapper " << mapper_number << " hasn't been implemented yet, or isn't proper NES mapper!";
			std::cin.get();
			return -2;
		}
		cpu.initializeProgramCounter();	//Ustawia PC zgodnie z odpowiednimi informacjami w PRG-ROM za³adowanymi do RAMu
	}


#ifdef DEBUG_PPU
	//palette_debug_window color_debug(ppu, 50);
	//pattern_table_debug_window pattern_table_debug(ppu);
	nametable_debug_window nametable_debug(ppu);
	//oam_debug_window oam_debug(ppu);
#endif





	while (win.isOpen())
	{
		sf::Event ev;
		while (win.pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed || (ev.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)))
			{
				delete mapper;
				win.close();
				return 0;
			}
		}



		win.clear();


		while (!ppu.isReadyToRender())
		{
			cpu.emulate_cycle();

			if (pad->get_is_polling())
					pad->update_controller_latch();
				

			while ((cpu.get_total_cycles() - 1) * 3 > ppu.get_total_cycles())
				ppu.update();
		}


#ifdef DEBUG_PPU
		//color_debug.update();
		//pattern_table_debug.update();
		nametable_debug.update(true);
		//oam_debug.update();
#endif


		mapper->control_mapper();
		screen.display(win);
		win.display();
	}

	delete pad;
	return 0;
}



//       NV-B DIZC
//		 84 1 8421