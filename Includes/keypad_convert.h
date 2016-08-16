#ifndef KEYPAD_CONVERT_H
#define KEYPAD_CONVERT_H

#include <bit.h>
#include <keypad.h>

unsigned char keypad_convert()

{
	unsigned char x;
	x = GetKeypadKey();

	switch (x) {

		case '\0': return 0x1F; // All 5 LEDs on

		case '1': return 0x01;  // hex equivalent

		case '2': return 0x02;
		
		case '3': return 0x03;
		
		case '4': return 0x04;
		
		case '5': return 0x05;
		
		case '6': return 0x06;
		
		case '7': return 0x07;
		
		case '8': return 0x08;
		
		case '9': return 0x09;
		
		case 'A': return 0x0A;
		
		case 'B': return 0x0B;
		
		case 'C': return 0x0C;

		// . . . ***** FINISH *****

		case 'D': return 0x0D;

		case '*': return 0x0E;

		case '0': return 0x00;

		case '#': return 0x0F;

		default: return 0x1B; // Should never occur. Middle LED off.

	}

}

#endif