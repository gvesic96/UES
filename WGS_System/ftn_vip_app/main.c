/*PROJEKAT IZ PREDMETA UMREZENI EMBEDED SISTEMI - CUVAR LUBENICA - WATERMELON GUARD SYSTEM*/
/*GRIGORIJE VESIC e116-2023*/

#include <atmel_start.h>
#include <stdlib.h>
#include "ftn_vip_lib/usbUART.h"
#include "ftn_vip_lib/debugUART.h"
#include "ftn_vip_lib/nbiotUART.h"
#include "ftn_vip_lib/timer_1ms.h"
#include "ftn_vip_lib/Quectel_BC68.h"

/*OVDE SE NALAZI FUNKCIJA ZA ULAZAK U SLEEP MODE*/
#include "hal_sleep.h"





//on-board sensors:
#include "ftn_vip_lib/SHTC3.h"
#include "ftn_vip_lib/bmp280.h"
#include "ftn_vip_lib/BH1750FVI.h"
#include "ftn_vip_lib/LIS2DE12.h"


static struct calendar_alarm alarm1;

typedef struct  
{
	//SHTC3
	uint32_t shtc3_hum;
	uint32_t shtc3_temp;
	//BMP280
	int32_t bmp280_temp;
	int32_t bmp280_pres;
	//BH1750FVI
	uint32_t lum;
}sensor_data;

//parametri servera

//MORA BUILD SOLUTION

/*#define ServerIP	"199.247.17.15"*/
#define ServerIP	"45.76.87.164"
#define ServerPort	50044

#define ALARM_S = 0;
#define ALARM_M = 0;
#define ALARM_H = 1;

/*
	Callback for alarm
*/
static void alarm_cb(struct calendar_descriptor *const descr)
{

	struct calendar_date_time datetime;
	calendar_get_date_time(&CALENDAR, &datetime);
	
	char *alarm_message = malloc(32);
	sprintf(alarm_message, "ALARM AT %d-%d-%d %02d:%02d:%02d\r\n", datetime.date.year, datetime.date.month, datetime.date.day, datetime.time.hour, datetime.time.min, datetime.time.sec);
	
	usbUARTputString(alarm_message);

}




/*funkcija za ocitavanje podataka sa integrisanih senzora*/
void getSensorData(sensor_data *sd)
{
	char str[64];
	//SHTC3
	SHTC3_update();
	sd -> shtc3_hum = SHTC3_raw2Percent() * 100;
	sd -> shtc3_temp = SHTC3_raw2DegC() * 100;
	sprintf(str, "SHTC3 ->\tT = %d.%d C\tH = %d.%d \%\r\n", sd -> shtc3_temp / 100, sd -> shtc3_temp % 100, sd -> shtc3_hum / 100, sd -> shtc3_hum % 100);
	usbUARTputString(str);
		
	//BMP280
	bmp280_measure(&(sd -> bmp280_temp), &(sd -> bmp280_pres));
	sprintf(str, "BMP280 ->\tT = %d.%d C\tP = %d.%d mBar\r\n", sd -> bmp280_temp / 100, sd -> bmp280_temp % 100, sd -> bmp280_pres / 100, sd -> bmp280_pres % 100);
	usbUARTputString(str);
		
	//BH1750FVI
	sd -> lum = BH1750FVI_GetLightIntensity();
	sprintf(str, "BH1750FVI ->\tL = %ld lux\r\n\r\n", sd -> lum);
	usbUARTputString(str);
}


void getAccelData(accel_3axis *accel)
{
	char str[32];
	LIS2DE12_getAccel(accel);

	sprintf(str, "%d,%d,%d\r\n", accel -> accel_X, accel -> accel_Y, accel -> accel_Z);
	usbUARTputString(str);
		
	delay(5);
}

//funkcija za slanje stringa serveru na pritisak tastera
void echoTest(uint8_t protocol, char *payload)
{
	char str[128], response[128];

	sprintf(str, "Pritisni taster za slanje...\r\n");
	usbUARTputString(str);
	while (gpio_get_pin_level(BUTTON));

	char socket = BC68_openSocket(1, protocol);
	int16_t rxBytes = BC68_tx(protocol, ServerIP, ServerPort, payload, strlen(payload), socket);
	BC68_rx(response, rxBytes, socket);
	sprintf(str, "Odgovor servera -> %s\r\n", response);
	usbUARTputString(str);
	BC68_closeSocket(socket);
}

int main(void)
{
	//Inicijalizacija
	atmel_start_init();//prva funkcija koja se poziva//OTVORITI ATMEL_START da vidimo sta je tu se podesava sve oko kontrolera
	debugUARTdriverInit();
	usbUARTinit();
	nbiotUARTinit();
	timer_1ms_init();

	
	//kreira dva niza karaktera
	char str[256], payload[64];
	delay(3000);
	sprintf(str, "--- FTN-VIP NB-IoT ---\r\n");
	usbUARTputString(str);//posalje na serijsku komunikaciju
	setLEDfreq(FREQ_1HZ);
	enableLED();

	//init sensors
	SHTC3_begin();
	bmp280_init();
	BH1750FVI_begin();
	LIS2DE12_init();
	
	//NB-IoT connect
	BC68_debugEnable(true, DEBUG_USB);
	BC68_connect();
	//bilo zakomentarisano kada nam nije trebalo konektovanje na mrezu

	setLEDfreq(FREQ_1HZ);
	sensor_data sd;
	accel_3axis ad;
	
	//getSensorData(&sd);
	
	//set up calendar
	struct calendar_date date;
	struct calendar_time time;
	
	calendar_enable(&CALENDAR);//ovo bi mozda moglo na kraju while petlje
	
	date.year = 2024;
	date.month = 3;
	date.day = 17;
	
	time.hour = 0;
	time.min = 0;
	time.sec = 0;
	
	calendar_set_date(&CALENDAR, &date);
	calendar_set_time(&CALENDAR, &time);//u momentu kada se podesi pocinje da broji?
	
	alarm1.cal_alarm.datetime.time.sec = 0;
	alarm1.cal_alarm.datetime.time.min = 1;
	//alarm1.cal_alarm.datetime.time.hour = 0;
	//alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_SEC;
	alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_MIN;//MATCH_MIN ce uporedjivati i minute i sekunde
	
	//alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_HOUR;
	alarm1.cal_alarm.mode = REPEAT;

	//set alarm
	calendar_set_alarm(&CALENDAR, &alarm1, alarm_cb);
	
	//getSensorData(&sd);
	
	
	while(1)
	{
		getSensorData(&sd);
		//int lumen = (int) sd.lum;
		//int temp = (int) sd.shtc3_temp;
		//check for light and determine if it day or night
		if(sd.lum>10){
			//DAY
			if((sd.shtc3_temp/10)>20){
				//open hatch
				sprintf(str, "DAY: Hatch opened.\r\n");
				usbUARTputString(str);
				//delay(1000);
				}else{
				//close hatch
				sprintf(str, "DAY: Hatch closed.\r\n");
				usbUARTputString(str);
				//delay(1000);
			}
			
			
			}else{
			//NIGHT
			//close hatch
			sprintf(str, "NIGHT: Hatch closed.\r\n");
			usbUARTputString(str);
			if((sd.shtc3_temp/10)<15){
				//turn on heater
				sprintf(str, "NIGHT: Heater turned on.\r\n");
				usbUARTputString(str);
				delay(30000);//wait for 30 seconds after turning on the heater before checking again
				continue;//get into new iteration of while loop skip sending the data
				}else{
				if((sd.shtc3_temp/10)>20){
					//turn off heater
					sprintf(str, "NIGHT: Heater turned off.\r\n");
					usbUARTputString(str);
					
				}
			}
		}
		//send sensor data to server
		sprintf(str, "{\"temp\": {\"value\": %d.%d},\"pres\": {\"value\": %d.%d},\"hum\": {\"value\": %d.%d},\"lum\": {\"value\": %ld}}\r\n", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.bmp280_pres / 100, sd.bmp280_pres % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum);
		usbUARTputString(str);
		delay(5000);
		//echoTest(UDP, str); //sending data to server
	
		gpio_set_pin_level(PA16, true);
		gpio_set_pin_level(PA18, true);
		delay(2000);
		
		gpio_set_pin_level(PA16, false);
		gpio_set_pin_level(PA18, false);
		delay(2000);
		
		
		sprintf(str, "Going to sleep: BACKUP\r\n");
		usbUARTputString(str);
		sleep(0x05);
		
		
	}
}

