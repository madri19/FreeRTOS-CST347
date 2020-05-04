/*
 * buttonDriver.c
 *
 * Created: 1/19/2020 7:16:13 PM
 *  Author: admin
 */ 


#include <asf.h>
#include "myDefines.h"
#include "buttonDriver.h"


void initializeButtonDriver(void){
	// Responsible for initializing the Buttons
	// Call from the Hardware Init Function in main.
	// Setup the ports for the Buttons.
		
	// Initialize IO
	ioport_init();
		
	// Setup 2 Buttons direction, mode, and Sense
	ioport_set_pin_dir(EXT_SW1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(EXT_SW2, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(EXT_SW1, (IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE));
	ioport_set_pin_mode(EXT_SW2, (IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE));
	ioport_set_pin_sense_mode(EXT_SW1, (IOPORT_SENSE_RISING));
	ioport_set_pin_sense_mode(EXT_SW2, (IOPORT_SENSE_RISING));
}


uint8_t readButton(uint8_t uiButtonNum){
	
	if (uiButtonNum == 0)
	{
		return ioport_get_pin_level(BUTTON_0_PIN);
	}
	else if (uiButtonNum == 1)
	{
		return ioport_get_pin_level(EXT_SW1);
	}
	else if (uiButtonNum == 2)
	{
		return ioport_get_pin_level(EXT_SW2);
	}
		
	// Throw Error
	return 3;
}