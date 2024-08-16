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

/* 
 *  Digital Light sensor BH1750FVI (GY-302)
 */
  
#ifndef BH1750FVI_h
#define BH1750FVI_h

#include <atmel_start.h>

/*
 * BH1750FVI driver class
 * Provides an easy interface to the BH1750FVI digital lightsensor
 */
typedef enum eDeviceAddress {
	k_DevAddress_L = 0x23,
	k_DevAddress_H = 0x5C
} eDeviceAddress_t;

typedef enum eDeviceMode {
	k_DevModeContHighRes     = 0x10,
	k_DevModeContHighRes2    = 0x11,
	k_DevModeContLowRes      = 0x13,
	k_DevModeOneTimeHighRes  = 0x20,
	k_DevModeOneTimeHighRes2 = 0x21,
	k_DevModeOneTimeLowRes   = 0x23
} eDeviceMode_t;

typedef enum eDeviceState {
	k_DevStatePowerDown = 0x00,
	k_DevStatePowerUp   = 0x01,
	k_DevStateReset     = 0x07
} eDeviceState_t;
    
    
/*
* The defacto standard begin call to initialize and start the driver
* This will take care of setting the address, mode and powering up the device
*/
void BH1750FVI_begin(void);

/*
* Get the measured light intensity from the device
* @return The light intensity measured by the device in 'lux'
*/
uint16_t BH1750FVI_GetLightIntensity(void);
    
/*
* Set the mode
* @param DeviceMode, the mode to set the device in
*/
void BH1750FVI_SetMode(eDeviceMode_t DeviceMode);

/*
* Activate sleep mode
*/
void BH1750FVI_Sleep(void); 
    
/*
* Reset the device
*/
void BH1750FVI_Reset(void);
       
#endif
