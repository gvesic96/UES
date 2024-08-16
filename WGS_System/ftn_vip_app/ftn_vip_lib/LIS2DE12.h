/*
 * LIS2DE12.h
 *
 * Created: 13-Nov-19 1:38:07 PM
 *  Author: Milan Lukic
 */ 


#ifndef LIS2DE12_H_
#define LIS2DE12_H_

#include <atmel_start.h>

typedef struct{
	int8_t accel_X;
	int8_t accel_Y;
	int8_t accel_Z;	
}accel_3axis;

#define ACCEL_STATUS_REG_AUX	0x07
#define ACCEL_OUT_TEMP_L		0x0C
#define ACCEL_OUT_TEMP_H		0x0D
#define ACCEL_WHO_AM_I			0x0F
#define ACCEL_CTRL_REG0			0x1E
#define ACCEL_TEMP_CFG_REG		0x1F
#define ACCEL_CTRL_REG1			0x20
#define ACCEL_CTRL_REG2			0x21
#define ACCEL_CTRL_REG3			0x22
#define ACCEL_CTRL_REG4			0x23
#define ACCEL_CTRL_REG5			0x24
#define ACCEL_CTRL_REG6			0x25
#define ACCEL_REFERENCE			0x26
#define ACCEL_STATUS_REG		0x27
#define ACCEL_FIFO_READ_START	0x28
#define ACCEL_OUT_X_H			0x29
#define ACCEL_OUT_Y_H			0x2B
#define ACCEL_OUT_Z_H			0x2D
#define ACCEL_FIFO_CTRL_REG		0x2E
#define ACCEL_FIFO_SRC_REG		0x2F
#define ACCEL_INT1_CFG			0x30
#define ACCEL_INT1_SRC			0x31
#define ACCEL_INT1_THS			0x32
#define ACCEL_INT1_DURATION		0x33
#define ACCEL_INT2_CFG			0x34
#define ACCEL_INT2_SRC			0x35
#define ACCEL_INT2_THS			0x36
#define ACCEL_INT2_DURATION		0x37
#define ACCEL_CLICK_CFG			0x38
#define ACCEL_CLICK_SRC			0x39
#define ACCEL_CLICK_THS			0x3A
#define ACCEL_TIME_LIMIT		0x3B
#define ACCEL_TIME_LATENCY		0x3C
#define ACCEL_TIME_WINDOW		0x3D
#define ACCEL_ACT_THS			0x3E
#define ACCEL_ACT_DUR			0x3F

typedef enum 
{
	LIS2DE12_POWER_DOWN                      = 0x00,
	LIS2DE12_ODR_1Hz                         = 0x10,
	LIS2DE12_ODR_10Hz                        = 0x20,
	LIS2DE12_ODR_25Hz                        = 0x30,
	LIS2DE12_ODR_50Hz                        = 0x40,
	LIS2DE12_ODR_100Hz                       = 0x50,
	LIS2DE12_ODR_200Hz                       = 0x60,
	LIS2DE12_ODR_400Hz                       = 0x70,
	LIS2DE12_ODR_1kHz620_LP                  = 0x80,
	LIS2DE12_ODR_5kHz376_LP_1kHz344_NM_HP    = 0x90,
} lis2de12_odr_t;

bool LIS2DE12_init(void);
void LIS2DE12_getAccel(accel_3axis *a);
void LIS2DE12_enableClickInterrupt(void);
void LIS2DE12_resetInterruptPin(void);
void LIS2DE12_regDump(uint8_t first, uint8_t last);
void LIS2DE12_powerDown(void);

#endif /* LIS2DE12_H_ */