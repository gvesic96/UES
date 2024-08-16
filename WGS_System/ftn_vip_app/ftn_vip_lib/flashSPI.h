#ifndef FLASH_SPI_H_
#define FLASH_SPI_H_

#include <atmel_start.h>
#include "debugUART.h"

void flashSPIdebugEnable(bool value);
void flashSPIdriverInit(void );
void flashSPIceSet(bool value);
void flashSPIreadID(uint8_t* id_buffer);
void flashSPIreadJEDECID(uint8_t* jedec_id_buffer);
void flashSPIreadStatusReg(uint8_t* status_reg);
void flashSPIwriteEnable(bool value);
void flashSPIread(uint32_t address, uint8_t* buffer, uint32_t size);
void flashSPIchipErase(bool block);
void flashSPIpageProgram(uint32_t address, uint8_t* buffer, uint32_t size);
void flashSPISectorErase(uint32_t address, bool size64k);
void flashSPIpowerDown(bool release);

#endif /* FLASH_SPI_H_ */