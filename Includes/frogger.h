#ifndef FROGGER_H
#define FROGGER_H

#include <matrix.h>
#include <player.h>

enum terrain{LAND, WATER, SAFE, WIN};
enum direction{LEFT, RIGHT, STILL};
enum difficulty{EASY, MED, HARD};

typedef struct _Row
{
	char contents[16];
	enum terrain type;
	enum direction dir;
	short speed;
	
} Row;

//Function generates a new row with randomly generated hazards
void generateRow(Row* r,const Row* rprev)
{

	static char rnd;
	rnd = rand() % 100;
	r->type = (rnd <= 55) ? LAND:WATER;
	if(rprev->type == WATER && rprev->dir == LEFT)
	{r->dir = RIGHT;}
	else if(rprev->type == WATER && rprev->dir == RIGHT)
	{r->dir = LEFT;}
	else
	{
		rnd = rand() % 50;
		r->dir = (rnd > 24) ? LEFT:RIGHT;
	}
		
	char hazard_count = 0;
	char h_size;
	char pos;
	for(char i = 0; i <16; ++i)
	{		
		if(hazard_count >= 3)
		{r->contents[i] = 0;}
		else if(r->type == LAND)
		{
			rnd = rand() % 10;
			if(rnd >= 7)
			{
				h_size = (rand() % 3) + 1;
				for(pos = 0; i<16 && pos < h_size; ++pos, ++i)
				{r->contents[i] = 1;}
				hazard_count++;
				if(i < 16)
				{++i;}
			}
			else
			{r->contents[i] = 0;}
		}
		else if(r->type == WATER)
		{
			rnd = rand() % 7;
			if( hazard_count == 0 || rnd > 2)
			{
				if(hazard_count != 0)
				{h_size = (rand() % 3) + 1;}
				else
				{h_size = (rand() % 2) + 2;}
				for(pos = 0; i<16 && pos < h_size; ++pos, ++i)
				{r->contents[i] = 1;}
				hazard_count++;
				if(i < 16)
				{++i;}
			}
			else
			{r->contents[i] = 0;}
		}		
	}
	rnd = rand() % 6;
	r->speed = 50 * (rnd  + 8);
}


//Generates a safe starting row for player
void generateSafe(Row* r)
{
	r->dir=STILL;
	r->type=SAFE;
	r->speed = 100;
	memset(&(r->contents),0,sizeof(r->contents));
}

//Creates the victory row
void generateWin(Row* r)
{
	r->dir = STILL;
	r->type = WIN;
	r->speed = 100;
	memset(&(r->contents),0,sizeof(r->contents));
}

//Struct uses an array of 8 rows to create an area
typedef struct _Area
{
	Row space[8];
} Area;

//Function draws the Area into a matrix struct
void drawWorld(const Area a, Matrix* w)
{
	for(char i = 0; i < 8; ++i)
	{
		for(char j = 0; j < 8; ++j)
		{
			if(a.space[i].type == LAND)
			{
				if(a.space[i].contents[j] >= 1)
				{w->grid[i][j] = RED;}
				else
				{w->grid[i][j] = 0;}
			}
			
			else if(a.space[i].type == WATER)
			{
					if(a.space[i].contents[j] >= 1)
					{w->grid[i][j] = 0;}
					else
					{w->grid[i][j] = BLUE;}
			}
			else
			{
				w->grid[i][j] = PURPLE;
			}
		}
	}
}

//Function generates 8 rows to create an area
void generateArea(Area* a)
{
	generateWin(&(a->space[0]));
	for(char i = 1; i < 7; ++i)
	{generateRow(&(a->space[i]),&(a->space[i-1]));}
	generateSafe(&(a->space[7]));
}

//Tells the array of a row to "advance" allowing hazards to move
void progressRow(Row* r, Player* p,const char rowNum)
{
	char temp;
	if(r->dir == LEFT)
	{
		temp = r->contents[15];
		for(char i = 15; i > 0; --i)
		{r->contents[i] = r->contents[i-1];}
		r->contents[0] = temp;
		if(r->type == WATER && p->x == rowNum)
		{
			moveLeft(p);
		}
	}
	else if(r->dir == RIGHT)
	{
		temp = r->contents[0];
		for(char i = 0; i < 15; ++i)
		{r->contents[i] = r->contents[i+1];}
		r->contents[15] = temp;
		if(r->type == WATER && p->x == rowNum)
		{
			moveRight(p);
		}
	}
}

//detects collision between player and hazards
char detectCollision(const Area a, const Player p)
{
	if(a.space[p.x].contents[p.y] >= 1 && a.space[p.x].type == LAND)
	{
		return 1;
	}
	else if(a.space[p.x].contents[p.y] == 0 && a.space[p.x].type == WATER)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


#endif