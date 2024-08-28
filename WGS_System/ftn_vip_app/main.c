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


//parametri alarma
#define ALARM_S 0;
#define ALARM_M 0;
#define ALARM_H 1;

/*
	Callback for alarm
*/
static void alarm_cb(struct calendar_descriptor *const descr)
{

	struct calendar_date_time datetime;
	calendar_get_date_time(&CALENDAR, &datetime);
	
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


//funkcija za slanje stringa serveru

void echoTest(uint8_t protocol, char *payload)
{
	char str[128], response[128];

	char socket = BC68_openSocket(1, protocol);
	int16_t rxBytes = BC68_tx(protocol, ServerIP, ServerPort, payload, strlen(payload), socket);
	BC68_rx(response, rxBytes, socket);
	BC68_closeSocket(socket);
}



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
	
	calendar_enable(&CALENDAR);
	
	date.year = 2024;
	date.month = 3;
	date.day = 17;
	
	time.hour = 0;
	time.min = 0;
	time.sec = 0;
	
	calendar_set_date(&CALENDAR, &date);
	calendar_set_time(&CALENDAR, &time);
	
	alarm1.cal_alarm.datetime.time.sec = ALARM_S;
	alarm1.cal_alarm.datetime.time.min = ALARM_M;
	alarm1.cal_alarm.datetime.time.hour = ALARM_H;
	
	//alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_SEC;
	//alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_MIN;//MATCH_MIN ce uporedjivati i minute i sekunde
	
	alarm1.cal_alarm.option = CALENDAR_ALARM_MATCH_HOUR;
	
	alarm1.cal_alarm.mode = REPEAT;

	//set alarm
	calendar_set_alarm(&CALENDAR, &alarm1, alarm_cb);
	
	
	
	
	bool send_permit = true;
	bool heater_state = false;
	
	while(1)
	{
		
		getSensorData(&sd);
		delay(500);
		
		
		if(sd.lum>20){
			//DAY
			
			if((sd.shtc3_temp/100)>20){
				open_hatch();
					//otvori klapnu ako je temperatura u toku dana veca od 20C	
				}else{
				close_hatch();
					//zatvori klapnu ako je temperatura u toku dana manja od 20C
				}
				
			if((sd.shtc3_hum/100)>65 && (sd.shtc3_temp/100)<=20){
				open_hatch();
				delay(60000);//sacekaj minut da se provetri (smanji vlaznost)
				close_hatch();//zatvori klapnu
			}
				
		}else{
			//NIGHT
				close_hatch();//nocu drzi zatvorenu klapnu
				
				if((sd.shtc3_temp/100)<15){
					//turn on heater
					gpio_set_pin_level(HEATER, true);
					heater_state=true;
					if(send_permit){
						//send data to server
						sprintf(str, "{\"temp\": {\"value\": %d.%d},\"humi\": {\"value\": %d.%d},\"lum\": {\"value\": %ld},\"heater\": {\"value\": %d}}", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum, heater_state);
						echoTest(UDP, str); //sending data to server
						
						send_permit=false;
					}			
					delay(60000);//sacekaj 60 sekundi pre ponovne provere
					continue;//udji ponovo u novu iteraciju petlje
				}else{
					if((sd.shtc3_temp/100)>20){
						//turn off heater
						gpio_set_pin_level(HEATER, false);
						heater_state=false;
						send_permit=true;
				}
			}
		}
		
		//send sensor data to server
		if(send_permit){
			sprintf(str, "{\"temp\": {\"value\": %d.%d},\"humi\": {\"value\": %d.%d},\"lum\": {\"value\": %ld},\"heater\": {\"value\": %d}}", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum, heater_state);
			echoTest(UDP, str); //sending data to server
			
		}
			
		disableLED();//disabling led before going into sleep
		sleep(0x05);//GOING TO SLEEP IN BACKUP MODE
		
		
		
	}
}

