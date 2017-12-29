#include "x86_function.h"
#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>

using ns = std::chrono::nanoseconds;
using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;

int width = 1000;
int height = 600;

struct verticle
{
	int x, y;
	char r,g,b,a;
};

struct verticle_float
{
	float x, y;
	float r,g,b,a;
};

void drawGrid( sf::Uint8* pixels, int space = 15  )
{
	for( int j = 0; j < width*height*4; j += 4*width )
	for( int i = 0; i < width*4; i += 4*space )
	{
		pixels[i+j+0] = 0;
		pixels[i+j+1] = 0;
		pixels[i+j+2] = 0;	
	}
	for( int j = 0; j < width*height*4; j += 4*space*width + 4*width )
	for( int i = 0; i < width*4; i += 4 )
	{
		pixels[j+i+0] = 0;
		pixels[j+i+1] = 0;
		pixels[j+i+2] = 0;
	}
}
void swap( verticle& v1, verticle& v2 )
{
	verticle tmp = v1;
	v1 = v2;
	v2 = tmp;
}

void drawTriangle( sf::Uint8* pixels, verticle* verticles )
{
// BEZ KOLORÓW !!!!!

	// 1. Sortowanie wierzchołków po y (v1.y <= v2.y <= v3.y)
	
	verticle v1 = verticles[0];
	verticle v2 = verticles[1];
	verticle v3 = verticles[2];
	
	if( v2.y < v1.y ) swap( v1, v2 );
	if( v3.y < v1.y ) swap( v1, v3 );
	if( v3.y < v2.y ) swap( v2, v3 );
	//std::cout << v1.y << ' ' << v2.y << ' ' << v3.y << '\n';
	//std::cout << v1.x << ' ' << v2.x << ' ' << v3.x << '\n';
	// Obliczanie różnic
	verticle_float d13, d12, d23;
	if( v3.y == v1.y ) return;
	d13.x = float(v3.x - v1.x)/(v3.y - v1.y);
	
	//std::cout << d13.x << ' ' << d12.x << ' ' << d23.x << '\n';
	// Początek i koniec odcinka
	verticle_float Vb, Ve;
	Vb.x = v1.x;
	Vb.y = v1.y;
	Ve = Vb;
		
	// 2. Rysowanie poziomych linii między wierzchołkami v1 i v2
	// 	a) d13 i d12 - zmiany współrzędnej x i kolorów w stosunku do zmiany y
	//		b) Vb i Ve - VectorBegin i VectorEnd, czyli (x,y,r,g,b,a) początku i końca rysowanej linii
	//			wyznaczane za pomocą algorytmu Bresenham'a
	//		c) Rysowanie poziomej linii przy pomocy interpolacji liniowej
	//		d) skok do 2b jeśli nie doszliśmy do v2
	if( v2.y != v1.y )
	{
		d12.x = float(v2.x - v1.x)/(v2.y - v1.y);
		//std::cout << v2.y << " " <<  v1.y << " " << d12.x << "\n";
		while( Vb.y <= v2.y )
		{
			//std::cout << Vb.y << ' ' << Vb.x << ' ' << Ve.x <<'\n';
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
	// 3. Rysowanie poziomych linii między wierzchołkami v2 i v3
	//		a) d13 i d23 - zmiany współrzędnej x i kolorów w stosunku do zmiany y
	//		b) Vb i Ve - VectorBegin i VectorEnd, czyli (x,y,r,g,b,a) początku i końca rysowanej linii
	//			wyznaczane za pomocą algorytmu Bresenham'a
	//		c) Rysowanie poziomej linii przy pomocy interpolacji liniowej
	//		d) skok do 3b jeśli nie doszliśmy do v3
	//
	
	
	if( v3.y != v2.y )
	{
		d23.x = float(v3.x - v2.x)/(v3.y - v2.y);
		Ve.x = v2.x;
		while( Vb.y <= v3.y )
		{
			//std::cout << Vb.y << ' ' << Vb.x << ' ' << Ve.x <<'\n';
			if( Vb.x < Ve.x )
			{
				for( int i = std::round(Vb.x); i <= std::round(Ve.x); ++i )
				{
					//std::cout << Vb.x << ' ' << Ve.x << ' ' << Vb.y << ' ' << i << '\n';
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
	// 4. Koniec
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
		delete image;
		image = new sf::Image;
		image->create( width, height, sf::Color::White );
		
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				verticles[0].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				verticles[0].y = sf::Mouse::getPosition(window).y;
		}
		else if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				verticles[1].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				verticles[1].y = sf::Mouse::getPosition(window).y;
		}
		else if( sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) )
		{
			if( 0 <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x < width )
				verticles[2].x = sf::Mouse::getPosition(window).x;
			if( 0 <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y < height )
				verticles[2].y = sf::Mouse::getPosition(window).y;
		}
		
		start = get_time::now();

		drawGrid( (sf::Uint8*)image->getPixelsPtr() );
		
		std::cout << "\033[2J\033[1;1HdrawGrid:         " << std::setw(20) << std::right << (std::chrono::duration_cast<ns>(get_time::now() - start)).count() << " ns\n";
		
		start = get_time::now();

		drawTriangle( (sf::Uint8*)image->getPixelsPtr(), verticles );
		
		std::cout << "drawTriangleCpp:  " << std::setw(20) << std::right << (std::chrono::duration_cast<ns>(get_time::now() - start)).count() << " ns\n";

		start = get_time::now();
		
		x86_function( (sf::Uint8*)image->getPixelsPtr(), width, height, verticles );

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