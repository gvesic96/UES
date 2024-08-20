/*PROJEKAT IZ PREDMETA UMREZENI EMBEDED SISTEMI - CUVAR LUBENICA - WATERMELON GUARD SYSTEM*/
/*GRIGORIJE VESIC e116-2023*/

#include <atmel_start.h>
#include <stdlib.h>
#include "ftn_vip_lib/debugUART.h"
#include "ftn_vip_lib/nbiotUART.h"
#include "ftn_vip_lib/timer_1ms.h"
#include "ftn_vip_lib/Quectel_BC68.h"

#include "hal_sleep.h"



//on-board sensors:
#include "ftn_vip_lib/SHTC3.h"
#include "ftn_vip_lib/BH1750FVI.h"


static struct calendar_alarm alarm1;

typedef struct  
{
	//SHTC3
	uint32_t shtc3_hum;
	uint32_t shtc3_temp;

	//BH1750FVI
	uint32_t lum;
}sensor_data;

//parametri servera
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
	
	//char *alarm_message = malloc(32);
	//sprintf(alarm_message, "ALARM AT %d-%d-%d %02d:%02d:%02d\r\n", datetime.date.year, datetime.date.month, datetime.date.day, datetime.time.hour, datetime.time.min, datetime.time.sec);
	
	//usbUARTputString(alarm_message);

}




/*funkcija za ocitavanje podataka sa integrisanih senzora*/
void getSensorData(sensor_data *sd)
{
	char str[64];
	//SHTC3
	SHTC3_update();
	sd -> shtc3_hum = SHTC3_raw2Percent() * 100;
	sd -> shtc3_temp = SHTC3_raw2DegC() * 100;
		
	//BH1750FVI
	sd -> lum = BH1750FVI_GetLightIntensity();
}


//funkcija za slanje stringa serveru na pritisak tastera

/*void echoTest(uint8_t protocol, char *payload)
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
*/


void open_hatch(void){

	gpio_set_pin_level(SERVO, true);
	delay_us(700);//delay 1ms
	gpio_set_pin_level(SERVO, false);
	delay_us(19300);//delay 19ms
	
	gpio_set_pin_level(SERVO, true);
	delay_us(700);//delay 1ms
	gpio_set_pin_level(SERVO, false);
	delay_us(19300);//delay 20ms
	gpio_set_pin_level(SERVO, true);
	delay_us(700);//delay 1ms
	gpio_set_pin_level(SERVO, false);
	delay_us(19300);//delay 20ms
	
	
}

void close_hatch(void){

	gpio_set_pin_level(SERVO, true);
	delay_us(1850);//delay 2ms
	gpio_set_pin_level(SERVO, false);
	delay_us(18150);//delay 18ms
	
	gpio_set_pin_level(SERVO, true);
	delay_us(1850);//delay 2ms
	gpio_set_pin_level(SERVO, false);
	delay_us(18150);//delay 18ms
	gpio_set_pin_level(SERVO, true);
	delay_us(1850);//delay 2ms
	gpio_set_pin_level(SERVO, false);
	delay_us(18150);//delay 18ms

	
}



int main(void)
{
	//Inicijalizacija
	atmel_start_init();//ZAKOMENTARISAO usb_init da bi radilo preko baterije
	debugUARTdriverInit();
	nbiotUARTinit();
	timer_1ms_init();

	
	//kreira dva niza karaktera
	char str[256], payload[64];
	delay(3000);
	//sprintf(str, "--- FTN-VIP NB-IoT ---\r\n");
	//usbUARTputString(str);//posalje na serijsku komunikaciju
	setLEDfreq(FREQ_1HZ);
	enableLED();

	//init sensors
	SHTC3_begin();
	BH1750FVI_begin();
	
	//NB-IoT connect
	BC68_debugEnable(true, DEBUG_UART);//PROMENIO SA USB_UART NA DEBUG_UART
	BC68_connect();
	
	
	setLEDfreq(FREQ_1HZ);//podesimo LED da sija frekvencijom 1Hz da znamo kada je program pokrenut
	sensor_data sd;
	
	
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
	
	alarm1.cal_alarm.datetime.time.sec = 5;
	//alarm1.cal_alarm.datetime.time.min = 1;
	//alarm1.cal_alarm.datetime.time.hour = 0;
	alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_SEC;
	//alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_MIN;//MATCH_MIN ce uporedjivati i minute i sekunde
	
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
				open_hatch();
				//sprintf(str, "DAY: Hatch opened.\r\n");
				//usbUARTputString(str);
				//delay(1000);
				}else{
				//close hatch
				close_hatch();
				//sprintf(str, "DAY: Hatch closed.\r\n");
				//usbUARTputString(str);
				//delay(1000);
			}
			
			
			}else{
			//NIGHT
			//close hatch
			close_hatch();
			//sprintf(str, "NIGHT: Hatch closed.\r\n");
			//usbUARTputString(str);
			if((sd.shtc3_temp/10)<15){
				//turn on heater
				//sprintf(str, "NIGHT: Heater turned on.\r\n");
				//usbUARTputString(str);
				gpio_set_pin_level(HEATER, true);
				delay(30000);//wait for 30 seconds after turning on the heater before checking again
				continue;//get into new iteration of while loop skip sending the data
			}else{
				if((sd.shtc3_temp/10)>20){
					//turn off heater
					gpio_set_pin_level(HEATER, false);
					//sprintf(str, "NIGHT: Heater turned off.\r\n");
					//usbUARTputString(str);
					
				}
			}
		}
		//send sensor data to server
		//sprintf(str, "{\"temp\": {\"value\": %d.%d},\"pres\": {\"value\": %d.%d},\"hum\": {\"value\": %d.%d},\"lum\": {\"value\": %ld}}\r\n", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.bmp280_pres / 100, sd.bmp280_pres % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum);
		//usbUARTputString(str);
		delay(7000);
		//echoTest(UDP, str); //sending data to server
			
		//sprintf(str, "Going to sleep: BACKUP\r\n");
		//usbUARTputString(str);
		sleep(0x05);
		
		
	}
}

