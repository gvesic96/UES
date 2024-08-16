#include "usbUART.h"
#include <hal_delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

volatile static char usb_Rx_Buffer[USB_RX_BUFFER_SIZE];	//RX FIFO buffer
volatile static unsigned char usb_Rx_Buffer_Size = 0;
volatile static unsigned char usb_Rx_Buffer_First = 0;
volatile static unsigned char usb_Rx_Buffer_Last = 0;

void usbUARTinit()
{
	usb_init();
}

uint8_t usbUARTavailable(void)
{
	return usb_Rx_Buffer_Size;	
}

void usbUARTwrite(unsigned char c)
{
	char tmp = c;
	cdcdf_acm_write(&tmp, 1);
}

void usbUARTputString(char* str)
{
	cdcdf_acm_write(str, strlen(str));
	delay_us(1000);
}

char usbUARTread(void)
{
	char c;

	if (!usb_Rx_Buffer_Size)
		return -1;
	
	c = usb_Rx_Buffer[usb_Rx_Buffer_First++];
	usb_Rx_Buffer_First &= USB_RX_BUFFER_SIZE - 1;
	usb_Rx_Buffer_Size--;
	
	return c;
}

uint8_t usbUARTgetString(char *s)
{
	uint8_t len = 0;

	while(usb_Rx_Buffer_Size)
		s[len++] = usbUARTread();
	s[len] = '\0';
	
	return len;
}

void usbUARTsendHex(uint8_t hex)
{
	uint8_t hi = hex >> 4;
	uint8_t lo = (hex & 0x0f);
	
	usbUARTwrite(hi < 10 ? hi + '0' : hi + 'A' - 10);
	usbUARTwrite(lo < 10 ? lo + '0' : lo + 'A' - 10);
}

void usbUARTflush(void)
{
	usb_Rx_Buffer_Size = 0;
	usb_Rx_Buffer_First = 0;
	usb_Rx_Buffer_Last = 0;
}

void usbUARTenqueue(char c)
{
	usb_Rx_Buffer[usb_Rx_Buffer_Last++] = c;
	usb_Rx_Buffer_Last &= USB_RX_BUFFER_SIZE - 1;
	if (usb_Rx_Buffer_Size < USB_RX_BUFFER_SIZE)
		usb_Rx_Buffer_Size++;
}