#ifndef MATRIX_H
#define MATRIX_H

#include <shift_register.h>

typedef struct _Matrix
{
	char grid[8][8];
} Matrix;

//Fills the matrix with a given color
void fillMatrix(Matrix* m, char color)
{
	for(char i = 0; i < 8; ++i)
	{
		for(char j = 0; j < 8; ++j)
		{
			m->grid[i][j] = color;
		}
	}
}

//Deep copies a matrix pattern into the matrix
void copyPattern(Matrix*m, const char source[8][8])
{
	for(char i = 0; i < 8; ++i)
	{
		for(char j = 0; j < 8; ++j)
		{
			m->grid[i][j] = source[i][j];
		}
	}
}

//Frog shape for main menu
void drawFrog(Matrix*m)
{
	char newGrid[8][8] = {
	{0,0,0,0,0,0,0,0},
	{3,3,0,0,0,0,3,3},
	{3,3,0,3,3,0,3,3},
	{0,3,3,3,3,3,3,0},
	{0,0,3,3,3,3,0,0},
	{0,0,3,3,3,3,0,0},
	{3,3,3,0,0,3,3,3},
	{3,3,3,0,0,3,3,3}
	};
	copyPattern(m,newGrid);
}

//Outputs specified column of matrix to led matrix
void print(Matrix m, unsigned char column)
{
	if(column == 8)
	{
		reg_outputs_off(ALL);
		return;
	}
	char rblue = 0;
	char rred = 0;
	char rgreen = 0;
	
		for(char j = 0; j < 8; j++)
		{
			switch(m.grid[column][j])
			{
				case RED:
				rred |= (0x01 << j);
				break;
				case BLUE:
				rblue |= (0x01 << j);
				break;
				case GREEN:
				rgreen |= (0x01 << j);
				break;
				case PURPLE:
				rred |= (0x01 << j);
				rblue |= (0x01 << j);
				break;
				case YELLOW:
				rred |= (0x01 << j);
				rgreen |= (0x01 << j);
				break;
				case TEAL:
				rblue |= (0x01 << j);
				rgreen |= (0x01 << j);
				break;
				case WHITE:
				rred |= (0x01 << j);
				rgreen |= (0x01 << j);
				rblue |= (0x01 << j);
				break;
				defaut:
				break;
			}
		}
		reg_outputs_off(ALL);
		
		transmit_data(~rred,RED);
		transmit_data(~rgreen,GREEN);
		transmit_data(~rblue,BLUE);
		transmit_Column(0x01 << (7 - column));
		
		reg_outputs_on(ALL);
	
}
#endif