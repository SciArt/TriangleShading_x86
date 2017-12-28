#include "x86_function.h"
#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>

using ns = std::chrono::nanoseconds;
using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;

unsigned int width = 1000;
unsigned int height = 400;

struct verticle
{
	unsigned int x, y;
	char r,g,b,a;
};

void drawGrid( sf::Uint8* pixels, unsigned int space = 15  )
{
	for( unsigned int j = 0; j < width*height*4; j += 4*width )
	for( unsigned int i = 0; i < width*4; i += 4*space )
	{
		pixels[i+j+0] = 0;
		pixels[i+j+1] = 0;
		pixels[i+j+2] = 0;	
	}
	for( unsigned int j = 0; j < width*height*4; j += 4*space*width + 4*width )
	for( unsigned int i = 0; i < width*4; i += 4 )
	{
		pixels[j+i+0] = 0;
		pixels[j+i+1] = 0;
		pixels[j+i+2] = 0;
	}
}

void drawTriangle( sf::Uint8* pixels, verticle* verticles )
{

}

int main()
{
	sf::RenderWindow window( sf::VideoMode(width, height), "Triangle shading | Gerard Wypych" );

	verticle verticles[3];

	verticles[0].x = 10;
	verticles[0].y = 10;
	verticles[0].r = 255;
	verticles[0].g = 0;
	verticles[0].b = 0;
	verticles[0].a = 0;

	verticles[1].x = 150;
	verticles[1].y = 300;
	verticles[1].r = 0;
	verticles[1].g = 255;
	verticles[1].b = 0;
	verticles[1].a = 0;

	verticles[2].x = 300;
	verticles[2].y = 50;
	verticles[2].r = 0;
	verticles[2].g = 0;
	verticles[2].b = 255;
	verticles[2].a = 0;
	
	//sf::Uint8* pixels = new sf::Uint8[width * height * 4];

	sf::Image* image = new sf::Image;
	image->create( width, height, sf::Color::White );
	
	sf::Texture texture;
	if( !texture.loadFromImage( *image ) )
		return -1;

	sf::Sprite* sprite = new sf::Sprite;
	sprite->setTexture(texture);
	
	auto start = get_time::now();
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				width = window.getSize().x;
				height = window.getSize().y;
				sf::View view(sf::FloatRect(0, 0, width, height));
				window.setView(view);
				delete image;
				image = new sf::Image;
				image->create( width, height, sf::Color::White );
				if( !texture.loadFromImage( *image ) )
					return -1;
				delete sprite;
				sprite = new sf::Sprite;
				sprite->setTexture(texture);
			}
		}	
		
		start = get_time::now();

		drawGrid( (sf::Uint8*)image->getPixelsPtr() );
		
		std::cout << "\033[2J\033[1;1HdrawGrid:         " << std::setw(20) << std::right << (std::chrono::duration_cast<ns>(get_time::now() - start)).count() << " ns\n";
		
		start = get_time::now();

		drawTriangle( (sf::Uint8*)image->getPixelsPtr(), verticles );
		
		std::cout << "drawTriangleCpp:  " << std::setw(20) << std::right << (std::chrono::duration_cast<ns>(get_time::now() - start)).count() << " ns\n";

		start = get_time::now();
		
		x86_function( (sf::Uint8*)image->getPixelsPtr(), verticles );

		std::cout << "drawTriangle_x86: " << std::setw(20) << std::right << (std::chrono::duration_cast<ns>(get_time::now() - start)).count() << " ns\n";

		texture.update(*image);
		
		window.clear();
		window.draw(*sprite);		
		window.display();		
	}
	
	delete image;
	delete sprite;

	//delete[] pixels;
	
	return 0;
}
