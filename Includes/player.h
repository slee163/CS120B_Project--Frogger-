#ifndef PLAYER_H
#define PLAYER_H

#include <matrix.h>

//player struct contains player coordinates, lives, etc
typedef struct _Player
{
	char x;
	char y;
	char startx;
	char starty;
	char color;
	unsigned int lives;
} Player;

//creates a player with teh given specifications
void initiatePlayer(Player* p, char nx, char ny, char ncol,char nlives)
{
	p->x=nx;
	p->startx =nx;
	p->y=ny;
	p->starty = ny;
	p->color=ncol;
	p->lives=nlives;
}

//Places the player into the world matrix
void place_player(Matrix* m, Player p)
{
	m->grid[p.x][p.y] = p.color;
}

void moveLeft(Player* p)
{
	if(p->y < 7)
	{
		p->y++;
	}
	else if(p->x != 7 && p->x != 0)
	{
		playerDeath(p);
	}
}

void moveRight(Player* p)
{
	if(p->y > 0)
	{
		p->y--;
	}
	else if(p->x != 7 && p->x != 0)
	{
		playerDeath(p);
	}
}

void moveUp(Player* p)
{
	if(p->x > 0)
	{
		p->x--;
	}
}

void moveDown(Player* p)
{
	if(p->x < 7)
	{
		p->x++;
	}
}

//called on death, deducts lives and returns player to start
void playerDeath(Player* p)
{
	if(p->lives > 0)
	{p->lives--;}
	p->x = p->startx;
	p->y = p->starty;
}
#endif