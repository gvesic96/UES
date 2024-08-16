/*
 * LIS2DE12.c
 *
 * Created: 13-Nov-19 1:38:25 PM
 *  Author: Milan Lukic
 */ 

#include <stdio.h>
#include "timer_1ms.h"
#include "debugUART.h"
#include "LIS2DE12.h"

#define LIS2DE12_ADDR	0x19

struct io_descriptor *I2C_sens_io;

void i2c_read_byte(uint8_t i2cSlaveAddr, uint8_t regAddr, uint8_t *value)
{
	io_write(I2C_sens_io, &regAddr, 1);
	io_read(I2C_sens_io, value, 1);
}

i2c_write_byte(uint8_t i2cSlaveAddr,  uint8_t regAddr, uint8_t value)
{
	uint8_t buff[2];
	buff[0] = regAddr; buff[1] = value;
	io_write(I2C_sens_io, buff, 2);
}

i2c_read_packet(uint8_t i2cSlaveAddr,  uint8_t regAddr, uint8_t *buff, uint8_t length)
{
	io_write(I2C_sens_io, &regAddr, 1);
	io_read(I2C_sens_io, buff, length);
}

bool LIS2DE12_init(void)
{
	debugUARTputString("Accelerometer init... ");
	
	//I2C initialization
	I2C_SENS_init();
	i2c_m_sync_get_io_descriptor(&I2C_SENS, &I2C_sens_io);
	i2c_m_sync_enable(&I2C_SENS);
	i2c_m_sync_set_slaveaddr(&I2C_SENS, LIS2DE12_ADDR, I2C_M_SEVEN);

	uint8_t accel_ID;
	i2c_read_byte(LIS2DE12_ADDR, ACCEL_WHO_AM_I, &accel_ID);
	
	if (accel_ID != 0x33)	//pre-programmed fixed value 0x33
	{
		debugUARTputString("FAIL!\r\n");
		return false;
	}
	debugUARTputString("OK!\r\n");
	
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CTRL_REG1, LIS2DE12_ODR_400Hz | 0x0F);	//ODR = 1Hz
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CTRL_REG6, 0x02);						//INT pin polarity active-low

	return true;
}

void LIS2DE12_getAccel(accel_3axis *a)
{
	uint8_t tmp[6];
	i2c_read_packet(LIS2DE12_ADDR, ACCEL_FIFO_READ_START | 0x80, tmp, 6);
	
	a->accel_X = tmp[1];
	a->accel_Y = tmp[3];
	a->accel_Z = tmp[5];
}

void LIS2DE12_enableClickInterrupt(void)
{
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CTRL_REG3, 0x80);	//enable CLICK interrupt on INT1 pin
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CLICK_CFG, 0x15);	//enable single-click interrupt on x, y and z
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CLICK_THS, 0x88);	//LIR_click = 1; threshold = 8


	i2c_write_byte(LIS2DE12_ADDR, ACCEL_TIME_LIMIT, 0x01);
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_TIME_LATENCY, 0x01);
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_TIME_WINDOW, 0x01);
	
}

void LIS2DE12_resetInterruptPin(void)
{
	uint8_t tmp;
	i2c_read_byte(LIS2DE12_ADDR, ACCEL_CLICK_SRC, &tmp);
}

void LIS2DE12_regDump(uint8_t first, uint8_t last)
{
	for (uint8_t regAddr = first; regAddr < last + 1; regAddr++)
	{
		uint8_t tmp;
		
		//when reading multiple bytes, the MSB of the register address must be asserted
		i2c_read_byte(LIS2DE12_ADDR, regAddr, &tmp);
		char str[32];
		sprintf(str, "reg[0x%02x] = 0x%02x\r\n", regAddr, tmp);
		debugUARTputString(str);
		delay(10);
	}
}

void LIS2DE12_powerDown(void)
{
	debugUARTputString("Accelerometer sleep... ");
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CTRL_REG1, 0x00);	//Power-down mode
	i2c_write_byte(LIS2DE12_ADDR, ACCEL_CTRL_REG6, 0x02);	//INT pin polarity active-low
	debugUARTputString("OK!\r\n");
}