/*
 * matrix.c
 *
 *  Created on: 2016-05-15
 *      Author: DMB-220
 */

//Ikeliami sriftai
#include "font.h"
#include "images.h"

//Led matrix dydis
#define PLOTIS 96
#define AUKSTIS 64

/* buffers for pixel data */
unsigned char Buffer[768] = {0};
unsigned char Buffer_copy[768] = {0};


/* fast bit masks */
unsigned char bitmask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
unsigned char ibitmask[]={0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};

unsigned char rowas[]={0x00000000, 0x00000008, 0x00000010, 0x00000018, 0x00000020,
		0x00000028, 0x00000030, 0x00000038, 0x00000040, 0x00000048, 0x00000050, 0x00000058,
		0x00000060, 0x00000068, 0x00000070, 0x00000078};

//Isvalomi duomenys is Buffer'io
void isvalyti() {
    volatile uint8_t *ptr = Buffer_copy;
    for (uint16_t i = 0; i < (PLOTIS * AUKSTIS / 8); i++) {
        *ptr = 0x00;
        ptr++;
    }
}

// draw a  pixel
void  setPoint(unsigned char x, unsigned char y, unsigned char color) {
	if(x<PLOTIS || y<AUKSTIS){
  unsigned int pos;
  unsigned int xi = (95-x);
  pos = (xi<<3);
  pos += (y>>3);
  if(color){
	  Buffer_copy[pos] |= bitmask[y&0x07];
  }else{
	  Buffer_copy[pos] &= ibitmask[y&0x07];
  }
}
}

//rasoma po viena simboli (SMALL)
void drawLetterSmall(unsigned char x, unsigned char y, char ch) {
  if (ch == ' '){return;}
  for (unsigned char yi = 0; yi<7; yi++) {
    unsigned char data = small[(ch-' ')*7+yi];
    for (unsigned char xi = 0; xi<5; xi++)
      if ((data>>(4-xi))&0x01)
        setPoint(x+xi,y+yi,1);
  }
}

//rasoma po viena simboli (NORMAL)
void drawLetterNormal(unsigned char x, unsigned char y, char ch) {
  if (ch == ' '){return;}
  for (unsigned char yi = 0; yi<12; yi++) {
    unsigned char data = normal[(ch-' ')*12+yi];
    for (unsigned char xi = 0; xi<8; xi++)
      if ((data>>(7-xi))&0x01)
        setPoint(x+xi,y+yi,1);
  }
}

//rasoma po viena simboli (BIG)
void drawLetterBig(unsigned char x, unsigned char y, char ch) {
  if (ch == ' '){return;}
  for (unsigned char yi = 0; yi<48; yi++) {
    unsigned char data = big[(ch-'0')*48+yi];
    for (unsigned char xi = 0; xi<8; xi++){
      if ((data>>(7-xi))&0x01){
        if(yi%2){setPoint(8+x+xi,y+yi/2,1);}else{setPoint(x+xi,y+yi/2,1);}}
	  }
  }
}

//rasoma po viena simboli (BIG)
void drawLetterBig2(unsigned char x, unsigned char y, char ch) {
  if (ch == ' '){return;}
  for (unsigned char yi = 0; yi<64; yi++) {
    unsigned char data = big2[(ch-'0')*64+yi];
    for (unsigned char xi = 0; xi<8; xi++){
      if ((data>>(7-xi))&0x01){
        if(yi%2){setPoint(8+x+xi,y+yi/2,1);}else{setPoint(x+xi,y+yi/2,1);}}
	  }
  }
}

//rasoma po viena simboli (NORMAL BOLD)
void drawLetterNormalBold(unsigned char x, unsigned char y, char ch) {
  if (ch == ' '){return;}
  for (unsigned char yi = 0; yi<16; yi++) {
    unsigned char data = normal_bold[(ch-' ')*16+yi];
    for (unsigned char xi = 0; xi<8; xi++)
      if ((data>>(7-xi))&0x01)
        setPoint(x+xi,y+yi,1);
  }
}

//rasome teksta
void drawString(unsigned char x, unsigned char y, char *str, int font) {
  char ch, sk=0, xx=x;
  while ((ch = *str++)!=0) {
    sk++;
    if(font==1){drawLetterSmall(x,y,ch); x+=6; if(sk==15){y+=8;  x=xx;}}
    if(font==4){drawLetterNormal(x,y,ch); x+=7;}
    if(font==2){drawLetterNormalBold(x,y,ch); x+=9;}
  }
}

void drawNumber(unsigned char x, unsigned char y, unsigned long val, int font, int ilgis) {
   unsigned long tmp = val;
   uint8_t pradzia=0;
   if(font==1){pradzia = (ilgis-1)*6;}
   if(font==4){pradzia = (ilgis-1)*9;}
   if(font==3){pradzia = (ilgis-1)*15;}
   if(font==5){pradzia = (ilgis-1)*15;}
   if(font==2){pradzia = (ilgis-1)*9;}
   for(int i=0; i<ilgis; i++){
    if(font==1){drawLetterSmall(x+pradzia,y,'0'+tmp%10); tmp/=10; pradzia-=6;}
    if(font==4){drawLetterNormal(x+pradzia,y,'0'+tmp%10); tmp/=10; pradzia-=8;}
    if(font==3){drawLetterBig2(x+pradzia,y,'0'+tmp%10); tmp/=10; pradzia-=15;}
    if(font==5){drawLetterBig(x+pradzia,y,'0'+tmp%10); tmp/=10; pradzia-=15;}
    if(font==2){drawLetterNormalBold(x+pradzia,y,'0'+tmp%10); tmp/=10; pradzia-=8;}
    }
}



/* refresh screen */
void atnaujinti(unsigned char row) {
	//int ro = row;
    unsigned int pos = ((unsigned int)row<<3), pos0;
    unsigned char mask;
    // disable screens
    GPIO_SetBits(GPIOA, en_pin);
    for (unsigned char i=0; i<64; i++) {
        //get current mask
        mask = bitmask[i&0x07];
        // Clock = low
        GPIO_ResetBits(GPIOB, clock_pin);
        // reset data lines to high
        GPIO_ResetBits(GPIOB, data1_R1 | data2_R1 | data2_R2);
        GPIO_ResetBits(GPIOA, data3_R2 | data3_R1 | data1_R2);
        pos0 = pos + (i>>3);
        // set data lines

        //1 ekranas
         if (Buffer[pos0]&mask)
        	 GPIO_SetBits(GPIOB, data1_R1);
          pos0+=128;
          if (Buffer[pos0]&mask)
        	GPIO_SetBits(GPIOA, data1_R2);
          pos0+=128;

        //2 ekranas
          if (Buffer[pos0]&mask)
        	GPIO_SetBits(GPIOB, data2_R1);
          pos0+=128;

          if (Buffer[pos0]&mask)
        	GPIO_SetBits(GPIOB, data2_R2);
          pos0+=128;

        //3 ekranas
          if (Buffer[pos0]&mask)
        	GPIO_SetBits(GPIOA, data3_R1);
          pos0+=128;
          if (Buffer[pos0]&mask)
        	GPIO_SetBits(GPIOA, data3_R2);

        // Clock = high
        GPIO_SetBits(GPIOB, clock_pin);
    }
    // strobe latch pin
    GPIO_SetBits(GPIOB, latch_pin);
    GPIO_ResetBits(GPIOB, latch_pin);

    // set row
    GPIO_ResetBits(GPIOA, la_pin);
    GPIO_ResetBits(GPIOA, lb_pin);
    GPIO_ResetBits(GPIOA, lc_pin);
    GPIO_ResetBits(GPIOA, ld_pin);
    GPIOA->BSRR = rowas[row];

    // enable again
    GPIO_ResetBits(GPIOA, en_pin);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//Piesiama horizontali linija
void drawHLine(unsigned char x, unsigned char y, unsigned char l, unsigned char color){
	//unsigned char by, bi;
		for (unsigned char cy=0; cy<l; cy++){
			setPoint(x+cy, y, color);
		}
}

//Piesiama vertikali linija
void drawVLine(unsigned char x, unsigned char y, unsigned char l, unsigned char color){
	//unsigned char by, bi;
		for (unsigned char cy=0; cy<l; cy++){
			setPoint(x, y+cy, color);
		}
}

//Piesiamas staciakampis
void drawRect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color){
	unsigned char tmp;
	if (x1>x2){ tmp=x1; x1=x2; x2=tmp; }
	if (y1>y2){ tmp=y1; y1=y2; y2=tmp; }

	drawHLine(x1, y1, x2-x1, color);
	drawHLine(x1, y2, x2-x1, color);
	drawVLine(x1, y1, y2-y1, color);
	drawVLine(x2, y1, y2-y1+1, color);
}

void drawImage(uint16_t xoffset, uint16_t yoffset, uint16_t width, uint16_t height, const uint8_t *image){
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            const uint8_t *byte = image + (x + y * width) / 8;
            uint8_t  bit = 7 - x % 8;
            uint8_t  pixel = (*byte >> bit) & 1;
            setPoint(x + xoffset, y + yoffset, pixel);
        }
    }
}

