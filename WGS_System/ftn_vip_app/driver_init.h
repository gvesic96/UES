/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_adc_async.h>

#include <hal_calendar.h>
#include <hal_spi_m_sync.h>

#include <hal_i2c_m_sync.h>
#include <hal_usart_async.h>
#include <hal_usart_async.h>

#include <hal_i2c_m_sync.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>

#include "hal_usb_device.h"

extern struct adc_async_descriptor ADC_0;

extern struct calendar_descriptor   CALENDAR;
extern struct spi_m_sync_descriptor EXT_FLASH;

extern struct i2c_m_sync_desc        Crypto_Interface;
extern struct usart_async_descriptor debugUART;
extern struct usart_async_descriptor nbiotUART;

extern struct i2c_m_sync_desc  I2C_SENS;
extern struct timer_descriptor TIMER_1ms;

void ADC_0_init(void);

void CALENDAR_CLOCK_init(void);
void CALENDAR_init(void);

void EXT_FLASH_PORT_init(void);
void EXT_FLASH_CLOCK_init(void);
void EXT_FLASH_init(void);

void Crypto_Interface_CLOCK_init(void);
void Crypto_Interface_init(void);
void Crypto_Interface_PORT_init(void);

void debugUART_PORT_init(void);
void debugUART_CLOCK_init(void);
void debugUART_init(void);

void nbiotUART_PORT_init(void);
void nbiotUART_CLOCK_init(void);
void nbiotUART_init(void);

void I2C_SENS_CLOCK_init(void);
void I2C_SENS_init(void);
void I2C_SENS_PORT_init(void);

void USB_DEVICE_INSTANCE_CLOCK_init(void);
void USB_DEVICE_INSTANCE_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
