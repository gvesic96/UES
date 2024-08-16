/*
 * Quectel_BC68.c
 *
 *  Author: Milan Lukic
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Quectel_BC68.h"
#include "timer_1ms.h"
#include "debugUART.h"
#include "nbiotUART.h"
#include "usbUART.h"

bool DEBUG_ENABLED = true;
BC68_DEBUG_INTERFACE DEBUG_INTERFACE = DEBUG_USB;
char IMSI[20] = "";

//function prototypes:
bool getBC68response(uint8_t* command, uint8_t* exp_response, uint8_t* response, uint32_t timeout);

void printDebugString(char debug_str[])
{
	if (DEBUG_ENABLED)
	{
		if (DEBUG_INTERFACE == DEBUG_USB)
			usbUARTputString(debug_str);
		else
			debugUARTputString(debug_str);
	}
}

void printDebugChar(char c)
{
	if (DEBUG_ENABLED)
	{
		if (DEBUG_INTERFACE == DEBUG_USB)
		{
			char str[8];
			sprintf(str, "%c", c);
			usbUARTputString(str);
		}
		else
			debugUARTputChar(c);
	}
}

void BC68_reset(void)
{
	char str[32];
	char response[2] = {'\0', '\0'};
	
	sprintf(str, "BC68 reset... ");
	printDebugString(str);
	gpio_set_pin_level(NBIOT_RST, true);
	delay(1000);
	gpio_set_pin_level(NBIOT_RST, false);
	delay(1000);
	sprintf(str, "OK!\r\n");
	printDebugString(str);
	
	uint32_t t0 = millis();
	while(millis() - t0 < 20000)
	{	
		if (nbiotUARTavailable())
		{
			response[0] = response[1];
			response[1] = nbiotUARTgetChar();
			printDebugChar(response[1]);
		}
		
		if (response[0] == 'O' && response[1] == 'K')		
			break;
	}
	delay(100);
	while (nbiotUARTavailable())
	{
		char c = nbiotUARTgetChar();
		printDebugChar(c);
	}
	//nbiotUARTflush();
}

bool BC68_nwkRegister(void)
{
	char response[1024];
	
	setLEDfreq(FREQ_10HZ);
	enableLED();
	nbiotUARTflush();
	BC68_reset();
	
	//	check FW version
	if (!getBC68response("ATI\r\n", "OK", response, 1000))
		return false;
	
	if (!getBC68response("AT+NCONFIG=AUTOCONNECT,FALSE\r\n", "OK", response, 20000))
		return false;

	//	turn off full module functionality
	if (!getBC68response("AT+CFUN=0\r\n", "OK", response, 20000))
		return false;

	//scan band 20 only
	if (!getBC68response("AT+NBAND=20\r\n", "OK", response, 1000))
		return false;

	//	turn on full module functionality
	if (!getBC68response("AT+CFUN=1\r\n", "OK", response, 20000))
		return false;
	
	//	check IMSI
	char IMSI[20];
	BC68_getIMSI(IMSI);
	
	//	error reporting
	if (!getBC68response("AT+CMEE=1\r\n", "OK", response, 10000))
		return false;
	
	//	automatically report network registration status
	if (!getBC68response("AT+CEREG=1\r\n", "OK", response, 3000))
		return false;
	
	//signaling connection status
	if (!getBC68response("AT+CSCON=1\r\n", "OK", response, 3000))
		return false;
	
	//set APN
	if (!getBC68response("AT+CGDCONT=0,\"IP\",\"iot\"\r\n", "OK", response, 3000))
		return false;
	
	//connect to VIP
	if (!getBC68response("AT+COPS=1,2,\"22005\"\r\n", "+CEREG:1", response, 30000))
	//if (!getBC68response("AT+COPS=0\r\n", "+CEREG:1", response, 120000))
		return false;
	
	//polling the network registration status
	bool reg_ok;
	uint8_t attempt_cnt = 0;
	do
	{
		reg_ok = getBC68response("AT+CGATT?\r\n", "+CGATT:1", response, 5000);
		if (!reg_ok)
		{
			if (++attempt_cnt == 30)
			return false;
		}
	} while (!reg_ok);
	
	//get signal strength
	getBC68response("AT+CSQ\r\n", "OK", response, 3000);

	
	//Disable unsolicited result code
	getBC68response("AT+CSCON=0\r\n", "OK", response, 3000);
	
	//Gagijeve kucne carolije:
	getBC68response("AT+CEDRXS=3\r\n", "OK", response, 3000);	//disable eDRX
	getBC68response("AT+CEREG=1\r\n", "OK", response, 3000);
	getBC68response("AT+NPSMR=1\r\n", "OK", response, 3000);
	
	//disable HUAWEI	getBC68response("AT+QREGSWT=2\r\n", "OK", response, 3000);
	
	//T3412 = 30h (ovo resava odlazak u PSM)
	getBC68response("AT+CPSMS=1,,,01000011,10000000\r\n", "OK", response, 3000);

	getBC68response("AT+CPSMS?\r\n", "OK", response, 3000);

	return true;
}

bool BC68_connect(void)
{
	char str[32];
	bool connect_ok;
	
	setLEDfreq(FREQ_10HZ);
	uint16_t attempts = 0;
	do
	{
		connect_ok = BC68_nwkRegister();
		attempts++;
		if (!connect_ok)
		{
			sprintf(str, "Attempt %d failed. Retry...\r\n", attempts);
			printDebugString(str);
		}
	}
	while (!connect_ok);
	
	sprintf(str, "Connect OK!\r\n");
	printDebugString(str);
	
	setLEDfreq(FREQ_1HZ);
	return true;
}

char BC68_openSocket(uint16_t listening_port, uint8_t protocol)
{
 	char cmd[64], response[64];
 	char socket;
	 
	getBC68response("AT+CSQ\r\n", "OK", response, 3000);
	
	if (protocol == UDP)
		sprintf(cmd, "AT+NSOCR=DGRAM,17,%d,1\r\n", listening_port);
	else
		sprintf(cmd, "AT+NSOCR=STREAM,6,%d,1\r\n", listening_port);
	
 	if (!getBC68response(cmd, "OK", response, 3000))
 	{
 		if (!getBC68response(cmd, "OK", response, 3000))
 			return -1;
	}
	uint8_t i = 0;
	uint8_t len = strlen(response);
	
	while(!(response[i] >= '0' && response[i] < '7') && (i < len))
		i++;
		
	if (i==len)
		return -1;
	
	socket = response[i];
	if (!(socket >= '0' && socket < '7'))
		return -1;
	
	sprintf(cmd, "Socket #%c\r\n", socket);
	printDebugString(cmd);

	return socket;
}

bool BC68_closeSocket(char socket)
{
	char response[32], cmd[32];
	
	sprintf(cmd, "AT+NSOCL=%c\r\n", socket);

	return getBC68response(cmd, "OK", response, 1000);
}

bool BC68_getConnectionStatus(void)
{
	char response[64];
	return getBC68response("AT+CGATT?\r\n", "+CGATT:1", response, 5000);
}

// UDP related functions
int16_t BC68_tx(uint8_t protocol, char SERVER_IP[], uint16_t sending_port, uint8_t* payload, uint16_t length, char socket)
{
	char cmd[128], response[128], msg2hex[3], nsonmi[24];
	uint8_t msg_index = 0, hi, lo;

	if (protocol == UDP)
		sprintf(cmd, "AT+NSOST=%c,%s,%d,%d,", socket, SERVER_IP, sending_port, length);
	else
	{
		//connect to the server
		sprintf(cmd, "AT+NSOCO=1,%s,%d\r\n", SERVER_IP, sending_port);
		getBC68response(cmd, "OK", response, 5000);
		sprintf(cmd, "AT+NSOSD=1,%d,", length);
	}
	
	printDebugString(cmd);
	nbiotUARTputString(cmd);
	msg2hex[2] = '\0';
	for (int i = 0; i < length; i++)
	{
		hi = payload[i] >> 4;
		lo = payload[i] & 0x0f;
		
		msg2hex[0] = hi > 9 ? hi + 'A' - 10 : hi + '0';
		msg2hex[1] = lo > 9 ? lo + 'A' - 10 : lo + '0';
		
		printDebugString(msg2hex);
		nbiotUARTputString(msg2hex);
		if ((i % 128) == 0)
			delay(200);
	}
	sprintf(cmd, "\r\n");
	printDebugString(cmd);
	
	if (!getBC68response(cmd, "OK", response, 30000))
		return -1;	//tx error
	
	sprintf(nsonmi, "+NSONMI:%c,", socket);
	msg_index = 0;
	uint32_t t0 = millis();
	while((millis() - t0 < 30000) && !strstr(response, nsonmi))
	{
		if (nbiotUARTavailable())
		{
			response[msg_index] = nbiotUARTgetChar();
			printDebugChar(response[msg_index]);
			response[++msg_index] = '\0';
		}
	}
	delay(200);
	
	if (!strstr(response, nsonmi))
	{
		nbiotUARTflush();
		return -1;
	}
	else
	{
		msg_index = 0;
		while (nbiotUARTavailable())
		{
			response[msg_index] = nbiotUARTgetChar();
			printDebugChar(response[msg_index]);
			response[++msg_index] = '\0';
		}
		int rx_bytes = atoi(response);
		sprintf(cmd, "\r\n# of rx bytes = %d\r\n", rx_bytes);
		printDebugString(cmd);

		return rx_bytes;	//return number of received bytes
	}
}

bool BC68_rx(char* msg, int16_t rx_bytes, char socket)
{
	char cmd[32], response[48], str[32];
	uint8_t i;
	
	getBC68response("AT+CSCON=0\r\n", "OK", response, 3000); //don't signal connection status anymore
	getBC68response("AT+CEREG=0\r\n", "OK", response, 3000); //don't signal registration status anymore
	
	if (rx_bytes <= 0)
		rx_bytes = 256;
	
	sprintf(cmd, "AT+NSORF=%c,%d\r\n", socket, rx_bytes);
	
	sprintf(str, "NB-IoT CMD -> ");
	printDebugString(str);
	printDebugString(cmd);
	nbiotUARTputString(cmd);
	
	uint32_t t0 = millis();
	uint16_t cnt = 0;
	char c, tmp;
	bool resp_OK = false, even_odd = true;
	//response data is after the 4th comma
	while ((millis() - t0) < 5000)
	{
		if (nbiotUARTavailable())
		{
			c = nbiotUARTgetChar();
			printDebugChar(c);
			if (c == ',')
			if (++cnt == 4)
			break;
		}
	}
	if (cnt != 4)
	return false;
	
	cnt = 0;
	while (((millis() - t0) < 4000) && (cnt < rx_bytes))
	{
		//wdt_reset();
		
		if (nbiotUARTavailable())
		{
			c = nbiotUARTgetChar();
			printDebugChar(c);
			//decode hex
			if (c >= 'A')
			c = c - 'A' + 10;
			else
			c -= '0';
			
			if (even_odd)
			{
				tmp = c << 4;
				even_odd = false;
			}
			else
			{
				tmp |= c;
				even_odd = true;
				msg[cnt++] = tmp;
			}
		}
	}
	msg[cnt] = '\0';
	
	delay(100);
	while (nbiotUARTavailable())
	{
		char c = nbiotUARTgetChar();
		printDebugChar(c);
	}
	
	return true;
}


bool BC68_getIPaddr(char *IP_addr)
{
	char *tmp, response[64];
	uint8_t i = 0;
	
	nbiotUARTflush();
	
	if (!getBC68response("AT+CGPADDR=0\r\n", "OK", response, 3000))
		return false;
		
	tmp = strstr(response, "+CGPADDR:0,");
	if (!tmp)
		return false;
		
	tmp += 11;	//strlen("+CGPADDR:0,")
	int8_t len = strlen(tmp);
	while ((*tmp != '\r') && (i < len))
	{
		IP_addr[i++] = *tmp++;
	}
	
	if (i == len)
	{
		IP_addr[--i] = '\0';
		return false;
	}
	
	IP_addr[i] = '\0';
	
	sprintf(response, "IP address: %s\r\n", IP_addr);
	printDebugString(response);
		
	return true;
}

bool BC68_getIMSI(char *IMSI)
{
	char response[64];
	uint8_t i = 0;

	getBC68response("AT+CIMI\r\n", "OK", response, 2000);

	for (i = 0; i < 15; i++)
		IMSI[i] = response[2 + i];
	IMSI[i] = '\0';

	sprintf(response, "IMSI: %s\r\n", IMSI);
	printDebugString(response);

	return true;
}

bool getBC68response(uint8_t* command, uint8_t* exp_response, uint8_t* response, uint32_t timeout)
{
	char str[32];
	uint16_t count = 0;
	bool resp_OK = false;

	response[0] = '\0';
	sprintf(str, "NB-IoT CMD -> ");
	printDebugString(str);
	for (char *tmp = command; *tmp != '\0'; tmp++)
	{
		printDebugChar(*tmp);
		nbiotUARTputChar(*tmp);
		delay(5);
	}
	//nbiotUARTputString(command);
	
	uint32_t t0 = millis();
	while ((millis() - t0) < timeout)
	{
		if (nbiotUARTavailable())
		{
			response[count] = nbiotUARTgetChar();
			printDebugChar(response[count]);
			response[++count] = '\0';
		}
		if (strstr(response, exp_response))
		{
			resp_OK = true;
			break;
		}
	}

	delay(10);
	while (nbiotUARTavailable())
	{
		response[count] = nbiotUARTgetChar();
		printDebugChar(response[count]);
		response[++count] = '\0';
	}

	nbiotUARTflush();
	return resp_OK;
}

bool BC68_timeSync(void)
{
	char *tmp, response[32];

	struct calendar_date_time cdt;
	calendar_enable(&CALENDAR);
	
	getBC68response("AT+CCLK?\r\n", "OK", response, 2000);
	
	tmp = strstr(response, "+CCLK:");
	if (!tmp)
		return false;
	
	tmp += 6; //strlen("+CCLK:")
	cdt.date.year = 2000 + atoi(tmp);
	tmp += 3;
	cdt.date.month = atoi(tmp);
	tmp += 3;
	cdt.date.day = atoi(tmp);
	tmp += 3;
	cdt.time.hour = atoi(tmp);
	tmp += 3;
	cdt.time.min = atoi(tmp);
	tmp += 3;
	cdt.time.sec = atoi(tmp);

	//dow expects months to be 1-12, and days 1-31
	tmp += 2;
	int32_t TS = 0;	//atoi(tmp) * 15 * 60;	//time zone adjustment
	
	sprintf(response, "Unix TS = %ld\r\n", calendar_date_to_timestamp(&cdt));
	printDebugString(response);

	calendar_set_date(&CALENDAR, &cdt.date);
	calendar_set_time(&CALENDAR, &cdt.time);
	
	delay(100);
}

void BC68_debugEnable(bool enabled, BC68_DEBUG_INTERFACE interface)
{
	DEBUG_ENABLED = enabled;
	DEBUG_INTERFACE = interface;
}