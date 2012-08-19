#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "stdafx.h"

class Player_t
{

public:
	bool exists;
	bool movable;
	bool visible;
	Player_t(int param1, Stuff::Coord param2):s(param1),coord(param2),exists(true),movable(true),visible(true){};
	void Draw();
	void Hide();
	Stuff::Coord GetPos();
	void SetPos(Stuff::Coord param1);

private:
	Stuff::Coord coord;
	int s;
	void Draw(int ch);

};



#endif