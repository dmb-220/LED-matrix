//###############################################################################
//## Autorius: DMB-220                                                         ##
//## Projektas: LED MATRIX										               ##
//## Data: 2016-11-20                                                          ##
//## Versija: v 0.5                                                            ##
//###############################################################################

//BIBLIOTEKU ikelimas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//Standartines
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_adc.h"
#include <stm32f10x_exti.h>
#include <stm32f10x_tim.h>
#include <misc.h>
//Mano parasytos
#include "main.h"
#include "matrix.h"
//Laikmatis SysTick
#include "systick-utils.h"
#include "milli-timer.h"
//RTC DS3132
#include "ds1307.h"
//HTU21D
#include "htu21d.h"

//Sukuriamas SysTick laikmatis
MilliTimer led_metrix, ds3231, htu21d, adc_key;

//Sukuriam pertraukimo apdorojimo paprogramę
void TIM3_IRQHandler(){
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	//Isvedam duomenis i LED matrix
	atnaujinti(row);
	if(row==15){row=0;}else{row++;}
}

//nustatomas meniu ka rodyti
void meniu_set(){
	menu=meniu;
	if(set_meniu == 0){
		if(button != btn){
		  if(btn == DESINE){if(meniu < 3){meniu++; btn = NO_KEY;}}
		  if(btn == KAIRE){if(meniu > 1){meniu--; btn = NO_KEY;}}
		}
	}
	//if(meniu != menu){set_meniu=0;}
}


int main(void){
	//Paleidziamas SysTick laikmatis
	init_systick_utils();
	led_metrix = new_milli_timer();
	ds3231 = new_milli_timer();
	htu21d = new_milli_timer();
	adc_key = new_milli_timer();
	//ADC
	ADC_Configuration();
	//Paleidziama LED MATRIX
	matrix_gpio_init();
	matrix_timer_init();
	//Paleidziamas DS3231
	ds1307_init();
	// Set initial date and time
	//ds1307_set_calendar_date(DS1307_SATURDAY, 30, 10, 16);
	//ds1307_set_time_24(17, 40, 00);


	while (1) {
		//Nuskaitomas HTU21D
		if (mt_poll(&htu21d, 3000)) {
	    	tempu = htu21d_temp();
	    	humu = htu21d_hum();}
		//Nuskaitomas DS3231
		if (mt_poll(&ds3231, 1000)) {
			// Get date and time
			ds1307_get_calendar_date(&D, &d, &M, &y);
			ds1307_get_time_24(&h, &m, &s);}

		//Nuskaitomas ADC KEY
		if (mt_poll(&adc_key, 100)) {
			key = readADC1(ADC_Channel_0);}

		//Piesiama ant LED matrix
		if (mt_poll(&led_metrix, 500)) {
			meniu_set();
			isvalyti();

			//jei niekas neatitinka, vykdom sita
			if(meniu == BOOT) {
				set_laikas++;
				if(set_laikas<=20){
					drawRect(0,0,95,15,1);
					drawString(4,0,"LED Matrix",2);
					//drawString(30,18,"v. 1.5",1);
					drawImage(15, 20, 64, 32, arduino);
					drawString(12,55,"2016 DMB-220",1);
				}
			    if(set_laikas >= 25){set_laikas = 0; meniu = LAIKRODIS;}
			    if(button != btn){
			    	if(btn == OK){btn = NO_KEY; meniu = LAIKRODIS; set_laikas = 0;}}
			}

			////////////////////////////////Rodom laika, data ir temperatura su dregme////////////////////////////////////
			if(meniu==LAIKRODIS) {
				switch (set_meniu) {
				//Rodomas laikas
				case 1:
					//Rodom laika
					drawNumber(-2,-1,h,3,2);
					drawNumber(32,-1,m,3,2);
					drawNumber(66,-1,s,3,2);
					//Tarpai
					if(s%2){
						drawRect(29,8,31,10,1); drawRect(29,17,31,19,1);
					}else{
						drawRect(29,5,31,7,1); drawRect(29,20,31,22,1);
						}

					if(s%2){
						drawRect(63,5,65,7,1); drawRect(63,20,65,22,1);
					}else{
						drawRect(63,8,65,10,1); drawRect(63,17,65,19,1);}

					//Rodoma DATA
					drawNumber(58,28,2000+y,2,4);
					drawNumber(57,40,M,2,2);
					drawString(72,40,".",2);
					drawNumber(78,40,d,2,2);

					drawVLine(55, 30, 24, 1);

					//Rodoma temperatura ir dregme
					char temp2[5];
					ftoa(tempu-6, temp2, 2);
					drawString(2,31,"T",1);
					drawString(6,31,":",1);
					drawString(12,31,temp2,1);
					drawLetterSmall(42,31,127);
					drawString(48,31,"C",1);
					char hum2[5];
					ftoa(humu, hum2, 2);
					drawString(2,40,"RH",1);
					drawString(12,40,":",1);
					drawString(18,40,hum2,1);
					drawString(49,40,"%",1);

					//Rodom savates diena
					drawString(1,55,"D",1);
					drawString(5,55,":",1);
					drawString(12,55,dienos[D-1],1);

					//ijungiamas nustatymu meniu
					 if(button != btn){
						 if(btn == MENIU){btn = NO_KEY; set_meniu = 2; pozicija = 1;}
						 if(btn == ATGAL){set_meniu = 0; btn = NO_KEY;}
					 }
		    	 break;

		    	 //Nustatymai
				case 2:
					drawString(4,-1,"Nustatymai",2);
					drawRect(0,0,95,16,1);
					//pasirinkimas

					drawRect(3,(pozicija*10)+13,92,(pozicija*10)+23,1);

					for(int i = 0; i < 4; i++){
						int ii = (i+1)*10;
						drawString(5,15+ii,time_meniu[vieta][i],1);
					}
					//myktukai
					if(button != btn){
						if(btn == KAIRE){vieta = !vieta; btn = NO_KEY;}
						if(btn == DESINE){vieta = !vieta; btn = NO_KEY;}

						if(btn == AUKSTYN){if(pozicija > 1){pozicija--; btn = NO_KEY;}}
						if(btn == ZEMYN){if(pozicija < 4){pozicija++; btn = NO_KEY;}}

						if(btn == OK){
							if(pozicija == 1 && vieta == 0){
								btn = NO_KEY; set_meniu = 10;
								sh = h, sm = m, ss = s; pozicija = 1;
							}
							if(pozicija == 2 && vieta == 0){
								btn = NO_KEY; set_meniu = 11;
								sd = d, sM = M, sy = y; pozicija = 1;
							}
							if(pozicija == 3 && vieta == 0){
								btn = NO_KEY; set_meniu = 14;
								sD = D; pozicija = 1;
							}
						}
						if(btn == ATGAL){set_meniu = 1; btn = NO_KEY; pozicija = 1;}
					}
				break;

				//issaugo TXT
				case 9:
					set_laikas++;
					if(set_laikas<=10){
						drawString(7,33,"ISSAUGOTA",2);
						drawRect(0,32,95,48,1);
					}else{
							set_meniu = 2;
					}
				break;

				//Nustatymai
				case 10:
					drawString(4,0,"LAIKAS",2);
					drawRect(0,0,95,16,1);

					if(pozicija == 1){
						drawRect(1,19,27,20,1);
						drawRect(1,50,27,51,1);}
					if(pozicija == 2){
						drawRect(33,19,61,20,1);
						drawRect(33,50,61,51,1);}
					if(pozicija ==3){
						drawRect(67,19,95,20,1);
						drawRect(67,50,95,51,1);}

					//Rodom laika
					drawNumber(-2,20,sh,3,2);
					drawNumber(32,20,sm,3,2);
					drawNumber(66,20,ss,3,2);
					//Tarpai
					drawRect(29,29,31,31,1);
					drawRect(29,38,31,40,1);
					drawRect(63,29,65,31,1);
					drawRect(63,38,65,40,1);

					//tikrinam ar vaiksciota po meniu
					if(button!=btn){
						if(btn == AUKSTYN && pozicija == 1){if(sh < 23){sh++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 1){if(sh > 0){sh--; btn = NO_KEY;}}

						if(btn == AUKSTYN && pozicija == 2){if(sm < 59){sm++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 2){if(sm > 0){sm--; btn = NO_KEY;}}

						if(btn == AUKSTYN && pozicija == 3){if(ss < 59){ss++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 3){if(ss > 0){ss--; btn = NO_KEY;}}

						if(btn == OK){btn = NO_KEY; set_meniu = 12; pozicija = 0;}
						if(btn == KAIRE){if(pozicija > 1){pozicija--; btn = NO_KEY;}}
						if(btn == DESINE){if(pozicija < 3){pozicija++; btn = NO_KEY;}}

						if(btn == ATGAL){set_meniu = 2; btn = NO_KEY; pozicija = 1;}}
				break;

				//DATA
				case 11:
					drawString(4,0,"DATA",2);
					drawRect(0,0,95,16,1);

					if(pozicija == 1){
						drawRect(1,19,27,20,1);
						drawRect(1,50,27,51,1);}
					if(pozicija == 2){
						drawRect(33,19,61,20,1);
						drawRect(33,50,61,51,1);}
					if(pozicija ==3){
						drawRect(67,19,95,20,1);
						drawRect(67,50,95,51,1);}

					//Rodom data
					drawNumber(-2,20,sy,3,2);
					drawNumber(32,20,sM,3,2);
					drawNumber(66,20,sd,3,2);
					//Tarpai
					drawRect(29,45,31,47,1);
					drawRect(63,45,65,47,1);


					//tikrinam ar vaiksciota po meniu
					if(button!=btn){
						if(btn == AUKSTYN && pozicija == 1){if(sy < 30){sy++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 1){if(sy > 10){sy--; btn = NO_KEY;}}

						if(btn == AUKSTYN && pozicija == 2){if(sM < 12){sM++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 2){if(sM > 1){sM--; btn = NO_KEY;}}

						if(btn == AUKSTYN && pozicija == 3){if(sd < 31){sd++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 3){if(sd > 1){sd--; btn = NO_KEY;}}

						if(btn == OK){btn = NO_KEY; set_meniu = 13; pozicija = 1;}
						if(btn == KAIRE){if(pozicija > 1){pozicija--; btn = NO_KEY;}}
						if(btn == DESINE){if(pozicija < 3){pozicija++; btn = NO_KEY;}}

						if(btn == ATGAL){set_meniu = 2; btn = NO_KEY; pozicija = 2;}}
				break;

				//issaugom laika
				case 12:
					ds1307_set_seconds(ss);
					ds1307_set_minutes(sm);
					ds1307_set_hours_24(sh);
					//standartinis ISSAUGOTA uzrasas
					set_meniu = 9;
				break;

				//issaugom laika
				case 13:
					ds1307_set_year(sy);
					ds1307_set_month(sM);
					ds1307_set_date(sd);
					//standartinis ISSAUGOTA uzrasas
					set_meniu = 9;
				break;

				//SAVITES DIENA
				case 14:
					drawString(4,0,"DIENA",2);
					drawRect(0,0,95,16,1);

					if(pozicija == 1){
						drawRect(1,19,94,20,1);
						drawRect(1,30,94,31,1);}

					//Rodom savates diena
					drawString(1,22,dienos[sD-1],1);


					//tikrinam ar vaiksciota po meniu
					if(button!=btn){
						if(btn == AUKSTYN && pozicija == 1){if(sD < 7){sD++; btn = NO_KEY;}}
						if(btn == ZEMYN && pozicija == 1){if(sD > 1){sD--; btn = NO_KEY;}}

						if(btn == OK){btn = NO_KEY; set_meniu = 15; pozicija = 2;}
						if(btn == ATGAL){set_meniu = 2; btn = NO_KEY; pozicija = 3;}}
				break;

				//issaugom savaites dienas
				case 15:
					ds1307_set_day(sD);
					//standartinis ISSAUGOTA uzrasas
					set_meniu = 9;
				break;


		    	 //rodomas  pasisveikinimas
		    	 default:
		    		 drawImage(25, 2, 48, 44, laikrodis);
		    		 drawString(10,48,"Laikrodis",2);
		    	 		if(button!=btn){
		    	 			if(btn==4){btn=0; set_meniu=1;}}
				break;
				}
			}

			//////////////////////////////////////////////////////////////Radijas///////////////////////////////////////
			if(meniu==RADIJAS) {
				switch (set_meniu) {
				//Rodomas laikas
				case 1:
					//drawNumber(66,0,s,5,2);
					//drawNumber(10,30,rc5_code,1,3);


				//if(button!=btn){
				//if(btn==7){btn=0; set_meniu=7;}}
				break;

				//rodomas  pasisveikinimas
				default:
				drawImage(27, 2, 48, 40, radijas);
				drawString(15,48,"Radijas",2);
				if(button!=btn){
				if(btn==4){btn=0; set_meniu=1;}}
				//set_meniu=1;
				break;
				}
			}
			/////////////////////////////////////////////////////////////Orai//////////////////////////////////////////////
			if(meniu==ORAI) {
				switch (set_meniu) {
							//Rodomas laikas
							case 1:
								drawString(2,2,"WI-FI",2);
							//if(button!=btn){
							//if(btn==7){btn=0; set_meniu=7;}}
							break;

							//rodomas  pasisveikinimas
							default:
							drawImage(25, 2, 48, 48, orai);
							drawString(30,48,"Orai",2);
							if(button!=btn){
							if(btn==4){btn=0; set_meniu=1;}}
							//set_meniu=1;
							break;
							}
						}

		}

		for(int i=0; i<768; i++){
			Buffer[i]= Buffer_copy[i];
		}

	}
}
