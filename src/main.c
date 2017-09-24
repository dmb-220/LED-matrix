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
//radijas
#include "rda5807.h"
//WIFI
#include "usart.h"
//#include "esp8266.h"


//Kintamieji
int row=0, gett;
//DS3231
uint8_t D, d, M, y, h, m, s;
//ADC
uint16_t key;
//HTU21D
float  tempu, humu;

//meniu kintamieji
unsigned char meniu=0, set_meniu=0, set_laikas=0;
unsigned char pra=20, pab=0, pozicija=0;
//uzdelsimo kintamieji
unsigned long readMillis, previousMillis;

char * const time_meniu[10] = {"Laikas","Data","Stylius","Laiko zona",
"Efektai","Meniu 1","Meniu 2","Meniu 3",
"Meniu 4","Meniu 5"};


//Sukuriamas SysTick laikmatis
MilliTimer LED, DS3231, HTU21D;

//Sukuriam pertraukimo apdorojimo paprogramę
void TIM3_IRQHandler(){
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	//Isvedam duomenis i LED matrix
	atnaujinti(row);
	if(row==15){row=0;}else{row++;
		}
}

/*void USART1_IRQHandler(void){
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char c = USART_ReceiveData(USART1);
        usart_string_append(c);
        if (c == '\n') {line_ready = 1;}
    }
}*/


//nustatomas meniu ka rodyti
void meniu_set(){
	menu=meniu;
	if(set_meniu==0){
	if(button!=btn){
      if(btn==2){if(meniu<4){meniu++; btn=0;}}}
	if(button!=btn){
		if(btn==5){if(meniu>1){meniu--; btn=0;}}}
		}else{
	if(button!=btn){
		if(btn==1){
			if(set_meniu>1){set_meniu=1; btn=0;}else{
				set_meniu=0; btn=0;}}}
		}
	if(meniu!=menu){set_meniu=0;}
}


int main(void){
	//Paleidziamas SysTick laikmatis
	init_systick_utils();
	LED = new_milli_timer();
	DS3231 = new_milli_timer();
	HTU21D = new_milli_timer();
	//UART
	USART1_Init();
	//ADC
	ADC_Configuration();
	//Paleidziama LED MATRIX
	matrix_gpio_init();
	matrix_timer_init();
	//Paleidziamas DS3231
	ds1307_init();
	// Set initial date and time
	//ds1307_set_calendar_date(DS1307_MONDAY, 25, 04, 17);
	//ds1307_set_time_24(21, 32, 00);



	while (1) {
		//Nuskaitomas HTU21D
		if (mt_poll(&HTU21D, 3000)) {
	    	tempu = htu21d_temp();
	    	humu = htu21d_hum();
		}

		//Nuskaitomas DS3231
		if (mt_poll(&DS3231, 300)) {
			// Get date and time
			ds1307_get_calendar_date(&D, &d, &M, &y);
			ds1307_get_time_24(&h, &m, &s);
		}

		//Piesiama ant LED matrix
		if (mt_poll(&LED, 100)) {
			key = readADC1(ADC_Channel_0);
			meniu_set();
			isvalyti();
			//jei niekas neatitinka, vykdom sita
			if(meniu==BOOT) {
				set_laikas++;
				if(set_laikas<=20){
			    drawRect(0,0,95,15,1);
			    drawString(4,0,"LED Matrix",2);
			    //drawString(30,18,"v. 1.5",1);
			    drawImage(15, 20, 64, 32, arduino);
			    drawString(12,55,"2016 DMB-220",1);
			        }
			    if(set_laikas>=25){
			    	set_laikas=0; meniu=LAIKRODIS;}
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
					drawRect(29,8,31,10,1);
					drawRect(29,17,31,19,1);
					if(s%2){
						drawRect(63,5,65,7,1); drawRect(63,20,65,22,1);}else{
							drawRect(63,8,65,10,1); drawRect(63,17,65,19,1);}

					//Rodoma DATA
					drawNumber(58,35,2000+y,2,4);
					drawNumber(57,47,M,2,2);
					drawString(72,47,".",2);
					drawNumber(78,47,d,2,2);

					drawVLine(55, 34, 30, 1);

					//Rodoma temperatura ir dregme
					char temp2[5];
					ftoa(tempu-6, temp2, 2);
					drawString(2,46,"T",1);
					drawString(6,46,":",1);
					drawString(12,46,temp2,1);
					drawLetterSmall(42,46,127);
					drawString(48,46,"C",1);
					char hum2[5];
					ftoa(humu, hum2, 2);
					drawString(2,55,"RH",1);
					drawString(12,55,":",1);
					drawString(18,55,hum2,1);
					drawString(49,55,"%",1);

				//ijungiamas nustatymu meniu
		         if(button!=btn){
		         if(btn==7){btn=0; set_meniu=2;}}
		    	 break;

		    	 //Nustatymai
				case 2:
					//tikrinam ar vaiksciota po meniu
					if(button!=btn){
						if(btn==5){if(pozicija>0){pozicija--; btn=0;}}
						if(btn==2){if(pozicija<9){pozicija++; btn=0;}}
						if(btn==4){btn=0; set_meniu=pozicija+10;}
					}

					drawString(4,-1,"Nustatymai",2);
					drawRect(0,0,95,16,1);
					drawString(5,56,"<",1);
					if(pozicija==0){drawImage(33, 20, 32, 31, laikas);}
					if(pozicija==1){drawImage(33, 20, 32, 30, data);}
					    drawString(15,56,time_meniu[pozicija],1);
					    drawString(85,56,">",1);
				break;

				//Nustatymai
				case 10:
					//tikrinam ar vaiksciota po meniu
					if(button!=btn){
						if(btn==5){if(pozicija>0){pozicija--; btn=0;}}
						if(btn==2){if(pozicija<3){pozicija++; btn=0;}}}

					drawString(4,-1,"LAIKAS",2);
					drawRect(0,0,95,14,1);
					//drawString(5,56,"<",1);
					//Rodom laika
					drawNumber(-2,20,h,3,2);
					drawNumber(32,20,m,3,2);
					drawNumber(66,20,s,3,2);
					//Tarpai
					drawRect(29,29,31,31,1);
					drawRect(29,38,31,40,1);
					drawRect(63,29,65,31,1);
					drawRect(63,38,65,40,1);
					//drawString(85,56,">",1);
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
