/*
  MIT License
  
  Copyright (c) 2017 PeterEmbedded
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "BH1750FVI.h"
#include "timer_1ms.h"

//private:
    
/*
* Write data to the I2C port
* @param Data, data to write
*/
void BH1750FVI_I2CWrite(uint8_t Data);  

#define BH1750FVI_I2C_ADDR	0x23			//!< I2C address of the device
eDeviceMode_t m_DeviceMode = k_DevModeContHighRes;	//!< Mode of the device
struct io_descriptor *I2C_sens_io;

void BH1750FVI_begin(void)
{
	struct io_descriptor *I2C_sens_io;
	I2C_SENS_init();
	i2c_m_sync_get_io_descriptor(&I2C_SENS, &I2C_sens_io);
	i2c_m_sync_enable(&I2C_SENS);
	i2c_m_sync_set_slaveaddr(&I2C_SENS, BH1750FVI_I2C_ADDR, I2C_M_SEVEN);
	
	BH1750FVI_I2CWrite(k_DevStatePowerUp);      // Turn it On 
	//BH1750FVI_I2CWrite(k_DevStateReset);
	BH1750FVI_SetMode(m_DeviceMode);            // Set the mode
	delay(1000);
}
  
void BH1750FVI_Sleep(void)
{
	BH1750FVI_I2CWrite(k_DevStatePowerDown); // Turn it off
}

void BH1750FVI_Reset(void)
{
	BH1750FVI_I2CWrite(k_DevStatePowerUp);  // Turn it on before we can reset it
	BH1750FVI_I2CWrite(k_DevStateReset );   // Reset
}

void BH1750FVI_SetMode(eDeviceMode_t DeviceMode)
{
  m_DeviceMode = DeviceMode;
  delay(10);
  BH1750FVI_I2CWrite(m_DeviceMode);
}

uint16_t BH1750FVI_GetLightIntensity(void)
{
  uint8_t data[2];
  i2c_m_sync_set_slaveaddr(&I2C_SENS, BH1750FVI_I2C_ADDR, I2C_M_SEVEN);
  io_read(I2C_sens_io, data, 2);

	return data[0] << 8 | data[1];
}

void BH1750FVI_I2CWrite(uint8_t Data)
{
	uint8_t data = Data;
	i2c_m_sync_set_slaveaddr(&I2C_SENS, BH1750FVI_I2C_ADDR, I2C_M_SEVEN);
	io_write(I2C_sens_io, &data, 1);
}
