#include <iostream>

#include <ctime>
#include <list>

#include "Command.h"
#include "Player.h"
#include "stdafx.h"

const int WIDTH=40;
const int HEIGHT=20;

HANDLE hStdout;
int LocalTime = 0;

const COORD TIME={45,3};
const COORD COORDS={45,5};
char mapchars[40*20],mappassability[40*20];


enum {NORMAL, PAUSED, BACKWARD, FORWARD} state;

bool split=false;









Player_t* pl;
std::list<Player_t *> Players;
void timeprint(unsigned int st)
{

	Stuff::gotoxy(TIME);
	std::cout<<"Local time: "<<(st/100)<<"     ";
	Stuff::gotoxy(COORDS);
	std::cout<<"X: "<<pl->GetPos().X<<" Y:"<<pl->GetPos().Y<<"    ";
}

std::list<Command_t *> *Active;
std::list<Command_t *>::iterator ActIt;



void StateKeyHandler(Stuff::KEYS k,unsigned int *last)
{
	using namespace Stuff;
	switch(k)
	{
	case KB_SPACE: state=(state!=PAUSED)?PAUSED:NORMAL;break;
	case KB_MINUS: state=(state==PAUSED)?BACKWARD:state;break;
	default: return;
	}
	static bool wasbackward;
	if(state==PAUSED){pl->movable=false;}
	if(state==BACKWARD)
	{
		if(!wasbackward){
			wasbackward=true;
			Active->insert(ActIt, new PlayerCommand_t(pl,false));
			(*(--ActIt))->Execute();
			//	hide=true;
			Coord tmpcoord=pl->GetPos();
			pl->movable=true;
			pl->visible=false;
			pl=new Player_t((int)'@',tmpcoord);
			pl->movable=false;
			Players.push_back(pl);
			pl->exists=false;
			pl->visible=true;}
		//Active->insert(ActIt, new PlayerCommand(pl,true));
		//swapcl();
	}
	if(state==NORMAL)
	{
		pl->movable=true;
		if(wasbackward){Active->insert(ActIt, new PlayerCommand_t(pl,true)); wasbackward=false;split=true;}
	}
	*last=clock();
}





void DrawPlayers()
{
	std::list<Player_t*>::iterator pls = Players.begin();
	for(;pls!=Players.end();++pls)
	{
		if((*pls)->exists || (*pls)->visible ){(*pls)->Draw();}
	}

}

void nop(){}




bool pass(Stuff::KEYS key)
{
	using namespace Stuff;
	switch(key)
	{
	case KB_LEFT: if(::mappassability[pl->GetPos().Y*40+pl->GetPos().X+1]==1){return false;};break;
	case KB_RIGHT:if(::mappassability[pl->GetPos().Y*40+pl->GetPos().X-1]==1){return false;}break;
	case KB_UP: if(::mappassability[(pl->GetPos().Y -1)*40+pl->GetPos().X]==1){return false;};break;
	case KB_DOWN: if(::mappassability[(pl->GetPos().Y +1)*40+pl->GetPos().X]==1){return false;}break;
	};
	return true;
}


struct Button
{
	COORD c;
	Button(){c=Stuff::Coord(0,0);}
	void Draw()
	{
		Stuff::gotoxy(c);
		std::cout<<'O';
	}
	bool getpressed()
	{

		for(std::list<Player_t*>::iterator pls = Players.begin();pls!=Players.end();++pls)
		{
			if((*pls)->exists && (*pls)->GetPos()==c){return true;}
		}
		return false;
	}

};
struct Door
{
	COORD* d;
	short dcnt;
	short *b;
	short bcnt;
	bool opened;
	Door(){opened=true;bcnt=0;dcnt=0;}
	void Draw()
	{
		for(short i=0; i<dcnt;++i)
		{
			Stuff::gotoxy(d[i]);

			std::cout<<(opened?' ':char(0xb3));
		}
	}
	void setopen(bool param)
	{
		opened=param;
		for(short i=0; i<dcnt;++i)
		{
			::mappassability[d[i].Y*40+d[i].X]=(param?0:1);
		}
	}
};

Button *buttons;
Door *doors;
short b_cnt,d_cnt;
Stuff::Coord startc(1,1),endc(1,1);
void mapopen(FILE *mf)
{
	short t1,t2;
	fread(mapchars,sizeof(char),800,mf);
	fread(&t1,sizeof(short),1,mf);
	b_cnt=t1;
	buttons= new Button[t1]();
	for(int i=0;i<t1;++i)
	{
		fread(&(buttons[i].c.X),sizeof(short),1,mf);
		fread(&(buttons[i].c.Y),sizeof(short),1,mf);
	}
	fread(&t1,sizeof(short),1,mf);
	doors= new Door[t1]();
	d_cnt=t1;
	for(int i=0;i<t1;++i)
	{
		fread(&t2,sizeof(short),1,mf);
		doors[i].bcnt=t2;
		doors[i].b=new short[t2];
		for(int j=0;j<t2;++j)
		{
			fread(&(doors[i].b[j]),sizeof(short),1,mf);
		}
		fread(&t2,sizeof(short),1,mf);
		doors[i].dcnt=t2;
		doors[i].d=new COORD[t2];
		for(int j=0;j<t2;++j)
		{
			fread(&(doors[i].d[j].X),sizeof(short),1,mf);
			fread(&(doors[i].d[j].Y),sizeof(short),1,mf);
		}
	}
	fread(&(startc.X),sizeof(short),1,mf);
	fread(&(startc.Y),sizeof(short),1,mf);
	fread(&(endc.X),sizeof(short),1,mf);
	fread(&(endc.Y),sizeof(short),1,mf);
}

void mapprint()
{
	unsigned char ch;
	for(int i=0;i<20;++i)
	{
		for(int j=0;j<40;++j)
		{	
			Stuff::gotoxy(Stuff::Coord(j,i));
			ch = mapchars[i*40+j];
			std::cout<<ch;
			if(ch>=unsigned char(0xb3) && ch<=unsigned char(0xda))
			{
				mappassability[i*40+j]=1;
			}
		}
	}
	gotoxy(endc);
	std::cout<<'X';

}

void DrawD_B()
{
	for(short i=0;i<b_cnt;++i)
	{
		buttons[i].Draw();
	}
	for(short i=0;i<d_cnt;++i)
	{
		doors[i].Draw();
	}
}
void UpdateDoors()
{
	for(short i=0;i<d_cnt;++i)
	{
		bool opened=true;
		short t1=doors[i].bcnt;
		for(short j=0;j<t1;++j)
		{
			if(!(buttons[doors[i].b[j]].getpressed())){opened=false;break;}
		}
		if(doors[i].opened!=opened){doors[i].setopen(opened);}
	}
}

bool Win()
{
	if((pl->GetPos())==endc)
	{
		Stuff::gotoxy(Stuff::Coord(24,10));
		std::cout<<"You win! Press any key to exit.";
		_getch();
		return true;
	}
	return false;
}

int main()
{
	using namespace Stuff;
	using namespace std;
	FILE * map;
	fopen_s (&map ,"lol.map","rb");
	mapopen(map);
	//map.read(mapchars,sizeof(mapchars));
	Active= new list<Command_t *>();
	ActIt=Active->begin();
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,0x0F);

	for(int i=0;i<2500;++i){cout<<' ';};
	KEYS key;
	Command_t *tmp;
	for(int j=0;j<800;++j){::mappassability[j]=0;}


	pl= new Player_t((int)'@',startc);
	pl->exists=true;
	pl->movable=true;
	Players.push_back(pl);
	tmp=new EmptyCommand_t();
	Active->insert(ActIt,tmp);
	tmp->Execute();
	--ActIt;
	state=NORMAL;
	unsigned int last=clock();
	unsigned int now;
	mapprint();

	do
	{
		UpdateDoors();
		DrawD_B();
		DrawPlayers();
		//pl->Draw();
		//timeprint(clock()-start);
		Sleep(1000/60);
		key=GetKey();
		StateKeyHandler(key,&last);


		if(key==KB_PLUS)
		{
			nop();
		}

		if(state==::NORMAL)
		{
			now=clock();
			LocalTime=LocalTime+now-last;
			last=now;
			if(split){
				split=false; 
				(*(--ActIt))->Execute();}
			while(ActIt!=Active->begin() && (*(--ActIt))->timestamp<=LocalTime )
			{

				(*ActIt)->Execute();
				//--ActIt;
				//--BackIt;

			}
			++ActIt;
			if(key>=KB_LEFT && key<=KB_DOWN)
			{
				if(pass(key)){
					//++ActIt;
					tmp=new KeyCommand_t(pl,key);
					tmp->Execute();
					Active->insert(ActIt,tmp);
					--ActIt;}
			}



		}








		if(state==::BACKWARD){
			now=clock();
			LocalTime=LocalTime-(now-last);
			last=now;
			//	if(hide){hide=false;(*(--ActIt))->Execute();}
			if(LocalTime<=0){state=::PAUSED; LocalTime=0;}
			while(ActIt!=Active->end() && (*ActIt)->timestamp >= LocalTime)
			{
				(*ActIt)->Unexecute();
				++ActIt;
			}


		}

		timeprint(LocalTime);


	}while(key!=KB_ESC && !Win());







	return 0;
}
