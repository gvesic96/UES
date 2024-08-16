#include "flashSPI.h"

bool flashSPIdebug = false;

static struct io_descriptor* flash_io;

void flashSPIdebugEnable(bool value)
{
	flashSPIdebug = value;
}

void flashSPIdriverInit(void ) {
	spi_m_sync_get_io_descriptor(&EXT_FLASH, &flash_io);
	spi_m_sync_enable(&EXT_FLASH);
}

void flashSPIceSet(bool value) {
	gpio_set_pin_level(SPI_SS, value);
}



void flashSPIreadID(uint8_t* id_buffer)
{
	uint8_t command[4] = {0xAB, 0x00, 0x00, 0x00};
	
	flashSPIceSet(false);
	
 	io_write(flash_io, command, 1);
 	io_read(flash_io, id_buffer, 4);
	 
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("Flash ID: ");
		for(uint8_t i = 0; i < 4; i++) {
			debugUARTsendHex(id_buffer[i]);
		}
		debugUARTputString("\r\n");
	}
}

void flashSPIreadJEDECID(uint8_t* jedec_id_buffer)
{
	uint8_t command = 0x9F;
	
	flashSPIceSet(false);
	
	io_write(flash_io, &command, 1);
	io_read(flash_io, jedec_id_buffer, 4);
	
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("JEDEC ID: ");
		for(uint8_t i = 0; i < 4; i++) {
			debugUARTsendHex(jedec_id_buffer[i]);
		}
		debugUARTputString("\r\n");
	}
}

void flashSPIreadStatusReg(uint8_t* status_reg)
{
	uint8_t command = 0x05;
	
	flashSPIceSet(false);
	
	io_write(flash_io, &command, 1);
	io_read(flash_io, status_reg, 1);
	
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("Status register: ");
		debugUARTsendHex(*status_reg);
		debugUARTputString("\r\n");
	}
}

void flashSPIwriteEnable(bool value)
{
	uint8_t command;
	
	if(value)
		command = 0x06;
	else
		command = 0x04;
	
	flashSPIceSet(false);
	
	io_write(flash_io, &command, 1);
	
	flashSPIceSet(true);
	
	if(flashSPIdebug)
	{
		if(value)
			debugUARTputString("Write enabled.\r\n");
		else
			debugUARTputString("Write disabled.\r\n");
	}
}

void flashSPIread(uint32_t address, uint8_t* buffer, uint32_t size)
{
	uint8_t command[4];
	
	command[0] = 0x03;
	command[1] = (uint8_t)((address & 0xff0000) >> 16);
	command[2] = (uint8_t)((address & 0xff00) >> 8);
	command[3] = (uint8_t)(address & 0xff);
	
	flashSPIceSet(false);
	
	io_write(flash_io, command, 4);
	io_read(flash_io, buffer, size);
	
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("Read from memory: ");
		for(uint8_t i = 0; i < size; i++) {
			debugUARTsendHex(buffer[i]);
		}
		debugUARTputString("\r\n");
	}
}

void flashSPIchipErase(bool block)
{
	uint8_t command = 0x60;
	uint8_t status;
	
	
	flashSPIwriteEnable(true);
	
	flashSPIceSet(false);
	
	io_write(flash_io, &command, 1);
	
	flashSPIceSet(true);
	
	if(block) {
		do {
			flashSPIreadStatusReg(&status);
		} while(status & 1);
	}
	
	if(flashSPIdebug)
	{
		debugUARTputString("Chip erased.\r\n");
	}
}

void flashSPIpageProgram(uint32_t address, uint8_t* buffer, uint32_t size) 
{
	uint8_t command[4];
	
	command[0] = 0x02;
	command[1] = (uint8_t)((address & 0xff0000) >> 16);
	command[2] = (uint8_t)((address & 0xff00) >> 8);
	command[3] = (uint8_t)(address & 0xff);
	
	flashSPIwriteEnable(true);
	
	flashSPIceSet(false);
	
	io_write(flash_io, command, 4);
	io_write(flash_io, buffer, size);
	
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("Data written in memory: ");
		for(uint8_t i = 0; i < size; i++) {
			debugUARTsendHex(buffer[i]);
		}
		debugUARTputString("\r\n");
	}
}

void flashSPISectorErase(uint32_t address, bool size64k)
{
	uint8_t command[4];
	
	if(size64k)
		command[0] = 0xD8;
	else
		command[0] = 0x20;
	command[1] = (uint8_t)((address & 0xff0000) >> 16);
	command[2] = (uint8_t)((address & 0xff00) >> 8);
	command[3] = (uint8_t)(address & 0xff);
	
	flashSPIwriteEnable(true);
	
	flashSPIceSet(false);
	
	io_write(flash_io, command, 4);
	
	flashSPIceSet(true);

	if(flashSPIdebug)
	{
		debugUARTputString("Sector of ");
		if(size64k)
			debugUARTputString("64k ");
		else
			debugUARTputString("4k ");
		debugUARTputString("erased on 0x");
		debugUARTsendHex(command[3]);
		debugUARTsendHex(command[2]);
		debugUARTsendHex(command[1]);
		debugUARTputString(".\r\n");
	}
}

void flashSPIpowerDown(bool release)
{
	uint8_t command;
	
	if(release)
		command = 0xAB;
	else
		command = 0xB9;
	
	flashSPIceSet(false);
	
	io_write(flash_io, &command, 1);
	
	flashSPIceSet(true);
	
	if(flashSPIdebug)
	{
		if(!release)
			debugUARTputString("Device put in deep power down mode.\r\n");
		else
			debugUARTputString("Device woken up from deep power down mode.\r\n");
	}
}