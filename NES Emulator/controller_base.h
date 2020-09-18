#pragma once
#include<SFML/Graphics.hpp>


class controller_base
{
	unsigned char controller_latch;
	unsigned char controller_cycle;
	bool is_polling;
public:
	controller_base();
	~controller_base();
	void control_polling(const bool& command);
	void update_controller_latch();
	const bool read_controller_latch();
	const bool& get_is_polling() const { return is_polling; }
};