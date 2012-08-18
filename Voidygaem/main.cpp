#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <ctime>
#include <list>
#include<cstdio>
using namespace std;
const int WIDTH=40;
const int HEIGHT=20;
const COORD TIME={45,3};
const COORD COORDS={45,5};
char mapchars[40*20],mappassability[40*20];


enum {NORMAL, PAUSED, BACKWARD, FORWARD} state;
enum KEYS {KB_NONE,KB_LEFT, KB_RIGHT, KB_UP, KB_DOWN, KB_SPACE, KB_ESC, KB_PLUS, KB_MINUS};
bool split=false;
int LocalTime=0;

int modulo(int a, int b)
{
	return a>=0?a%b:a%b+b;
}
struct Coord:public COORD
{
	Coord(short par1,short par2){X=par1;Y=par2;};
	Coord operator+(COORD c)
	{
		X=modulo(X+(c.X),(WIDTH));
		Y=modulo(Y+(c.Y),(HEIGHT));
		return *this;
	}
	Coord operator-(COORD c)
	{
		X=modulo(X-(c.X),(WIDTH));
		Y=modulo(Y-(c.Y),(HEIGHT));
		return *this;
	}
	bool operator==(COORD c)
	{
		return ((X==c.X)&&(Y==c.Y));
	}
};

HANDLE hStdout;
void gotoxy(COORD c)
{
SetConsoleCursorPosition(hStdout,c);
}

KEYS GetKey()
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
}

class Player
{
	Coord coord;
	int s;
	void Draw(int ch)
	{
		gotoxy(coord);
		cout<<(char)ch;
	}
public:
	bool exists;
	bool movable;
	bool visible;
	Player(int param1, Coord param2):s(param1),coord(param2),exists(true),movable(true),visible(true){};
	void Draw()
	{
		if(!movable){SetConsoleTextAttribute(hStdout,0x04);}
		Draw(s);
		if(!movable){SetConsoleTextAttribute(hStdout,0x0F);}
	}
	void Hide()
	{
		Draw((int)' ');
	}
	Coord GetPos()
	{
		return coord;
	}
	void SetPos(Coord param1)
	{
		Draw((int)' ');
		coord=param1;
	}
};
Player* pl;
list<Player *> Players;
void timeprint(unsigned int st)
{

	gotoxy(TIME);
	cout<<"Local time: "<<(st/100)<<"     ";
	gotoxy(COORDS);
	cout<<"X: "<<pl->GetPos().X<<" Y:"<<pl->GetPos().Y<<"    ";
}

class Command
{
protected:
	bool stamped;
public:
	unsigned int timestamp;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
};

class PlayerCommand :public Command
{
	Player *receiver;
	bool creating;
public:
	PlayerCommand(Player* param1, bool param2):receiver(param1),creating(param2){stamped=false;}
	void Execute()
	{
		if(!stamped){timestamp=LocalTime;stamped=true;}
		receiver->exists=creating;
		if(!creating){receiver->Hide();}
	}
	void Unexecute()
	{
		receiver->exists=!creating;
		if(creating){receiver->Hide();}
	}
};


class KeyCommand : public Command
{
	Player *receiver;
	KEYS key;
	Coord * c;


public:
	KeyCommand(Player* param1,KEYS param2):receiver(param1),key(param2)
	{
		stamped=false;
		switch(key)
		{
		case KB_LEFT: c= new Coord(1,0);break;
		case KB_RIGHT: c= new Coord(-1,0);break;
		case KB_UP: c= new Coord(0,-1);break;
		case KB_DOWN: c= new Coord(0,1);break;
		default: c=new Coord(0,0);
		};

	}
	void Execute()
	{
		receiver->SetPos(receiver->GetPos()+*c);
		if(!stamped){timestamp=LocalTime; stamped=true;}
	}
	void Unexecute()
	{
		receiver->SetPos(receiver->GetPos()-*c);
	}

};
class EmptyCommand:public Command
{

public:
	EmptyCommand(){stamped=false;}
	void Execute(){if(!stamped){timestamp=LocalTime;stamped=true;}}
	void Unexecute(){}
};


list<Command *> *Active;
list<Command *>::iterator ActIt;



void StateKeyHandler(KEYS k,unsigned int *last)
{
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
		Active->insert(ActIt, new PlayerCommand(pl,false));
		(*(--ActIt))->Execute();
	//	hide=true;
		Coord tmpcoord=pl->GetPos();
		pl->movable=true;
		pl->visible=false;
		pl=new Player((int)'@',tmpcoord);
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
		if(wasbackward){Active->insert(ActIt, new PlayerCommand(pl,true)); wasbackward=false;split=true;}
	}
	*last=clock();
}





void DrawPlayers()
{
	list<Player*>::iterator pls = Players.begin();
	for(;pls!=Players.end();++pls)
	{
		if((*pls)->exists || (*pls)->visible ){(*pls)->Draw();}
	}

}

void nop(){}




bool pass(KEYS key)
{
	
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
	Button(){c=Coord(0,0);}
	void Draw()
	{
		gotoxy(c);
		cout<<'O';
	}
	bool getpressed()
	{
		
		for(list<Player*>::iterator pls = Players.begin();pls!=Players.end();++pls)
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
			gotoxy(d[i]);
			
			cout<<(opened?' ':char(0xb3));
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
Coord startc(1,1),endc(1,1);
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
			gotoxy(Coord(j,i));
			ch = mapchars[i*40+j];
			cout<<ch;
			if(ch>=unsigned char(0xb3) && ch<=unsigned char(0xda))
			{
				mappassability[i*40+j]=1;
			}
		}
	}
	gotoxy(endc);
	cout<<'X';

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
		gotoxy(Coord(24,10));
		cout<<"You win! Press any key to exit.";
		getch();
		return true;
	}
	return false;
}

int main()
{
	FILE * map = fopen ("lol.map","rb");
	mapopen(map);
	//map.read(mapchars,sizeof(mapchars));
	Active= new list<Command *>();
	ActIt=Active->begin();
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,0x0F);

	for(int i=0;i<2500;++i){cout<<' ';};
	KEYS key;
	Command *tmp;
	for(int j=0;j<800;++j){::mappassability[j]=0;}


	pl= new Player((int)'@',startc);
	pl->exists=true;
	pl->movable=true;
	Players.push_back(pl);
	tmp=new EmptyCommand();
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
				tmp=new KeyCommand(pl,key);
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
