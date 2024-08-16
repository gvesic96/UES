/*
 * timer_1ms.h
 *
 * Created: 27-Aug-19 12:11:59 PM
 *  Author: Milan Lukic
 */ 


#ifndef TIMER_1MS_H_
#define TIMER_1MS_H_

#include <atmel_start.h>

#define FREQ_01HZ	5000
#define FREQ_1HZ	500
#define FREQ_2HZ	250
#define FREQ_5HZ	100
#define FREQ_10HZ	50
#define FREQ_ON		0

void timer_1ms_init(void);
uint32_t millis(void);
void delay(uint32_t d);
void enableLED(void);
void disableLED(void);
void setLEDfreq(uint16_t freq);

#endif /* TIMER_1MS_H_ */