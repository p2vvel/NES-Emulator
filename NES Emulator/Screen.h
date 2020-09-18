#pragma once

#include<SFML/Graphics.hpp>

#define PIXELS_TOTAL (width * height * pixel_size * pixel_size)

class Screen
{
private:
	sf::VertexArray pixels;
	unsigned short width;
	unsigned short height;
	unsigned char pixel_size;
public:
	Screen(const unsigned short &_width_ = 256, const unsigned short &_height_ = 240, const unsigned short& _pixel_size_ = 2);
	~Screen() {};
	void display(sf::RenderWindow & win);
	const sf::Color &getPixelColor(const unsigned int &x, const unsigned int &y);
	void setPixelColor(const unsigned int &x, const unsigned int &y, const sf::Color &color);
	void setPixelSize(const unsigned int &pixel_size);	//Ustawia odpowiednio na ekranie poszczególne piksele w uwzglêdniaj¹c przy tym ich wielkoœæ
	void resetScreen();
};