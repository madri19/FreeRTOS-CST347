/*
 * ledDriver.c
 *
 * Created: 1/19/2020 5:15:29 PM
 *  Author: admin
 */ 


#include <asf.h>
#include "myDefines.h"
#include "ledDriver.h"


void initializeLEDDriver(void){
	// Responsible for initializing the LEDs
	// Call from the Hardware Init Function in main.
	// Setup the ports for the LEDs and set them to the OFF state.
	
	// Initialize IO
	ioport_init(); 
	
	// Setup 3 LEDs GPIO to Output
	ioport_set_pin_dir(EXT_LED1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(EXT_LED2, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(EXT_LED3, IOPORT_DIR_OUTPUT);
	
	// Set LEDs to OFF
	ioport_set_pin_level(EXT_LED1, _INACTIVE_);
	ioport_set_pin_level(EXT_LED2, _INACTIVE_);
	ioport_set_pin_level(EXT_LED3, _INACTIVE_);
}


uint8_t readLED(uint8_t uiLedNum){
	// Return the current state of the given LED
	// uiLedNum Defined as:
	// 0 = Onboard LED
	// 1 = EXT_LED1
	// 2 = EXT_LED2
	// 3 = EXT_LED3

	if (uiLedNum == 0)
	{
		return ioport_get_pin_level(LED_0_PIN);
	}
	else if (uiLedNum == 1)
	{
		return ioport_get_pin_level(EXT_LED1);
	}
	else if (uiLedNum == 2)
	{
		return ioport_get_pin_level(EXT_LED2);
	}
	else if (uiLedNum == 3)
	{
		return ioport_get_pin_level(EXT_LED3);
	}
	
	// Throw Error
	return 3;
}


uint8_t setLED(uint8_t uiLedNum, uint8_t uiLedValue){
	// Set the LED to a specified value (ON/OFF)
	
	if (uiLedNum == 0)
	{
		ioport_set_pin_level(LED_0_PIN, uiLedValue);
		return readLED(uiLedNum);
	}	
	else if (uiLedNum == 1)
	{
		ioport_set_pin_level(EXT_LED1, uiLedValue);
		return readLED(uiLedNum);
	}
	else if (uiLedNum == 2)
	{
		ioport_set_pin_level(EXT_LED2, uiLedValue);
		return readLED(uiLedNum);
	}
	else if (uiLedNum == 3)
	{
		ioport_set_pin_level(EXT_LED3, uiLedValue);
		return readLED(uiLedNum);
	}
	
	// Throw Error
	return 3;
}

uint8_t toggleLED(uint8_t uiLedNum){
	// Toggles the LED
	// If on turns off
	// If off turns on

	if (uiLedNum >= 0 && uiLedNum <= 3){

		if (readLED(uiLedNum) == _INACTIVE_)
		{
			return setLED(uiLedNum, _ACTIVE_);
		}
		else
		{
			return setLED(uiLedNum, _INACTIVE_);
		}
	}
	
	// Throw Error
	return 3;
}