/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
}

/**
 * Example of using ADC_0 to generate waveform.
 */
void ADC_0_example(void)
{
	adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	adc_async_enable_channel(&ADC_0, 0);
	adc_async_start_conversion(&ADC_0);
}

/**
 * Example of using CALENDAR.
 */
static struct calendar_alarm alarm;

static void alarm_cb(struct calendar_descriptor *const descr)
{
	/* alarm expired */
}

void CALENDAR_example(void)
{
	struct calendar_date date;
	struct calendar_time time;

	calendar_enable(&CALENDAR);

	date.year  = 2000;
	date.month = 12;
	date.day   = 31;

	time.hour = 12;
	time.min  = 59;
	time.sec  = 59;

	calendar_set_date(&CALENDAR, &date);
	calendar_set_time(&CALENDAR, &time);

	alarm.cal_alarm.datetime.time.sec = 4;
	alarm.cal_alarm.option            = CALENDAR_ALARM_MATCH_SEC;
	alarm.cal_alarm.mode              = REPEAT;

	calendar_set_alarm(&CALENDAR, &alarm, alarm_cb);
}

/**
 * Example of using EXT_FLASH to write "Hello World" using the IO abstraction.
 */
static uint8_t example_EXT_FLASH[12] = "Hello World!";

void EXT_FLASH_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&EXT_FLASH, &io);

	spi_m_sync_enable(&EXT_FLASH);
	io_write(io, example_EXT_FLASH, 12);
}

void Crypto_Interface_example(void)
{
	struct io_descriptor *Crypto_Interface_io;

	i2c_m_sync_get_io_descriptor(&Crypto_Interface, &Crypto_Interface_io);
	i2c_m_sync_enable(&Crypto_Interface);
	i2c_m_sync_set_slaveaddr(&Crypto_Interface, 0x12, I2C_M_SEVEN);
	io_write(Crypto_Interface_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using debugUART to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_debugUART[12] = "Hello World!";

static void tx_cb_debugUART(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void debugUART_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&debugUART, USART_ASYNC_TXC_CB, tx_cb_debugUART);
	/*usart_async_register_callback(&debugUART, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&debugUART, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&debugUART, &io);
	usart_async_enable(&debugUART);

	io_write(io, example_debugUART, 12);
}

/**
 * Example of using nbiotUART to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_nbiotUART[12] = "Hello World!";

static void tx_cb_nbiotUART(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void nbiotUART_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&nbiotUART, USART_ASYNC_TXC_CB, tx_cb_nbiotUART);
	/*usart_async_register_callback(&nbiotUART, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&nbiotUART, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&nbiotUART, &io);
	usart_async_enable(&nbiotUART);

	io_write(io, example_nbiotUART, 12);
}

void I2C_SENS_example(void)
{
	struct io_descriptor *I2C_SENS_io;

	i2c_m_sync_get_io_descriptor(&I2C_SENS, &I2C_SENS_io);
	i2c_m_sync_enable(&I2C_SENS);
	i2c_m_sync_set_slaveaddr(&I2C_SENS, 0x12, I2C_M_SEVEN);
	io_write(I2C_SENS_io, (uint8_t *)"Hello World!", 12);
}

static struct timer_task TIMER_1ms_task1, TIMER_1ms_task2;

/**
 * Example of using TIMER_1ms.
 */
static void TIMER_1ms_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_1ms_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_1ms_example(void)
{
	TIMER_1ms_task1.interval = 100;
	TIMER_1ms_task1.cb       = TIMER_1ms_task1_cb;
	TIMER_1ms_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_1ms_task2.interval = 200;
	TIMER_1ms_task2.cb       = TIMER_1ms_task2_cb;
	TIMER_1ms_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_1ms, &TIMER_1ms_task1);
	timer_add_task(&TIMER_1ms, &TIMER_1ms_task2);
	timer_start(&TIMER_1ms);
}
