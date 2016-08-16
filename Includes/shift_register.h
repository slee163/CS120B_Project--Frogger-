#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#define RED 1
#define BLUE 2
#define GREEN 3
#define PURPLE 4
#define YELLOW 5
#define TEAL 6
#define WHITE 7
#define ALL 8

#define SERIAL 0x01
#define CTRL_RED 0x20
#define CTRL_BLUE 0x80
#define CTRL_GREEN 0x40
#define CLEAR 0x10
#define LCD_CONTROL PORTD
#define COLOR_CONTROL PORTB
#define OUTPUT_CONTROL PORTD

//Some functions use code from the shift register lab

//tells registers to display contents
void reg_outputs_on(char col)
{
	switch(col)
	{
	case ALL:
		OUTPUT_CONTROL &= 0x0F;
	case RED:
		OUTPUT_CONTROL  &= 0xDF; 
		break;
	case BLUE:
		OUTPUT_CONTROL  &= 0x7F;
		break;
	case GREEN:
		OUTPUT_CONTROL  &= 0xEF; 
		break;
	default:
		break;
	}
	
}

//tells registers to stop displaying contents
void reg_outputs_off(char col)
{
	switch(col)
	{
		case ALL:
		OUTPUT_CONTROL |= ~0x0F;
		case RED:
		OUTPUT_CONTROL |= ~0xDF;
		break;
		case BLUE:
		OUTPUT_CONTROL |= ~0x7F;
		break;
		case GREEN:
		OUTPUT_CONTROL |= ~0xEF;
		break;
		default:
		break;
	}
}

//trasmits data to the color shift registers
void transmit_data(unsigned char data, unsigned char col) {

	char i;
    for (i = 0; i < 8 ; ++i) {

	    // Sets SRCLR to 1 allowing data to be set
	    // Also clears SRCLK in preparation of sending data
	    COLOR_CONTROL = CLEAR;
	    // set SER = next bit of data to be sent.
	    COLOR_CONTROL |= ((data >> i) & 0x01);
	    // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		switch(col)
		{
		case RED:
			COLOR_CONTROL |= 0x02;
			break;
		case BLUE:
			COLOR_CONTROL |= 0x08;
			break;
		case GREEN:
			COLOR_CONTROL |= 0x04;
			break;
		default:
			break;
		}
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    //PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
	
    COLOR_CONTROL = 0x00;
}

//transmits data to the column shift register
void transmit_Column(unsigned char data) {
	for (char i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD &= 0xF0;
		PORTD |= 0x08;
		// set SER = next bit of data to be sent.
		PORTD |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x02;
	}
	PORTD |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTD &= 0xF0;
}
#endif