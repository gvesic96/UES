/*
 * debugUART.h
 *
 * Created: 26-Aug-19 5:49:25 PM
 *  Author: Milan Lukic
 */ 

#ifndef DEBUGUART_H_
#define DEBUGUART_H_

#include <atmel_start.h>

void debugUARTdriverInit(void);
uint16_t debugUARTavailable(void);
void debugUARTputChar(char c);
void debugUARTputSample(uint8_t sample);
void debugUARTputString(char *str);
char debugUARTgetChar(void);
void debugUARTgetString(char *str);
void debugUARTsendHex(uint8_t hex);
void debugUARTflush(void);

#endif /* DEBUGUART_H_ */