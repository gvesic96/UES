/*
 * nbiotUART.c
 *
 * Created: 26-Aug-19 5:51:00 PM
 *  Author: Milan Lukic
 */ 

#include "nbiotUART.h"

#define NBIOT_USART_RX_BUFFER_SIZE 1024

static struct io_descriptor *nbiot_io;
static volatile bool nbiotTxDone = true;

char nbiotRxBuffer[NBIOT_USART_RX_BUFFER_SIZE];		//prijemni FIFO bafer
volatile unsigned char nbiotRxBufferSize = 0;	//broj karaktera u prijemnom baferu
volatile unsigned char nbiotRxBufferFirst = 0;
volatile unsigned char nbiotRxBufferLast = 0;

static void rx_cb_nbiotUART(const struct usart_async_descriptor *const io_descr)
{
	char c;
	while (io_read(nbiot_io, &c, 1))
	{
		nbiotRxBuffer[nbiotRxBufferLast++] = c;			//ucitavanje primljenog karaktera
		nbiotRxBufferLast &= NBIOT_USART_RX_BUFFER_SIZE - 1;	//povratak na pocetak u slucaju prekoracenja
		if (nbiotRxBufferSize < NBIOT_USART_RX_BUFFER_SIZE)
			nbiotRxBufferSize++;						//inkrement brojaca primljenih karaktera
	}
}

static void tx_cb_nbiotUART(const struct usart_async_descriptor *const io_descr)
{
	nbiotTxDone = true;
}

void nbiotUARTinit(void)
{
	usart_async_register_callback(&nbiotUART, USART_ASYNC_RXC_CB, rx_cb_nbiotUART);
	usart_async_register_callback(&nbiotUART, USART_ASYNC_TXC_CB, tx_cb_nbiotUART);
	usart_async_get_io_descriptor(&nbiotUART, &nbiot_io);
	usart_async_enable(&nbiotUART);
}

uint16_t nbiotUARTavailable(void)
{
	return nbiotRxBufferSize;		//ocitavanje broja karaktera u prijemnom baferu
}

void nbiotUARTputChar(char c)
{
	nbiotTxDone = false;
	io_write(nbiot_io, &c, 1);
	while (!nbiotTxDone);
}

void nbiotUARTputString(char *str)
{
	nbiotTxDone = false;
 	io_write(nbiot_io, str, strlen(str));
 	while (!nbiotTxDone);
}

char nbiotUARTgetChar(void)
{
	char c;

	if (!nbiotRxBufferSize)							//bafer je prazan?
		return -1;
	c = nbiotRxBuffer[nbiotRxBufferFirst++];				//citanje karaktera iz prijemnog bafera
	nbiotRxBufferFirst &= NBIOT_USART_RX_BUFFER_SIZE - 1;	//povratak na pocetak u slucaju prekoracenja
	nbiotRxBufferSize--;								//dekrement brojaca karaktera u prijemnom baferu

	return c;
}

void nbiotUARTgetString(char *str)
{
	unsigned char len = 0;

	while(nbiotRxBufferSize) 					//ima karaktera u baferu?
		str[len++] = nbiotUARTgetChar();	//ucitavanje novog karaktera

	str[len] = 0;							//terminacija stringa
	return len;								//vraca broj ocitanih karaktera
}

void nbiotUARTsendHex(uint8_t hex)
{
	uint8_t hi = hex >> 4;
	uint8_t lo = (hex & 0x0f);
	
	nbiotUARTputChar(hi < 10 ? hi + '0' : hi + 'A' - 10);
	nbiotUARTputChar(lo < 10 ? lo + '0' : lo + 'A' - 10);
}

void nbiotUARTflush(void)
{
	usart_async_flush_rx_buffer(&nbiotUART);
	nbiotRxBufferSize = nbiotRxBufferFirst = nbiotRxBufferLast = 0;
}