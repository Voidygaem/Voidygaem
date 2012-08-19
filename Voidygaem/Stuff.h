
#ifndef __STUFF_H__
#define __STUFF_H__

#include "stdafx.h"

namespace Stuff
{
	enum KEYS {KB_NONE,KB_LEFT, KB_RIGHT, KB_UP, KB_DOWN, KB_SPACE, KB_ESC, KB_PLUS, KB_MINUS};
	int modulo(int a, int b);
	void gotoxy(COORD c);
	KEYS GetKey();

	struct Coord : public COORD
	{
		Coord(short par1,short par2){X=par1;Y=par2;};
		Coord operator+(COORD c);
		Coord operator-(COORD c);
		bool operator==(COORD c);
	};

}
#endif