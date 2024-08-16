#include "SHTC3.h"
#include "debugUART.h"
#include <stdio.h>

SHTC3_MeasurementModes_TypeDef _mode;

bool 						_inProcess;														// True when a macro-scale function is in progress in the library - blocks small functions from messing with sleep functions
bool 						_staySleeping;													// Used to indicate if the sensor should be kept in sleep mode
bool 						_isAsleep;														// Used to indicate if a wake() command is needed before talking

SHTC3_Status_TypeDef 		SHTC3_sendCommand_CTD(SHTC3_Commands_TypeDef cmd);
SHTC3_Status_TypeDef 		SHTC3_sendCommand_MMTD(SHTC3_MeasurementModes_TypeDef cmd);		// Overloaded version of send command to support the "measurement type" commands
SHTC3_Status_TypeDef 		SHTC3_abortUpdate(SHTC3_Status_TypeDef status);					// Used to bail from an update. Sets reading values to known fail state
SHTC3_Status_TypeDef		SHTC3_exitOp(SHTC3_Status_TypeDef status);						// Used to bail from any other operation - puts the sensor back to sleep

SHTC3_Status_TypeDef		SHTC3_startProcess( void );										// Used to wake up the sensor and set the _inProcess variable to true
SHTC3_Status_TypeDef 		SHTC3_endProcess( void );										// Used to end processes as the user desires
SHTC3_Status_TypeDef		SHTC3_checkCRC(uint16_t packet, uint8_t cs);
SHTC3_Status_TypeDef		lastStatus; 															// Stores the most recent status result for access by the user

bool		passRHcrc;																		// Indicates if the current value of RH has passed the CRC check
bool 		passTcrc;																		// Indicates if the current value of T has passed the CRC check
bool		passIDcrc;																		// Indicates if the current value of ID has passed the CRC check

uint16_t 	RH;																				// Current raw RH data from the sensor
uint16_t	T;																				// Current raw T data from the sensor
uint16_t 	ID;																				// Current raw ID data from the sensor

float toDegC();																				// Returns the floating point value of T in deg C
float toDegF();																				// Returns the floating point value of T in deg F
float toPercent();																			// Returns the floating point value of RH in % RH

struct io_descriptor *I2C_sens_io;

SHTC3_Status_TypeDef 	SHTC3_sendCommand_CTD(SHTC3_Commands_TypeDef cmd)
{
	uint8_t buff[2];
	buff[0] = (((uint16_t)cmd) >>     8);
	buff[1] = (((uint16_t)cmd) & 0x00FF);
	io_write(I2C_sens_io, buff, 2);

	return SHTC3_Status_Nominal;
}

SHTC3_Status_TypeDef 	SHTC3_sendCommand_MMTD(SHTC3_MeasurementModes_TypeDef cmd)
{
	return SHTC3_sendCommand_CTD((SHTC3_Commands_TypeDef)cmd);
}

SHTC3_Status_TypeDef SHTC3_abortUpdate(SHTC3_Status_TypeDef status)		// Sets the values to a known error state
{
	passRHcrc = false;
	passTcrc = false;
	passIDcrc = false;

	RH = 0x00;
	T = 0x00;
	ID = 0x00;

	return SHTC3_exitOp(status);
}

SHTC3_Status_TypeDef SHTC3_exitOp(SHTC3_Status_TypeDef status)
{
	lastStatus = status;
	return status;
}

SHTC3_Status_TypeDef SHTC3_startProcess( void )
{
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;
	_inProcess = true;
	if(_isAsleep)
		retval = SHTC3_wake(false);
	if(retval == SHTC3_Status_Nominal)
		_isAsleep = false;
	return SHTC3_Status_Nominal;
}

SHTC3_Status_TypeDef 	SHTC3_endProcess( void )
{
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;
	_inProcess = false;
	if(_staySleeping)
		retval = SHTC3_sleep(false);
	if(retval == SHTC3_Status_Nominal)
		_isAsleep = true;
	return SHTC3_Status_Nominal;
}

float SHTC3_toDegC()
{
	return SHTC3_raw2DegC(); 
}

float SHTC3_toDegF()
{
	return SHTC3_raw2DegF(); 
}

float SHTC3_toPercent()
{
	return SHTC3_raw2Percent();
}

SHTC3_Status_TypeDef	SHTC3_begin()
{
	_mode = SHTC3_CMD_CSE_RHF_NPM;	// My default pick

	_inProcess = false;				// Definitely not doing anything when the object is intantiated
	_staySleeping = true;			// Default to storing the sensor in low-power mode
	_isAsleep = true;				// Assume the sensor is asleep to begin (there won't be any harm in waking it up if it is already awake)

	passRHcrc = false;
	passTcrc = false;
	passIDcrc = false;

	RH = 0x00;
	T = 0x00;
	ID = 0x00;

	I2C_SENS_init();
	i2c_m_sync_get_io_descriptor(&I2C_SENS, &I2C_sens_io);
	i2c_m_sync_enable(&I2C_SENS);
	i2c_m_sync_set_slaveaddr(&I2C_SENS, SHTC3_ADDR_7BIT, I2C_M_SEVEN);
	
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;

	retval = SHTC3_startProcess();																// Multiple commands will go to the sensor before sleeping
	if(retval != SHTC3_Status_Nominal)
		return SHTC3_Status_Error;

	retval = SHTC3_wake(false);																	// Wake up the sensor
	if(retval != SHTC3_Status_Nominal)
		return SHTC3_Status_Error;		

	retval = SHTC3_checkID();																	// Verify that the sensor is actually an SHTC3
	if(retval != SHTC3_Status_Nominal)
		return SHTC3_Status_Error;

	retval = SHTC3_endProcess();	
															// We are about to return to user-land
	if(retval != SHTC3_Status_Nominal)
		return SHTC3_Status_Error;

	return SHTC3_Status_Nominal;
}

SHTC3_Status_TypeDef 	SHTC3_softReset()
{
	return SHTC3_sendCommand_CTD(SHTC3_CMD_SFT_RST);
}

SHTC3_Status_TypeDef	SHTC3_checkID()
{
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;
	const uint8_t numBytesRequest = 3;
	uint8_t numBytesRx = 0x00;

	retval = SHTC3_startProcess();																	// There will be several commands sent before returning control to the user
	if (retval != SHTC3_Status_Nominal)
		return SHTC3_abortUpdate(retval);

	retval = SHTC3_wake(false);
	if(retval != SHTC3_Status_Nominal){ return SHTC3_exitOp(retval); }

	retval = SHTC3_sendCommand_CTD(SHTC3_CMD_READ_ID);
	
	if (retval != SHTC3_Status_Nominal)
		return SHTC3_Status_Error;

	uint8_t buff[3];
	io_read(I2C_sens_io, buff, numBytesRequest);
	uint8_t IDhb = buff[0];
	uint8_t IDlb = buff[1];
	uint8_t IDcs = buff[2];

	ID = (((uint16_t)IDhb << 8) | ((uint16_t)IDlb));
	
	passIDcrc = false;

	if(SHTC3_checkCRC(ID, IDcs) == SHTC3_Status_Nominal){ passIDcrc = true; }

	if((ID & 0b0000100000111111) != 0b0000100000000111)									// Checking the form of the ID
		return SHTC3_exitOp(SHTC3_Status_ID_Fail); 						// to identify an SHTC3				

	return SHTC3_endProcess();		
}

SHTC3_Status_TypeDef	SHTC3_sleep(bool hold)
{
	_isAsleep = true;						// It is fail-safe to always assume the sensor is asleep (whether or not the sleep command actually took effect)
	if(hold)
		_staySleeping = true;
	return SHTC3_sendCommand_CTD(SHTC3_CMD_SLEEP);
}

SHTC3_Status_TypeDef	SHTC3_wake(bool hold)
{
	SHTC3_Status_TypeDef retval = SHTC3_sendCommand_CTD(SHTC3_CMD_WAKE);
	if(retval == SHTC3_Status_Nominal)
		_isAsleep = false;					// Only when the wake command was sent successfully can you assume the sensor is awake
	if(hold)
		_staySleeping = false;
	delay_us(240);
	SHTC3_Status_Nominal;
}

SHTC3_Status_TypeDef	SHTC3_setMode(SHTC3_MeasurementModes_TypeDef mode)
{
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;
	switch(mode)																// Switch used to disallow unknown types
	{
		case SHTC3_CMD_CSE_RHF_NPM : _mode = SHTC3_CMD_CSE_RHF_NPM; break;
		case SHTC3_CMD_CSE_RHF_LPM : _mode = SHTC3_CMD_CSE_RHF_LPM; break;
		case SHTC3_CMD_CSE_TF_NPM : _mode = SHTC3_CMD_CSE_TF_NPM; break;
		case SHTC3_CMD_CSE_TF_LPM : _mode = SHTC3_CMD_CSE_TF_LPM; break;			
		default:
			retval = SHTC3_Status_Error;
			break;
	}
	return SHTC3_Status_Nominal;
}

SHTC3_MeasurementModes_TypeDef	SHTC3_getMode( void )
{
	return _mode;
}

SHTC3_Status_TypeDef 	SHTC3_update()
{
	i2c_m_sync_set_slaveaddr(&I2C_SENS, SHTC3_ADDR_7BIT, I2C_M_SEVEN);
	
	SHTC3_Status_TypeDef retval = SHTC3_Status_Nominal;

	const uint8_t numBytesRequest = 6;
	uint8_t numBytesRx = 0;

	uint8_t RHhb = 0x00;
	uint8_t RHlb = 0x00;
	uint8_t RHcs = 0x00;

	uint8_t Thb = 0x00;
	uint8_t Tlb = 0x00;
	uint8_t Tcs = 0x00;
	
	uint8_t buff[6];

	retval = SHTC3_startProcess();		
	if(retval != SHTC3_Status_Nominal){ return SHTC3_abortUpdate(retval); }

	retval = SHTC3_sendCommand_MMTD(_mode);							// Send the appropriate command - Note: incorrect commands are excluded by the 'setMode' command and _mode is a protected variable
	if(retval != SHTC3_Status_Nominal){ return SHTC3_abortUpdate(retval); }	
	
	switch(_mode)											// Handle the two different ways of waiting for a measurement (polling or clock stretching)
	{
		case SHTC3_CMD_CSE_RHF_NPM : 						
		case SHTC3_CMD_CSE_RHF_LPM : 
		case SHTC3_CMD_CSE_TF_NPM : 
		case SHTC3_CMD_CSE_TF_LPM : 						// Address+read will yield an ACK and then clock stretching will occur
			numBytesRx = numBytesRequest;
			io_read(I2C_sens_io, buff, numBytesRequest);
			break;

		case SHTC3_CMD_CSD_RHF_NPM :
		case SHTC3_CMD_CSD_RHF_LPM :
		case SHTC3_CMD_CSD_TF_NPM :
		case SHTC3_CMD_CSD_TF_LPM :							// These modes not yet supported (polling - need to figure out how to repeatedly send just address+read and look for ACK)
		default : 
			return SHTC3_abortUpdate(SHTC3_Status_Error);		// You really should never get to this code because setMode disallows non-approved values of _mode (type SHTC3_MeasurementModes_TypeDef)
			break;
	}

	// Now handle the received data
	if(numBytesRx != numBytesRequest){ return SHTC3_abortUpdate(SHTC3_Status_Error); }		// Hopefully we got the right number of bytes

	switch(_mode)														// Switch for the order of the returned results
	{
		case SHTC3_CMD_CSE_RHF_NPM :
		case SHTC3_CMD_CSE_RHF_LPM :
		case SHTC3_CMD_CSD_RHF_NPM :
		case SHTC3_CMD_CSD_RHF_LPM :
			// RH First
			RHhb = buff[0];
			RHlb = buff[1];
			RHcs = buff[2];
			
			Thb = buff[3];
			Tlb = buff[4];
			Tcs = buff[5];
			break;
		case SHTC3_CMD_CSE_TF_NPM :
		case SHTC3_CMD_CSE_TF_LPM :
		case SHTC3_CMD_CSD_TF_NPM :
		case SHTC3_CMD_CSD_TF_LPM :
			// T First
			Thb = buff[0];
			Tlb = buff[1];
			Tcs = buff[2];

			RHhb = buff[3];
			RHlb = buff[4];
			RHcs = buff[5];
			break;
		default : 
			return SHTC3_abortUpdate(SHTC3_Status_Error);						// Again, you should never experience this section of code
			break;
	}
	// Update values
	RH = ((uint16_t)RHhb << 8) | ((uint16_t)RHlb << 0);
	T = ((uint16_t)Thb << 8) | ((uint16_t)Tlb << 0);

	passRHcrc = false;
	passTcrc = false;

	if(SHTC3_checkCRC(RH, RHcs) == SHTC3_Status_Nominal){ passRHcrc = true; }
	if(SHTC3_checkCRC(T, Tcs) == SHTC3_Status_Nominal){ passTcrc = true; }
	
	retval = SHTC3_endProcess();												// We are about to return to user-land
	if(retval != SHTC3_Status_Nominal){ return SHTC3_Status_Error; }

	return SHTC3_Status_Nominal;
}

SHTC3_Status_TypeDef SHTC3_checkCRC(uint16_t packet, uint8_t cs)
{
	uint8_t data[2] = {((packet & 0xFF00) >> 8), ((packet & 0x00FF) >> 0)};
	uint8_t crc = 0xFF;
	uint8_t poly = 0x31;
  
	for(uint8_t indi = 0; indi < 2; indi++)
	{
		crc ^= data[indi];

		for(uint8_t indj = 0; indj < 8; indj++)
		{
		    if(crc & 0x80)
		      crc = (uint8_t)((crc << 1) ^ poly);
		    else
		      crc <<= 1;
		}
	}

	if(cs ^ crc){ return SHTC3_exitOp(SHTC3_Status_CRC_Fail); }
	return SHTC3_exitOp(SHTC3_Status_Nominal);
}


float SHTC3_raw2DegC(void)
{
	return -45 + 175 * ((float)T/65535);
}

float SHTC3_raw2DegF(void)
{
	return SHTC3_raw2DegC() * (9.0/5) + 32.0;
}

float SHTC3_raw2Percent(void)
{
	return 100 * ((float)RH/65535);
}
