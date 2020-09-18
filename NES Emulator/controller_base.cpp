#include "controller_base.h"



controller_base::controller_base()
{
	is_polling = false;
	controller_cycle = 0;
	controller_latch = 0;
}

controller_base::~controller_base()
{
}

void controller_base::control_polling(const bool& command)
{
	is_polling = command;
	if (command)
	{
		controller_latch = 0;
		controller_cycle = 0;
	}
}

void controller_base::update_controller_latch()
{
	//0 - A			//1 - B
	//2 - Select	//3 - Start
	//4 - Up		//5 - Down
	//6 - Left		//7 - Right

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))		//A
		controller_latch |= 0x01;
	else
		controller_latch &= ~0x01;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))		//B
		controller_latch |= 0x02;
	else
		controller_latch &= ~0x02;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))		//Select
		controller_latch |= 0x04;
	else
		controller_latch &= ~0x04;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))		//Start
		controller_latch |= 0x08;
	else
		controller_latch &= ~0x08;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))		//Up
		controller_latch |= 0x10;//(controller_latch & ~0x30) | 0x10;
	else
		controller_latch &= ~0x10;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))		//Down
		controller_latch |= 0x20;//= (controller_latch & ~0x30) | 0x20;
	else
		controller_latch &= ~0x20;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))		//Left
		controller_latch |= 0x40;//=(controller_latch & ~0xc0) | 0x40;
	else
		controller_latch &= ~0x40;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))	//Right
		controller_latch |= 0x80; //(controller_latch & ~0xc0) | 0x80;
	else
		controller_latch &= ~0x80;

}

const bool controller_base::read_controller_latch()
{
	bool result = true;
	if (controller_cycle < 8)
	{
		result = controller_latch & (0x01 << controller_cycle);
		controller_cycle++;
	}
	return result;
}
