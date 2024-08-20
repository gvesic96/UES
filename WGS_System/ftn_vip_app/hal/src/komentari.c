//getSensorData(&sd);//read all sensors
		
		
		
		/*
		int lumen = (int) sd.lum;
		int temp = (int) sd.shtc3_temp;	
		//check for light and determine if it day or night
		if(lumen>10){
			//DAY
			if((temp/10)>20){
				//open hatch
				usbUARTputString("DAY: Hatch opened.\r\n");
				delay(1000);
			}else{
				//close hatch
				usbUARTputString("DAY: Hatch closed.\r\n");
				delay(1000);
			}
			
			
		}else{
			//NIGHT
			//close hatch
			usbUARTputString("NIGHT: Hatch closed.\r\n");
			if((sd.shtc3_temp/10)<15){
				//turn on heater
				usbUARTputString("NIGHT: Heater turned on.\r\n");
				delay(30000);//wait for 30 seconds after turning on the heater before checking again
				continue;//get into new iteration of while loop skip sending the data
			}else{
				if((sd.shtc3_temp/10)>20){
					//turn off heater
					usbUARTputString("NIGHT: Heater turned off.\r\n");
					
				}
			}
		}
		//send sensor data to server
		usbUARTputString("TESTING TESTING TESTING \r\n");
		sprintf(str, "{\"temp\": {\"value\": %d.%d},\"pres\": {\"value\": %d.%d},\"hum\": {\"value\": %d.%d},\"lum\": {\"value\": %ld}}\r\n", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.bmp280_pres / 100, sd.bmp280_pres % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum);
		usbUARTputString(str);
		delay(20000);
		//echoTest(UDP, str); //sending data to server
		
		*/
		
		
		
		//set calendar and go to sleep?
		//calendar_set_date(&CALENDAR, &date);
		//calendar_set_time(&CALENDAR, &time);
		//sprintf(str, "Calendar set to: %d-%d-%d %02d:%02d:%02d\r\n", date.year, date.month, date.day, time.hour, time.min, time.sec);
		//usbUARTputString(str);
		//delay(1000);
		//sprintf(str, "Going to sleep for %02d:%02d:%02d\r\n", alarm1.cal_alarm.datetime.time.hour, alarm1.cal_alarm.datetime.time.min, alarm1.cal_alarm.datetime.time.sec);
		//usbUARTputString(str);
		//delay(1000);
		
		//sleep(0x05); //Go to BACKUP sleep mode
		
		
		
		
		
		/*
		sprintf(str, "Unesite tekst poruke -> ");
		usbUARTputString(str);
		while(!usbUARTavailable());
		delay(20);//kasnjenje mu treba dok se iz serijske napuni buffer
		
		usbUARTgetString(payload);
		if(payload[0]==48){
			sprintf(str, "{\"temp\": {\"value\": %d.%d},\"pres\": {\"value\": %d.%d},\"hum\": {\"value\": %d.%d},\"lum\": {\"value\": %ld}}", sd.shtc3_temp / 100, sd.shtc3_temp % 100, sd.bmp280_pres / 100, sd.bmp280_pres % 100, sd.shtc3_hum / 100, sd.shtc3_hum % 100, sd.lum);	
			usbUARTputString(str);
			//echoTest(UDP, str);
			sprintf(str, "USPESNO SLANJE?!?!?\r\n");
			usbUARTputString(str);
			//sleep(0x6);
		}
		
		if(payload[0]==49){
			sleep(0x05);//0x6 je za OFF, 0x5je za BACKUP
			//delay(1000);//mora delay jer izgleda nastavi dalje i ponovo dodje do cekanja poruke sa serijske
		}
		*/
		
		
		
		
		//sprintf(str, "%s\r\n", payload);
		//usbUARTputString(str);
		
		//echoTest(UDP, payload);
		//echoTest(TCP, payload);
		
		
		//getAccelData(&ad);
		//getSensorData(&sd);
		//echoTest(UDP, "UDP test data...");
		//echoTest(TCP, "TCP test data...");
		
		//delay(5000);