#pragma once
#include <iostream>
#include <vector>
#include <Windows.h> 
#include <conio.h> // для _getch
#include <string>
#include <random>
#include <ctime>
#include <fstream>
#include <map>
#include <exception>
#include <iomanip>

// Перечисление с константами цветов
enum class Colour {
	BLACK,
	BLUE,
	GREEN,
	BR_BLUE,
	RED,
	VIOLET,
	YELLOW,
	GREY,
	DR_GREY,
	BLUE2,
	BR_GREEN,
	CYAN,
	PINK,
	VIOLET2,
	BEIGE,
	WHITE,
	MAX_COLOURS
};

// Перечисление всех объектов в игре
enum class Object {
	PLAYER,
	FRIEND,
	EDGE,
	WALL,
	FLOOR,
	DOOR,
	MAX_OBJ,
};
/*
enum class Status {
	WITHOUT,
	TALK,
	SELECTED,
};
*/

class Subject;
class Field;
class Pixel;
struct Point;

using colour_t = int;

colour_t colour(Colour back, Colour text);
void PrintColour(std::string str, colour_t col);
