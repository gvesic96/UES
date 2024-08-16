#ifndef SERIAL_COM_H_
#define SERIAL_COM_H_

#include "atmel_start.h"
#include "usb_start.h"

// Size of serial buffer
#define USB_RX_BUFFER_SIZE 256

// Initialize serial communication
void usbUARTinit();
uint8_t usbUARTavailable(void);
void usbUARTwrite(unsigned char c);
void usbUARTputString(char* str);
char usbUARTread(void);
uint8_t usbUARTgetString(char *s);
void usbUARTsendHex(uint8_t hex);
void usbUARTflush(void);
void usbUARTenqueue(char c);

#endif /* SERIAL_COM_H_ */