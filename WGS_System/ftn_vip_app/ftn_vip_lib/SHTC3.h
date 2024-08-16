#ifndef SF_SHTC3
#define SF_SHTC3

#include <atmel_start.h>

#define SHTC3_ADDR_7BIT	0b1110000
#define SHTC3_ADDR_WRITE 0b11100000
#define SHTC3_ADDR_READ  0b11100001

#define SHTC3_MAX_CLOCK_FREQ 1000000

typedef enum{
	SHTC3_CMD_WAKE = 0x3517,
	SHTC3_CMD_SLEEP = 0xB098,

	SHTC3_CMD_SFT_RST = 0x805D,

	SHTC3_CMD_READ_ID = 0xEFC8,
}SHTC3_Commands_TypeDef;

typedef enum{
	SHTC3_CMD_CSE_RHF_NPM = 0x5C24,		// Clock stretching, RH first, Normal power mode
	SHTC3_CMD_CSE_RHF_LPM = 0x44DE,		// Clock stretching, RH first, Low power mode
	SHTC3_CMD_CSE_TF_NPM = 0x7CA2,		// Clock stretching, T first, Normal power mode
	SHTC3_CMD_CSE_TF_LPM = 0x6458,		// Clock stretching, T first, Low power mode
	
	SHTC3_CMD_CSD_RHF_NPM = 0x58E0,		// Polling, RH first, Normal power mode
	SHTC3_CMD_CSD_RHF_LPM = 0x401A,		// Polling, RH first, Low power mode
	SHTC3_CMD_CSD_TF_NPM = 0x7866,		// Polling, T first, Normal power mode
	SHTC3_CMD_CSD_TF_LPM = 0x609C		// Polling, T first, Low power mode
}SHTC3_MeasurementModes_TypeDef;

typedef enum{
	SHTC3_Status_Nominal = 0,			// The one and only "all is good" return value
	SHTC3_Status_Error,					// The most general of error values - can mean anything depending on the context	
	SHTC3_Status_CRC_Fail,				// This return value means the computed checksum did not match the provided value
	SHTC3_Status_ID_Fail				// This status means that the ID of the device did not match the format for SHTC3
}SHTC3_Status_TypeDef;

SHTC3_Status_TypeDef	SHTC3_begin();											// Initializes the sensor
SHTC3_Status_TypeDef 	SHTC3_softReset();										// Resets the sensor into a known state through software
SHTC3_Status_TypeDef	SHTC3_checkID();										// Asks the sensor for the ID and checks that value against a CRC checksum
SHTC3_Status_TypeDef	SHTC3_sleep(bool hold);									// Wakes up the sensor. If hold is true then sets _staySleeping to false 
SHTC3_Status_TypeDef	SHTC3_wake(bool hold);									// Sleeps the sensor. If hold is true then sets _staySleeping to true
SHTC3_Status_TypeDef	SHTC3_setMode(SHTC3_MeasurementModes_TypeDef mode);		// Sets the desired measurement mode from the options in the typedef "SHTC3_MeasurementModes_TypeDef"
SHTC3_MeasurementModes_TypeDef	SHTC3_getMode( void );
SHTC3_Status_TypeDef 	SHTC3_update();											// Tells the sensor to take a measurement and updates the member variables of the object
SHTC3_Status_TypeDef	SHTC3_checkCRC(uint16_t packet, uint8_t cs);			// Checks CRC values

float SHTC3_raw2DegC(void);												// Converts SHTC3 T data to deg C
float SHTC3_raw2DegF(void);												// Converts SHTC3 T data to deg F
float SHTC3_raw2Percent(void);											// Converts SHTC3 RH data to % RH

#endif /* SF_SHTC3 */