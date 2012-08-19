#include "Player.h"

extern HANDLE hStdout;

void ::Player_t::Draw(int ch)
{	
	Stuff::gotoxy(coord);
	std::cout<<(char)ch;
}


void ::Player_t::Draw()
{
	if(!movable){SetConsoleTextAttribute(hStdout,0x04);}
	Draw(s);
	if(!movable){SetConsoleTextAttribute(hStdout,0x0F);}
}
void ::Player_t::Hide()
{
	Draw((int)' ');
}
Stuff::Coord Player_t::GetPos()
{
	return coord;
}
void ::Player_t::SetPos(Stuff::Coord param1)
{
	Draw((int)' ');
	coord=param1;
}