#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "stdafx.h"
#include "Player.h"

class Command_t
{
protected:
	bool stamped;
public:
	unsigned int timestamp;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
};

class PlayerCommand_t : public Command_t
{
public:
	PlayerCommand_t(Player_t* param1, bool param2):receiver(param1),creating(param2){stamped=false;}
	void Execute();
	void Unexecute();
private:	
	Player_t *receiver;
	bool creating;
};



class KeyCommand_t : public Command_t
{
public:
	KeyCommand_t(Player_t* param1,Stuff::KEYS param2);
	void Execute();
	void Unexecute();

private:
	Player_t *receiver;
	Stuff::KEYS key;
	Stuff::Coord * c;
};

class EmptyCommand_t : public Command_t
{
public:
	EmptyCommand_t();
	void Execute();
	void Unexecute();
};







#endif