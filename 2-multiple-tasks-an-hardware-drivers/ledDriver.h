/*
 * ledDriver.h
 *
 * Created: 1/19/2020 5:15:42 PM
 *  Author: admin
 */ 


#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_


void initializeLEDDriver(void);
uint8_t readLED(uint8_t uiLedNum);
uint8_t setLED(uint8_t uiLedNum, uint8_t uiLedValue); 
uint8_t toggleLED(uint8_t uiLedNum); 


#endif /* LEDDRIVER_H_ */