/*
 * nbiotUART.h
 *
 * Created: 26-Aug-19 5:49:25 PM
 *  Author: Milan Lukic
 */ 

#ifndef NBIOTUART_H_
#define NBIOTUART_H_

#include <atmel_start.h>

void nbiotUARTinit(void);
uint16_t nbiotUARTavailable(void);
void nbiotUARTputChar(char c);
void nbiotUARTputString(char *str);
char nbiotUARTgetChar(void);
void nbiotUARTgetString(char *str);
void nbiotUARTsendHex(uint8_t hex);
void nbiotUARTflush(void);

#endif /* NBIOTUART_H_ */