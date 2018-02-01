/*
 * main.h
 *
 *  Created on: 2016-08-22
 *      Author: DMB-220
 */

#ifndef MAIN_H_
#define MAIN_H_

//LED matrix pajungimas
#define latch_pin GPIO_Pin_5   //LE port B
#define clock_pin GPIO_Pin_4   //CLK port B
#define en_pin GPIO_Pin_7       //OE port A
#define la_pin GPIO_Pin_3       //A port A
#define lb_pin GPIO_Pin_4       //B port A
#define lc_pin GPIO_Pin_5       //C port A
#define ld_pin GPIO_Pin_6       //D port A
//DATA pin'ai
#define data1_R1 GPIO_Pin_3     //R1 port B
#define data1_R2 GPIO_Pin_15     //R2 port A
#define data2_R1 GPIO_Pin_0      //R3 port B
#define data2_R2 GPIO_Pin_1      //R4 port B
#define data3_R1 GPIO_Pin_1      //R5 port A
#define data3_R2 GPIO_Pin_2      //R6 port A

//MENIU NUSTATYMAI
#define BOOT       0
#define LAIKRODIS  1
#define RADIJAS    2
#define ORAI       3
#define ZAIDIMAI   4
#define NUSTATYMAI 5
#define APIE 	   6

//MYKTUKAI
#define NO_KEY       0
#define ATGAL        1
#define DESINE       2
#define ZEMYN        3
#define OK       	 4
#define KAIRE        5
#define AUKSTYN      6
#define MENIU        7

//Kintamieji
int row=0, gett;
//DS3231
uint8_t D, d, M, y, h, m, s;
//DS3231 laikui nustatyti
uint8_t sD, sd, sM, sy, sh, sm, ss;
//ADC
uint16_t key;
//myktukai
unsigned char button, menu;
uint16_t btn;
//HTU21D
float  tempu, humu;

//meniu kintamieji
unsigned char meniu=0, set_meniu=0, set_laikas=0;
unsigned char pra=20, pab=0, pozicija=0, vieta = 0;;
//uzdelsimo kintamieji
unsigned long readMillis, previousMillis;

char * const time_meniu[2][4] = {
		{"Laikas", "Data", "Sav. diena", "Laiko zona"},
		{"Stilius", "Efektai", "Kiti", "Atgal"}
};
char * const dienos[7] = {"Pirmadienis", "Antradienis", "Treciadienis", "Ketvirtadienis", "Penktadienis", "Sestadienis", "Sekmadienis"};

//FLOAT konvertavimas
// reverses a string 'str' of length 'len'
void reverse(char *str, int len){
    int i=0, j=len-1, temp;
    while (i<j){
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

int intToStr(int x, char str[], int d){
    int i = 0;
    while (x){
        str[i++] = (x%10) + '0';
        x = x/10;}
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint){
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    int i = intToStr(ipart, res, 0);
    if (afterpoint != 0){
        res[i] = '.';  // add dot
        fpart = fpart * pow(10, afterpoint);
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}


void matrix_gpio_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	/* Port A nustatomi pin'ai */
	GPIO_InitStructure.GPIO_Pin = data3_R1 | data3_R2 | data1_R2
			| la_pin | lb_pin | lc_pin | ld_pin | en_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Port B nustatomi pin'ai */
	GPIO_InitStructure.GPIO_Pin = data1_R1
			| latch_pin | clock_pin | data2_R1 | data2_R2 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void matrix_timer_init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 0;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 65454;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &timerInitStructure);
	TIM_Cmd(TIM3, ENABLE);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}


void ADC_Configuration(void){
  ADC_InitTypeDef  ADC_InitStructure;
  // PCLK2 is the APB2 clock
  // ADCCLK = PCLK2/6 = 72/6 = 12MHz
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_DeInit(ADC1);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t readADC1(uint8_t channel){
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
  // Start the conversion
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  uint16_t abutton = ADC_GetConversionValue(ADC1);
  //tikrinama koks myktukas buvo nuspaustas
  if(abutton<30){button = ATGAL; btn = ATGAL; /*Atgal*/ }else
  if (abutton<450 && abutton>300){button = DESINE; btn = DESINE; /*Desine*/ }else
  if (abutton<1050 && abutton>900){button = ZEMYN; btn = ZEMYN; /*Zemyn*/ }else
  if (abutton<1600 && abutton>1400){button = OK; btn = OK; /*OK*/ }else
  if (abutton<2300 && abutton>2100){button = KAIRE; btn = KAIRE; /*Kaire*/ }else
  if (abutton<2900 && abutton>2700){button = AUKSTYN; btn = AUKSTYN; /*Aukstyn*/ }else
  if (abutton<3400 && abutton>3200){button = MENIU; btn = MENIU; /*Meniu*/ }else{button = NO_KEY;}
  return btn;
  return button;
}



#endif /* MAIN_H_ */
