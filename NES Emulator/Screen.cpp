#include "Screen.h"

#define PIXEL_ADDRESS (x * 4) + (y * width * 4)

Screen::Screen(const unsigned short& _width_, const unsigned short& _height_, const unsigned short& _pixel_size_)
{
	if (_pixel_size_ >= 2)
		pixel_size = _pixel_size_;
	else
		pixel_size = 2;
	width = _width_;
	height = _height_;
	pixels.resize(PIXELS_TOTAL);
	pixels.setPrimitiveType(sf::Quads);
	setPixelSize(pixel_size);
}


void Screen::display(sf::RenderWindow &win)
{
	win.draw(pixels);
}

const sf::Color &Screen::getPixelColor(const unsigned int & x, const unsigned int & y)
{
	return pixels[PIXEL_ADDRESS].color;
}

void Screen::setPixelColor(const unsigned int & x, const unsigned int & y, const sf::Color &new_color)
{
	for (int i = 0; i < 4; i++)
		pixels[PIXEL_ADDRESS + i].color = new_color;
}

void Screen::setPixelSize(const unsigned int & pixel_size)
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{	  
			/*(x*4==x<<2)(x*1024==x<<10)*/
			pixels[PIXEL_ADDRESS/*+0*/].position = sf::Vector2f(x * pixel_size, y * pixel_size);
			pixels[PIXEL_ADDRESS + 1].position = sf::Vector2f(x * pixel_size + pixel_size, y * pixel_size);
			pixels[PIXEL_ADDRESS + 2].position = sf::Vector2f(x * pixel_size + pixel_size, y * pixel_size + pixel_size);
			pixels[PIXEL_ADDRESS + 3].position = sf::Vector2f(x * pixel_size, y * pixel_size + pixel_size);
		}
}

void Screen::resetScreen()
{
	for (int i = 0; i < PIXELS_TOTAL; i++)
		pixels[i].color = sf::Color(sf::Color::Black);
}