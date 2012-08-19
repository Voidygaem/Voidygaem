#include "stdafx.h"
#include "Stuff.h"



int Stuff::modulo(int a, int b)
{
	return a>=0?a%b:a%b+b;
};

void Stuff::gotoxy(COORD c)
{
	SetConsoleCursorPosition(hStdout,c);
};

Stuff::KEYS Stuff::GetKey()
{
	if(_kbhit())
	{
		int ch=_getch();
		if (ch==0xE0)
		{
			ch=_getch();
			switch (ch)
			{
			case 77:
				return KB_LEFT;
			case 75:
				return KB_RIGHT;
			case 72:
				return KB_UP;
			case 80:
				return KB_DOWN;
			}
		};
		switch(ch)
		{
		case 45:
			return KB_MINUS;
		case 43:
			return KB_PLUS;
		case 32:
			return KB_SPACE;
		case 27:
			return KB_ESC;
		}
	}
	return KB_NONE;
};


Stuff::Coord Stuff::Coord::operator+(COORD c)
{
	X=modulo(X+(c.X),(WIDTH));
	Y=modulo(Y+(c.Y),(HEIGHT));
	return *this;
};
Stuff::Coord Stuff::Coord::operator-(COORD c)
{
	X=modulo(X-(c.X),(WIDTH));
	Y=modulo(Y-(c.Y),(HEIGHT));
	return *this;
};
bool Stuff::Coord::operator==(COORD c)
{
	return ((X==c.X)&&(Y==c.Y));
};