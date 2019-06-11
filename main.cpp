/* ============================================================================
   [PROJECT]

   Name: Triangle Shading
   Desc: Drawing a triangle based on Gouraud shading
   Arch: x86_64
   Lang: C++, ASM (Intel syntax)
   Tech: AVX, AVX2
   Libs: SFML 2.4

   [FILE]
   
   Name: main.cpp
   Desc: Creating window, allowing interaction with the user, drawing grid,
         measuring execution time of functions written in C++ and ASM,
         defining "vertex" structure

   Auth: Gerard Wypych
==============================================================================*/

#include "x86_function.h"
#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <thread>

using ns = std::chrono::nanoseconds;
using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;

int width = 1000;
int height = 600;
const int delta_ratio = 10.0;

struct vertex
{
	float x,y,z,w; // Coordinates
	float r,g,b,a; // Colors
};

void drawGrid( sf::Uint8* pixels, int space = 15  )
{
	for( int j = 0; j < width*height*4; j += 4*width )
	for( int i = 0; i < width*4; i += 4*space )
	{
		pixels[i+j+0] = 230;
		pixels[i+j+1] = 230;
		pixels[i+j+2] = 230;	
	}
	for( int j = 0; j < width*height*4; j += 4*space*width + 4*width )
	for( int i = 0; i < width*4; i += 4 )
	{
		pixels[j+i+0] = 230;
		pixels[j+i+1] = 230;
		pixels[j+i+2] = 230;
	}
}
void swap( vertex& v1, vertex& v2 )
{
	vertex tmp = v1;
	v1 = v2;
	v2 = tmp;
}

void drawTriangle( sf::Uint8* pixels, vertex* vertices )
{
	// 1. Sorting vertices by y (v1.y <= v2.y <= v3.y)
	
	vertex v1 = vertices[0];
	vertex v2 = vertices[1];
	vertex v3 = vertices[2];
	
	if( v2.y < v1.y ) swap( v1, v2 );
	if( v3.y < v1.y ) swap( v1, v3 );
	if( v3.y < v2.y ) swap( v2, v3 );
	
	// Calculating differences
	vertex d13, d12, d23;
	if( v3.y == v1.y ) return;
	d13.x = float(v3.x - v1.x)/(v3.y - v1.y);
	
	// Beginning and end of the line
	vertex Vb, Ve;
	Vb.x = v1.x;
	Vb.y = v1.y;
	Ve = Vb;
		
	// 2. Drawing lines between v1 and v2
	if( v2.y != v1.y )
	{
		d12.x = float(v2.x - v1.x)/(v2.y - v1.y);
		while( Vb.y <= v2.y )
		{
			if( Vb.x < Ve.x )
			{
				for( int i = std::round(Vb.x); i <= std::round(Ve.x); ++i )
				{
					pixels[4*i + int(Vb.y)*width*4 + 0] = 255;
					pixels[4*i + int(Vb.y)*width*4 + 1] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 2] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 3] = 255;
				}
			}
			else
			{
				for( int i = std::round(Ve.x); i <= std::round(Vb.x); ++i )
				{
					pixels[4*i + int(Vb.y)*width*4 + 0] = 255;
					pixels[4*i + int(Vb.y)*width*4 + 1] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 2] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 3] = 255;
				}
			}
			
			
			Vb.x += d13.x;
			Vb.y += 1;
			
			Ve.x += d12.x;
			Ve.y += 1;
		}
		Ve.x -= d12.x;
	}
	
	// 3. Drawing lines between v2 and v3
	if( v3.y != v2.y )
	{
		d23.x = float(v3.x - v2.x)/(v3.y - v2.y);
		Ve.x = v2.x;
		while( Vb.y <= v3.y )
		{
			if( Vb.x < Ve.x )
			{
				for( int i = std::round(Vb.x); i <= std::round(Ve.x); ++i )
				{
					pixels[4*i + int(Vb.y)*width*4 + 0] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 1] = 255;
					pixels[4*i + int(Vb.y)*width*4 + 2] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 3] = 255;
				}
			}
			else
			{
				for( int i = std::round(Ve.x); i <= std::round(Vb.x); ++i )
				{
					pixels[4*i + int(Vb.y)*width*4 + 0] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 1] = 255;
					pixels[4*i + int(Vb.y)*width*4 + 2] = 0;
					pixels[4*i + int(Vb.y)*width*4 + 3] = 255;
				}
			}
			
			
			Vb.x += d13.x;
			Vb.y += 1;
			
			Ve.x += d23.x;
			Ve.y += 1;
		}	
	}
}

int main()
{
	sf::RenderWindow window( sf::VideoMode(width, height), 
		"Triangle shading | Gerard Wypych" );
	
	// Mask, it says where are the colors in vertex, first bytes
	char mask[16] = {0,4,8,12,0,0,0,0,0,0,0,0,0,0,0,0};

	vertex vertices[3];

	vertices[0].x = 50;
	vertices[0].y = 50;
	vertices[0].z = 1;
	vertices[0].w = 1;
	vertices[0].r = 255;
	vertices[0].g = 0;
	vertices[0].b = 0;
	vertices[0].a = 255;

	vertices[1].x = 950;
	vertices[1].y = 200;
	vertices[1].z = 1;
	vertices[1].w = 1;
	vertices[1].r = 0;
	vertices[1].g = 255;
	vertices[1].b = 0;
	vertices[1].a = 255;

	vertices[2].x = 450;
	vertices[2].y = 550;
	vertices[2].z = 1;
	vertices[2].w = 1;
	vertices[2].r = 0;
	vertices[2].g = 0;
	vertices[2].b = 255;
	vertices[2].a = 255;

	sf::Image* image = new sf::Image;
	image->create( width, height, sf::Color::White );
	
	sf::Texture texture;
	if( !texture.loadFromImage( *image ) )
		return -1;

	sf::Sprite* sprite = new sf::Sprite;
	sprite->setTexture(texture);
	
	auto start = get_time::now();
	
	auto time_asm = (std::chrono::duration_cast<ns>(get_time::now()-start)).count();
	
	auto all_time_asm = time_asm;
	auto display_time_asm = all_time_asm;
	
	const int number_of_samples = 500;
	int current_numeber_of_samples = 1;	
	
	auto time_cpp = time_asm;
	auto time_grid = time_cpp;
	
	while( window.isOpen() )
	{		
		sf::Event event;
		while( window.pollEvent(event) )
		{
			if( event.type == sf::Event::Closed )
				window.close();
				
			// Resizing the window
			if( event.type == sf::Event::Resized )
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
			
			// Changing colors by scrolling the mouse wheel
			if( event.type == sf::Event::MouseWheelScrolled )
			{
				// Vertex number 1
				if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) )
				{
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
					{
						vertices[0].r += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[0].r > 255.0 )
							vertices[0].r = 255.0;
						else if( vertices[0].r < 0.0 )
							vertices[0].r = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::G) )
					{
						vertices[0].g += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[0].g > 255.0 )
							vertices[0].g = 255.0;
						else if( vertices[0].g < 0.0 )
							vertices[0].g = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::B) )
					{
						vertices[0].b += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[0].b > 255.0 )
							vertices[0].b = 255.0;
						else if( vertices[0].b < 0.0 )
							vertices[0].b = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
					{
						vertices[0].a += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[0].a > 255.0 )
							vertices[0].a = 255.0;
						else if( vertices[0].a < 0.0 )
							vertices[0].a = 0.0;
					}
				}
				
				// Vertex number 2
				if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) )
				{
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
					{
						vertices[1].r += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[1].r > 255.0 )
							vertices[1].r = 255.0;
						else if( vertices[1].r < 0.0 )
							vertices[1].r = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::G) )
					{
						vertices[1].g += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[1].g > 255.0 )
							vertices[1].g = 255.0;
						else if( vertices[1].g < 0.0 )
							vertices[1].g = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::B) )
					{
						vertices[1].b += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[1].b > 255.0 )
							vertices[1].b = 255.0;
						else if( vertices[1].b < 0.0 )
							vertices[1].b = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
					{
						vertices[1].a += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[1].a > 255.0 )
							vertices[1].a = 255.0;
						else if( vertices[1].a < 0.0 )
							vertices[1].a = 0.0;
					}
				}
				
				// Vertex number 3
				if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) )
				{
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
					{
						vertices[2].r += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[2].r > 255.0 )
							vertices[2].r = 255.0;
						else if( vertices[2].r < 0.0 )
							vertices[2].r = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::G) )
					{
						vertices[2].g += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[2].g > 255.0 )
							vertices[2].g = 255.0;
						else if( vertices[2].g < 0.0 )
							vertices[2].g = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::B) )
					{
						vertices[2].b += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[2].b > 255.0 )
							vertices[2].b = 255.0;
						else if( vertices[2].b < 0.0 )
							vertices[2].b = 0.0;
					}
					if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
					{
						vertices[2].a += event.mouseWheelScroll.delta*delta_ratio;
						if( vertices[2].a > 255.0 )
							vertices[2].a = 255.0;
						else if( vertices[2].a < 0.0 )
							vertices[2].a = 0.0;
					}
				}
				
			}
		}
		
		// Clean the image, not so pretty, but it works
		delete image;
		image = new sf::Image;
		image->create( width, height, sf::Color::White );
		
		// Moving the vertices
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				vertices[0].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				vertices[0].y = sf::Mouse::getPosition(window).y;				
		}
		else if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				vertices[1].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				vertices[1].y = sf::Mouse::getPosition(window).y;
		}
		else if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				vertices[2].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				vertices[2].y = sf::Mouse::getPosition(window).y;
		}
		
		// Drawing
		
		//------------------------------------------------------------------------	
		start = get_time::now();

		drawGrid( (sf::Uint8*)image->getPixelsPtr() );
		
		time_grid = (std::chrono::duration_cast<ns>(get_time::now() - start)).count();
		//------------------------------------------------------------------------
		start = get_time::now();
		
		x86_function( vertices, (sf::Uint8*)image->getPixelsPtr(), width, height, mask );
		
		time_asm = (std::chrono::duration_cast<ns>(get_time::now() - start)).count();
		
		if( current_numeber_of_samples < number_of_samples )
		{
			++current_numeber_of_samples;
			all_time_asm += time_asm;
		}
		else
		{
			display_time_asm = all_time_asm/number_of_samples;
			current_numeber_of_samples = 0;
			all_time_asm = 0; 
		}
		

		//------------------------------------------------------------------------
		start = get_time::now();
		
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) )
			drawTriangle( (sf::Uint8*)image->getPixelsPtr(), vertices );
			
		time_cpp = (std::chrono::duration_cast<ns>(get_time::now() - start)).count();
		//------------------------------------------------------------------------

		// Time of drawing
		std::cout << "\033[2J\033[1;1HdrawGrid:         " << std::setw(20) << std::right << time_grid << " ns\n";
		std::cout << "drawTriangle_asm: " << std::setw(20) << std::right << display_time_asm << " ns\n";
		std::cout << "drawTriangle_cpp: " << std::setw(20) << std::right << time_cpp << " ns\n";
		
		texture.update(*image);
		
		window.clear();
		window.draw(*sprite);		
		window.display();		
	}
	
	delete image;
	delete sprite;
	
	return 0;
}
