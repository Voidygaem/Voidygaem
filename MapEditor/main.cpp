#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <fstream>
using namespace std;



HANDLE hStdout;
void gotoxy(COORD c)
{
SetConsoleCursorPosition(hStdout,c);
}



int main()
{
	ofstream map ("lol.map",ios::binary);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,0xF0);
	COORD curr={0,0};
	const COORD SYMB={45,3};
	int ch=1;
	int s=217;
	for(int i=0;i<2500;++i){cout<<' ';};
	char chars[40*20];
	for(int i=0;i<800;++i){chars[i]=0x20;}
	gotoxy(curr);
	while(ch!=27)
	{
		ch=getch();
		if (ch==0xE0)
		{
			ch=getch();
			switch (ch)
			{
			case 77:
				++curr.X;
				if(curr.X==40){curr.X=0;}
				gotoxy(curr);
				break;
			case 75:
				if(curr.X==0){curr.X=40;}
				--curr.X;
				gotoxy(curr);
				break;
			case 80:
				++curr.Y;
				if(curr.Y==20){curr.Y=0;}
				gotoxy(curr);
				break;
			case 72:
				if(curr.Y==0){curr.Y=20;}
				--curr.Y;
				gotoxy(curr);
				break;
			}
		};
		switch(ch)
		{
		case 45:
			--s;if(s==175){s=219;}
			gotoxy(SYMB);
			cout<<char(s)<<" "<<s;
			gotoxy(curr);
			break;
		case 43:
			++s;if(s==220){s=176;}
			gotoxy(SYMB);
			cout<<char(s)<<" "<<s;
			gotoxy(curr);
			break;
		case 32:
			cout<<char(s);
			gotoxy(curr);
			chars[curr.Y*40+curr.X]=(char)s;
			break;
		}

		
	

	}
		map.write(chars,sizeof(chars));
		map.close();
	
	return 0;
}
