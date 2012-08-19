#include "Command.h"


void PlayerCommand_t::Execute()
{
	if(!stamped)
	{
		timestamp=LocalTime;
		stamped=true;
	}
	receiver->exists=creating;
	if(!creating)
	{
		receiver->Hide();
	}
};
void PlayerCommand_t::Unexecute()
{
	receiver->exists=!creating;
	if(creating){receiver->Hide();}
};

KeyCommand_t::KeyCommand_t(Player_t* param1,Stuff::KEYS param2):receiver(param1),key(param2)
{
	using namespace Stuff;
	stamped=false;
	switch(key)
	{
	case KB_LEFT: c= new Coord(1,0);break;
	case KB_RIGHT: c= new Coord(-1,0);break;
	case KB_UP: c= new Coord(0,-1);break;
	case KB_DOWN: c= new Coord(0,1);break;
	default: c=new Coord(0,0);
	};


};
void KeyCommand_t::Execute()
{
	receiver->SetPos(receiver->GetPos()+*c);
	if(!stamped)
	{
		timestamp=LocalTime; 
		stamped=true;

	}
};

void KeyCommand_t::Unexecute()
{
	receiver->SetPos(receiver->GetPos()-*c);
};	


EmptyCommand_t::EmptyCommand_t()
{
	stamped=false;
};
void EmptyCommand_t::Execute()
{
	if(!stamped)
	{
		timestamp=LocalTime;
		stamped=true;
	}
};
void EmptyCommand_t::Unexecute(){};