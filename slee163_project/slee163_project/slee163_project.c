/*
 * slee163_project.c
 *
 * Created: 5/24/2013 2:12:54 PM
 *  Author: Spencer Lee
 */ 


#include <avr/io.h>
#include <scheduler.h>
#include <matrix.h>
#include <timer.h>
#include <player.h>
#include <frogger.h>
#include <lcd_8bit_task.h>

char ainput = 0;
char gameon = 0;

Area World;
Matrix game_world;
Player plr1;

unsigned char LCD_string_1[16];
unsigned char LCD_string_2[16];

char score = 0;

//Task for game logic (menu scoring loss)	
enum GL_States{initg, menu, ingame1, ingame2, nextLevel, game_over, waitRelease};
	
int GL_Tick(int state)
{
	static int timer = 0;
	static char buffer[10];
	switch(state)
	{
	case initg:
		state = menu;
		drawFrog(&game_world);
		timer = 0;
		break;
	case menu:
		if(ainput == 0)
		{state = menu;}
		else if((ainput & 0xF0) != 0)
		{
			score = 0;
			srand(timer);
			generateArea(&World);
			initiatePlayer(&plr1,7,4,GREEN,3);
			state = ingame1;
		}
		break;
	case ingame1:
		if(plr1.lives == 0)
		{
			state = game_over;
			fillMatrix(&game_world,RED);
		}
		else if(World.space[plr1.x].type == WIN)
		{
			score++;
			state = nextLevel;
		}
		else
		{state = ingame1;}
		break;
	case nextLevel:
		generateArea(&World);
		initiatePlayer(&plr1,7,plr1.y,GREEN,plr1.lives);
		state = ingame1;
		break;
	case game_over:
		if(ainput == 0)
		{
			state = game_over;
		}
		else if((ainput & 0xF0) != 0)
		{
			state = waitRelease;
			drawFrog(&game_world);
			timer = 0;
		}
		break;
	case waitRelease:
		if(ainput == 0)
		{state = menu;}
		else
		{state = waitRelease;}
		break;
	default:
		state = initg;
		break;
		
	}			
	memset(LCD_string_1,' ',sizeof(LCD_string_1));
	memset(LCD_string_2,' ',sizeof(LCD_string_2));
	switch(state)
	{
	case menu:

		strcpy(LCD_string_1,"Frogger Endless");
		strcpy(LCD_string_2,"1 Player");
		gameon = 0;
		timer++;
		break;
	case waitRelease:
		strcpy(LCD_string_1,"Frogger Endless");
		strcpy(LCD_string_2,"1 Player");
		gameon = 0;
		timer++;
		break;
	case ingame1:
		if(detectCollision(World,plr1) == 1)
		{
			playerDeath(&plr1);
		}
		memset(buffer,0,sizeof(buffer));
		itoa(plr1.lives,buffer,10);
		strcpy(LCD_string_1,"Lives:       ");
		strcpy(LCD_string_1 + 7,buffer);
		itoa(score,buffer,10);		
		strcpy(LCD_string_2,"Score:   ");
		strcpy(LCD_string_2 + 7,buffer);
		gameon = 1;
		break;
	case nextLevel:
		memset(buffer,0,sizeof(buffer));
		itoa(plr1.lives,buffer,10);
		strcpy(LCD_string_1,"Lives:       ");
		strcpy(LCD_string_1 + 7,buffer);
		itoa(score,buffer,10);
		strcpy(LCD_string_2,"Score:   ");
		strcpy(LCD_string_2 + 7,buffer);
		break;
	case game_over:
		strcpy(LCD_string_1,"GAME OVER!");
		itoa(score,buffer,10);
		strcpy(LCD_string_2,"Score: ");
		strcpy(LCD_string_2 + 7,buffer);
		gameon = 0;
		break;
	default:
		break;
	}
	return state;
}	
	
	
//Controls LCD messages
enum LT_States { LT_s0, LT_WaitLcdRdy, LT_FillAndDispString, LT_HoldGo1 } LT_State;

int LT_Tick(int LT_state) {
    switch(LT_State) { // Transitions
        case -1:
            LT_State = LT_s0;
            break;
        case LT_s0:
            LT_State = LT_WaitLcdRdy;
            break;
        case LT_WaitLcdRdy:
            if (!LCD_rdy_g) {
                LT_State = LT_WaitLcdRdy;
            }
            else if (LCD_rdy_g) {
                LT_State = LT_FillAndDispString;
            }
            break;
        case LT_FillAndDispString:
            LT_State = LT_HoldGo1;
            break;
		case LT_HoldGo1:
			LCD_go_g=0;
			LT_State = LT_WaitLcdRdy;
			break;
        default:
            LT_State = LT_s0;
        } // Transitions

    switch(LT_State) { // State actions
        case LT_s0:
            LCD_go_g=0;
			strcpy(LCD_string_g, "1234567890123456");
            break;
        case LT_WaitLcdRdy:
            break;
        case LT_FillAndDispString:
			memset(LCD_string_g,0,sizeof(LCD_string_g));
			strcpy(LCD_string_g,LCD_string_1);
			strcpy(LCD_string_g+16,LCD_string_2);
            LCD_go_g = 1; // Display string
            break;
        case LT_HoldGo1:
            break;
        default:
            break;
    } // State actions
	return LT_State;
}


//Controls Matrix updating
enum UP_States{initu, UP_s0};
	
int UP_Tick(int state)
{
	static char column = 0;
	switch(state)
	{
		case initu:
			state = UP_s0;
			break;
		case UP_s0:
			state = UP_s0;
			break;
		default:
			state = initu;
			break;
	}

	switch(state)
	{
		case UP_s0:
			if(gameon == 1 || gameon == 2)
			{
				drawWorld(World,&game_world);
				place_player(&game_world,plr1);
				print(game_world,column);
				if(column >= 7)
				{column = 0;}
				else
				{column = column + 1;}
			}					
			else
			{
				print(game_world,column);
				if(column >= 7)
				{column = 0;}
					else
				{column = column + 1;}
			}		
			break;
		default:
			break;
	}
	return state;
}


//Task polls for movement
enum MV_States{inits, MV_s0, MV_sN, MV_sW, MV_sE, MV_sS, MV_sWait};
	
int MV_Tick(int state)
{
	if(gameon == 1)
	{
		switch(state)
		{
			case inits:
			state = MV_s0;
			break;
			case MV_s0:
			switch(ainput)
			{
				case 0x01:
				moveLeft(&plr1);
				state = MV_sWait;
				break;
				case 0x02:
				moveUp(&plr1);
				state = MV_sWait;
				break;
				case 0x04:
				moveDown(&plr1);
				state = MV_sWait;
				break;
				case 0x08:
				moveRight(&plr1);
				state = MV_sWait;
				break;
				default:
				state = MV_s0;
				break;
			}
			case MV_sWait:
				if(ainput == 0x00)
				{state = MV_s0;}
				else
				{state = MV_sWait;}
				break;
				default:
				state = inits;
				break;
			}
			
			switch(state)
			{
				case MV_s0:
				break;
				case MV_sWait:
				break;
				defaut:
				break;
			}
	}	
	return state;
}


//Task updates the roads/ rivers
enum RD_States{RD_s0, RD_s1, RD_s2, RD_s3, RD_s4, RD_s5, AV_s0, AV_s1, AV_s2, AV_s3, AV_s4, AV_s5};

int RD_Tick(int state)
{
	switch(state)
	{
		default:
		break;
	}
	switch(state)
	{
		case RD_s0:
		if(gameon == 1)
		{
			progressRow(&(World.space[1]),&plr1,1);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,1);
		}
		break;
		case RD_s1:
		if(gameon == 1)
		{
			progressRow(&(World.space[2]),&plr1,2);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,2);
		}
		break;
		case RD_s2:
		if(gameon == 1)
		{
			progressRow(&(World.space[3]),&plr1,3);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,3);
		}
		break;
		case RD_s3:
		if(gameon == 1)
		{
			progressRow(&(World.space[4]),&plr1,4);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,4);
		}
		break;
		case RD_s4:
		if(gameon == 1)
		{
			progressRow(&(World.space[5]),&plr1,5);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,5);
		}
		break;
		case RD_s5:
		if(gameon == 1)
		{
			progressRow(&(World.space[6]),&plr1,6);
			drawWorld(World,&game_world);
			place_player(&game_world,plr1);
			print(game_world,6);
		}
		break;
		default:
		break;
	}
	return state;
}


int main(void)
{
	
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
		
	unsigned long int SMTick1_period = 100;		//GAME LOGIC
	unsigned long int SMTick2_period = 10;		//LCD WRITER
	unsigned long int SMTick3_period = 5;		//LCD DISPLAY
	unsigned long int SMTick4_period = 1;		//LED MATRIX
	unsigned long int SMTick5_period = 100;		//MOVE POLL
	
	unsigned long int SMTick6_period = 100;		//ROADS & RIVERS
	unsigned long int SMTick7_period = 100;
	unsigned long int SMTick8_period = 100;
	unsigned long int SMTick9_period = 100;
	unsigned long int SMTick10_period = 100;
	unsigned long int SMTick11_period = 100;
	
	
	static task task1, task2, task3, task4, task5, task6, task7, task8, task9, task10, task11;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8, &task9, &task10, &task11};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = initg;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &GL_Tick;//Function pointer for the tick.
	
	
	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &LT_Tick;//Function pointer for the tick.
	
	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period;//Task current elapsed time.
	task3.TickFct = &LCDI_SMTick;//Function pointer for the tick.
	
	// Task 4
	task4.state = initu;//Task initial state.
	task4.period = SMTick4_period;//Task Period.
	task4.elapsedTime = SMTick4_period;//Task current elapsed time.
	task4.TickFct = &UP_Tick;//Function pointer for the tick.
	
	// Task 5
	task5.state = inits;//Task initial state.
	task5.period = SMTick5_period;//Task Period.
	task5.elapsedTime = SMTick5_period;//Task current elapsed time.
	task5.TickFct = &MV_Tick;//Function pointer for the tick.
	
	// Task 6
	task6.state = RD_s0;//Task initial state.
	task6.period = SMTick6_period;//Task Period.
	task6.elapsedTime = SMTick6_period;//Task current elapsed time.
	task6.TickFct = &RD_Tick;//Function pointer for the tick.
	
	// Task 7
	task7.state = RD_s1;//Task initial state.
	task7.period = SMTick7_period;//Task Period.
	task7.elapsedTime = SMTick7_period;//Task current elapsed time.
	task7.TickFct = &RD_Tick;//Function pointer for the tick.	
	
	// Task 8
	task8.state = RD_s2;//Task initial state.
	task8.period = SMTick8_period;//Task Period.
	task8.elapsedTime = SMTick8_period;//Task current elapsed time.
	task8.TickFct = &RD_Tick;//Function pointer for the tick.
	
	// Task 9
	task9.state = RD_s3;//Task initial state.
	task9.period = SMTick9_period;//Task Period.
	task9.elapsedTime = SMTick9_period;//Task current elapsed time.
	task9.TickFct = &RD_Tick;//Function pointer for the tick.
	
	// Task 10
	task10.state = RD_s4;//Task initial state.
	task10.period = SMTick10_period;//Task Period.
	task10.elapsedTime = SMTick10_period;//Task current elapsed time.
	task10.TickFct = &RD_Tick;//Function pointer for the tick.
	
	// Task 11
	task11.state = RD_s5;//Task initial state.
	task11.period = SMTick11_period;//Task Period.
	task11.elapsedTime = SMTick11_period;//Task current elapsed time.
	task11.TickFct = &RD_Tick;//Function pointer for the tick.
	int i;
	
	TimerSet(1);
	TimerOn();
	
	while(1)
	{	
		ainput = ~PINC;
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			//constructMatrix();
			if ( tasks[i]->elapsedTime >= tasks[i]->period )
			{
				if(i >= 5 && i <= 10)
				{tasks[i]->period = World.space[i - 4].speed;}
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		
		while(!TimerFlag);
		TimerFlag = 0;

	}
}