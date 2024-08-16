/*
 * debugUART.c
 *
 * Created: 26-Aug-19 5:51:00 PM
 *  Author: Milan Lukic
 */ 

#include "debugUART.h"
#include <string.h>

#define DEBUG_USART_RX_BUFFER_SIZE 1024
static struct io_descriptor *debug_io;
static volatile bool debugTxDone = true;

char debugRxBuffer[DEBUG_USART_RX_BUFFER_SIZE];		//prijemni FIFO bafer
volatile unsigned char debugRxBufferSize = 0;	//broj karaktera u prijemnom baferu
volatile unsigned char debugRxBufferFirst = 0;
volatile unsigned char debugRxBufferLast = 0;

static void rx_cb_debugUART(const struct usart_async_descriptor *const io_descr)
{
	char c;
	io_read(debug_io, &c, 1);
	debugRxBuffer[debugRxBufferLast++] = c;			//ucitavanje primljenog karaktera
	debugRxBufferLast &= DEBUG_USART_RX_BUFFER_SIZE - 1;	//povratak na pocetak u slucaju prekoracenja
	if (debugRxBufferSize < DEBUG_USART_RX_BUFFER_SIZE)
		debugRxBufferSize++;						//inkrement brojaca primljenih karaktera
}

static void tx_cb_debugUART(const struct usart_async_descriptor *const io_descr)
{
	debugTxDone = true;
}

void debugUARTdriverInit(void)
{
	usart_async_register_callback(&debugUART, USART_ASYNC_RXC_CB, rx_cb_debugUART);
	usart_async_register_callback(&debugUART, USART_ASYNC_TXC_CB, tx_cb_debugUART);
	usart_async_get_io_descriptor(&debugUART, &debug_io);
	usart_async_enable(&debugUART);
}

uint16_t debugUARTavailable(void)
{
	return debugRxBufferSize;
}

void debugUARTputChar(char c)
{
	debugTxDone = false;
	io_write(debug_io, &c, 1);
	while(!debugTxDone);
}

void debugUARTputSample(uint8_t sample)
{
	char str[16];
	sprintf(str, "%d\r\n", sample);
	debugTxDone = false;
	io_write(debug_io, str, strlen(str));
	while(!debugTxDone);
}

void debugUARTputString(char *str)
{
	debugTxDone = false;
 	io_write(debug_io, str, strlen(str));
	while(!debugTxDone);
}

char debugUARTgetChar(void)
{
	char c;

	if (!debugRxBufferSize)							//bafer je prazan?
		return -1;
	c = debugRxBuffer[debugRxBufferFirst++];				//citanje karaktera iz prijemnog bafera
	debugRxBufferFirst &= DEBUG_USART_RX_BUFFER_SIZE - 1;	//povratak na pocetak u slucaju prekoracenja
	debugRxBufferSize--;								//dekrement brojaca karaktera u prijemnom baferu

	return c;
}

void debugUARTgetString(char *str)
{
	unsigned char len = 0;

	while(debugRxBufferSize) 					//ima karaktera u faferu?
		str[len++] = debugUARTgetChar();	//ucitavanje novog karaktera

	str[len] = 0;							//terminacija stringa
	return len;								//vraca broj ocitanih karaktera
}

void debugUARTsendHex(uint8_t hex)
{
	uint8_t hi = hex >> 4;
	uint8_t lo = (hex & 0x0f);
	
	debugUARTputChar(hi < 10 ? hi + '0' : hi + 'A' - 10);
	debugUARTputChar(lo < 10 ? lo + '0' : lo + 'A' - 10);
}

void debugUARTflush(void)
{
	usart_async_flush_rx_buffer(&debugUART);
	debugRxBufferSize = 0;
}