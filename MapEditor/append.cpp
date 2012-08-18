#include <fstream>
#include <cstdio>
using namespace std;
int appnd_d()
{
	FILE* map = fopen("lol.map","ab+");
	short a=16;
	fwrite(&a,sizeof(short),1,map);
	return 0;
}