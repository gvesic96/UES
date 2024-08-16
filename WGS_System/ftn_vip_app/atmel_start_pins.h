/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAML21 has 9 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8

#define PA03 GPIO(GPIO_PORTA, 3)
#define PA04 GPIO(GPIO_PORTA, 4)
#define SPI_SS GPIO(GPIO_PORTA, 5)
#define PA06 GPIO(GPIO_PORTA, 6)
#define PA07 GPIO(GPIO_PORTA, 7)
#define PA08 GPIO(GPIO_PORTA, 8)
#define PA09 GPIO(GPIO_PORTA, 9)
#define PA10 GPIO(GPIO_PORTA, 10)
#define PA11 GPIO(GPIO_PORTA, 11)
#define MCU_FLASH_EN GPIO(GPIO_PORTA, 12)
#define PA22 GPIO(GPIO_PORTA, 22)
#define PA23 GPIO(GPIO_PORTA, 23)
#define PA24 GPIO(GPIO_PORTA, 24)
#define PA25 GPIO(GPIO_PORTA, 25)
#define SENSOR_EN GPIO(GPIO_PORTA, 27)
#define LED GPIO(GPIO_PORTB, 2)
#define NBIOT_RST GPIO(GPIO_PORTB, 6)
#define PB08 GPIO(GPIO_PORTB, 8)
#define PB09 GPIO(GPIO_PORTB, 9)
#define HIGH_CURR_EN GPIO(GPIO_PORTB, 14)
#define BUTTON GPIO(GPIO_PORTB, 23)
#define PB30 GPIO(GPIO_PORTB, 30)
#define PB31 GPIO(GPIO_PORTB, 31)

#endif // ATMEL_START_PINS_H_INCLUDED
