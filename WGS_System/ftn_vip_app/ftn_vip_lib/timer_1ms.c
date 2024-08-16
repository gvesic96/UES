/*
 * timer_1ms.c
 *
 * Created: 27-Aug-19 12:12:27 PM
 *  Author: Milan Lukic
 */ 

#include "timer_1ms.h"
#include <stdio.h>

static uint16_t LED_CNT_RELOAD = 500;

volatile uint32_t sys_time = 0;
volatile uint16_t LED_cnt = 1;
bool LED_enabled = false;
static struct timer_task TIMER_1ms_task;

static void TIMER_1ms_cb(const struct timer_task *const timer_task)
{
	//handle sys_time and LED	
	sys_time++;
	
	if(LED_enabled)
	{
		if (LED_CNT_RELOAD)
		{
			if (!--LED_cnt)
			{
				LED_cnt = LED_CNT_RELOAD;
				gpio_toggle_pin_level(LED);
			}
		}
		else
			gpio_set_pin_level(LED, true);
		
	}
}

void timer_1ms_init(void)
{
	TIMER_1ms_task.interval = 1;
	TIMER_1ms_task.cb       = TIMER_1ms_cb;
	TIMER_1ms_task.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_1ms, &TIMER_1ms_task);
	timer_start(&TIMER_1ms);
}

uint32_t millis()
{
	uint32_t tmp;
	__disable_irq;
	tmp = sys_time;
	__enable_irq;
	
	return tmp;
}

void delay(uint32_t d)
{
	uint32_t t0 = millis();
	
	while (millis() - t0 < d);
}

void enableLED()
{
	LED_enabled = true;
	gpio_set_pin_level(LED, true);
	LED_cnt = LED_CNT_RELOAD;
}

void disableLED()
{
	LED_enabled = false;
	gpio_set_pin_level(LED, false);
}

void setLEDfreq(uint16_t freq)
{
	LED_CNT_RELOAD = freq;
}