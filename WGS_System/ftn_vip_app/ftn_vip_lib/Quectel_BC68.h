/*
 * Quectel_BC68.h
 *
 * Created: 11-Jan-18 10:47:51 PM
 *  Author: Milan Lukic
 */ 

#ifndef QUECTEL_BC68_H_
#define QUECTEL_BC68_H_

#include <atmel_start.h>

#define UDP	17
#define TCP	6

typedef enum {DEBUG_USB, DEBUG_UART} BC68_DEBUG_INTERFACE;

void BC68_reset(void);
bool BC68_connect(void);
char BC68_openSocket(uint16_t listening_port, uint8_t protocol);
bool BC68_closeSocket(char socket);
bool BC68_timeSync(void);
bool BC68_getIPaddr(char *IP_addr);
bool BC68_getIMSI(char *IMSI);
void BC68_debugEnable(bool enabled, BC68_DEBUG_INTERFACE interface);
bool BC68_getConnectionStatus(void);

// UDP related functions
int16_t BC68_tx(uint8_t protocol, char SERVER_IP[], uint16_t sending_port, uint8_t* payload, uint16_t length, char socket);
bool BC68_rx(char* msg, int16_t rx_bytes, char socket);

#endif /* QUECTEL_BC68_H_ */