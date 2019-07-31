/*
 * OLED_functions.h
 *
 * Created: 3/3/2019 9:20:20 PM
 *  Author: kipferlj (dreifuerstrm tried to take credit for it)
 */ 


#ifndef OLED_FUNCTIONS_H_
#define OLED_FUNCTIONS_H_

#define OLED_ADD 0x3C            // Address for OLED
#define LSBcolumn   0x00         // Register Addresses for OLED
#define MSBcolumn   0x10
#define row         0xB0
#define displayreg  0x40

static void writeToOLED(uint8_t reg, uint8_t value);
static void OLEDSetup(void);
void Screenoff(void);
void Screenon(void);
void cleardisplay(void);
void timeselect(uint8_t hour, uint8_t minute);
void colon(void);
void zero(uint8_t start);
void one(uint8_t start);
void two(uint8_t start);
void three(uint8_t start);
void four(uint8_t start);
void five(uint8_t start);
void six(uint8_t start);
void seven(uint8_t start);
void eight(uint8_t start);
void nine(uint8_t start);
void Call_OLED(void);
void Fall_OLED(int num);
void F_OLED(uint8_t start);
void A_OLED(uint8_t start);
void L_OLED(uint8_t start);
void C_OLED(uint8_t start);
void blank(uint8_t start);
void heart(uint8_t start);
void heartrate(uint8_t start);



void timeselect(uint8_t hour, uint8_t minute){
	//Screenon();
	//cleardisplay();
	colon();
	if(hour/10 == 1) one(0);
	else if(hour/10 == 2) two(0);
	else zero(0);
	if(hour%10 == 9) nine(27);
	else if(hour%10 == 1) one(27);
	else if(hour%10 == 2) two(27);
	else if(hour%10 == 3) three(27);
	else if(hour%10 == 4) four(27);
	else if(hour%10 == 5) five(27);
	else if(hour%10 == 6) six(27);
	else if(hour%10 == 7) seven(27);
	else if(hour%10 == 8) eight(27);
	else zero(27);
	if(minute/10 == 9) nine(81);
	else if(minute/10 == 1) one(81);
	else if(minute/10 == 2) two(81);
	else if(minute/10 == 3) three(81);
	else if(minute/10 == 4) four(81);
	else if(minute/10 == 5) five(81);
	else zero(81);
	if(minute%10 == 9) nine(108);
	else if(minute%10 == 1) one(108);
	else if(minute%10 == 2) two(108);
	else if(minute%10 == 3) three(108);
	else if(minute%10 == 4) four(108);
	else if(minute%10 == 5) five(108);
	else if(minute%10 == 6) six(108);
	else if(minute%10 == 7) seven(108);
	else if(minute%10 == 8) eight(108);
	else zero(108);
}

//heartrate display
void heartrate(uint8_t bpm){
	heart(10);
	if(bpm < 10){
		blank(71);
		blank(98);
		if(bpm == 9) nine(44);
		else if(bpm == 1) one(44);
		else if(bpm == 2) two(44);
		else if(bpm == 3) three(44);
		else if(bpm == 4) four(44);
		else if(bpm == 5) five(4);
		else if(bpm == 6) six(44);
		else if(bpm == 7) seven(44);
		else if(bpm == 8) eight(44);
		else zero(44);
	}
	else if(bpm >9 && bpm <100){
		blank(98);
		if(bpm/10 == 9) nine(44);
		else if(bpm/10 == 1) one(44);
		else if(bpm/10 == 2) two(44);
		else if(bpm/10 == 3) three(44);
		else if(bpm/10 == 4) four(44);
		else if(bpm/10 == 5) five(4);
		else if(bpm/10 == 6) six(44);
		else if(bpm/10 == 7) seven(44);
		else if(bpm/10 == 8) eight(44);
		else zero(44);
		if(bpm%10 == 9) nine(71);
		else if(bpm%10 == 1) one(71);
		else if(bpm%10 == 2) two(71);
		else if(bpm%10 == 3) three(71);
		else if(bpm%10 == 4) four(71);
		else if(bpm%10 == 5) five(71);
		else if(bpm%10 == 6) six(71);
		else if(bpm%10 == 7) seven(71);
		else if(bpm%10 == 8) eight(71);
		else zero(71);
	}
	else{
		if(bpm/100 == 9) nine(44);
		else if(bpm/100 == 1) one(44);
		else if(bpm/100 == 2) two(44);
		else if(bpm/100 == 3) three(44);
		else if(bpm/100 == 4) four(44);
		else if(bpm/100 == 5) five(4);
		else if(bpm/100 == 6) six(44);
		else if(bpm/100 == 7) seven(44);
		else if(bpm/100 == 8) eight(44);
		else zero(44);
		if((bpm%100)/10 == 9) nine(71);
		else if((bpm%100)/10 == 1) one(71);
		else if((bpm%100)/10 == 2) two(71);
		else if((bpm%100)/10 == 3) three(71);
		else if((bpm%100)/10 == 4) four(71);
		else if((bpm%100)/10 == 5) five(71);
		else if((bpm%100)/10 == 6) six(71);
		else if((bpm%100)/10 == 7) seven(71);
		else if((bpm%100)/10 == 8) eight(71);
		else zero(71);
		if(bpm%10 == 9) nine(98);
		else if(bpm%10 == 1) one(98);
		else if(bpm%10 == 2) two(98);
		else if(bpm%10 == 3) three(98);
		else if(bpm%10 == 4) four(98);
		else if(bpm%10 == 5) five(98);
		else if(bpm%10 == 6) six(98);
		else if(bpm%10 == 7) seven(98);
		else if(bpm%10 == 8) eight(98);
		else zero(98);
	}
}


//Write call right in the middle of the screen

void Call_OLED(void){
  C_OLED(14);
  A_OLED(40);
  L_OLED(66);
  L_OLED(92);
}

void Fall_OLED(int num){
  F_OLED(1);
  A_OLED(27);
  L_OLED(53);
  L_OLED(79);
  if(num == 9) nine(105);
  else if(num == 1) one(105);
  else if(num == 2) two(105);
  else if(num == 3) three(105);
  else if(num == 4) four(105);
  else if(num == 5) five(105);
  else if(num == 6) six(105);
  else if(num == 7) seven(105);
  else if(num == 8) eight(105);
  else zero(105);
}

//clears the whole OLED screen to black
void cleardisplay(void){
	for(uint8_t i = 0; i<128; i++){
		for(uint8_t j = 0; j<4; j++){
			writeToOLED(LSBcolumn,i%16);
			writeToOLED(MSBcolumn,16+((i>>4)%16));
			writeToOLED(row,0xB0+j);
			writeToOLED(displayreg,0x00);
			
		}
	}
}

//writes colon in exact middle of the OLED screen
void colon(void){
	uint8_t start = 54; //where to start writing pixels
	//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i>7 && i<12)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i>7 && i<12)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		writeToOLED(displayreg,0x00);
	}
}
//prints zero onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void zero(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xFF);
		else if(i>3 && i<8)
		writeToOLED(displayreg,0x00);
		else if(i>7 && i<12)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xFF);
		else if(i>3 && i<8)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>3 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}
//prints 1 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void one(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i>3 && i<8)
		writeToOLED(displayreg,0xF0);
		else if(i>7 && i<12)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i>7 && i<12)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i>7 && i<12)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>3 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 2 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void two(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i>3 && i<16)
		writeToOLED(displayreg,0xF0);
		else if(i>15)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		writeToOLED(displayreg,0x0F);
	}
}

//prints 3 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void three(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i>7 && i<12)
		writeToOLED(displayreg,0xF0);
		else if(i>11 && i<16)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4)
		writeToOLED(displayreg,0xF0);
		else if(i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>3 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 4 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void four(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i>7 && i<12)
		writeToOLED(displayreg,0xF0);
		else if(i>11 && i<16)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<4)
		writeToOLED(displayreg,0xF0);
		else if(i>3 && i<8)
		writeToOLED(displayreg,0x0F);
		else if(i>11 && i<16)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i>11 && i<16)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>11 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 5 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void five(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0xF0);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4)
		writeToOLED(displayreg,0xF0);
		else if(i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>3 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 6 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void six(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4)
		writeToOLED(displayreg,0x00);
		else if(i>3 && i<8)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<4)
		writeToOLED(displayreg,0xFF);
		else if(i>15)
		writeToOLED(displayreg,0x00);
		else
		writeToOLED(displayreg,0xF0);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i>3 && i<16)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 7 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void seven(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i>15)
		writeToOLED(displayreg,0x0F);
		else if(i>11 && i<16)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i>3 && i<8)
		writeToOLED(displayreg,0xF0);
		else if(i>7 && i<12)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i<4)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

//prints 8 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void eight(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<4 || i>15)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0xF0);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i<4 || i>15)
		writeToOLED(displayreg,0x00);
		else
		writeToOLED(displayreg,0x0F);
	}
}

//prints 9 onto OLED at a designated start points
//each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void nine(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i<4 || i>15)
		writeToOLED(displayreg,0xF0);
		else
		writeToOLED(displayreg,0x0F);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i<4)
		writeToOLED(displayreg,0x0F);
		else if(i>15)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0xF0);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i<16 && i>11)
		writeToOLED(displayreg,0xF0);
		else if(i>15)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i<12)
		writeToOLED(displayreg,0x0F);
		else
		writeToOLED(displayreg,0x00);
	}
}

  //prints C onto OLED at a designated start points
  //each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void C_OLED(uint8_t start){
    for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB0);
   if(i<4 || i>15)
      writeToOLED(displayreg,0xF0);
    else
      writeToOLED(displayreg,0x0F);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB1);
   if(i<4)
      writeToOLED(displayreg,0xFF);
    else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB2);
   if(i<4)
      writeToOLED(displayreg,0xFF);
   else if(i>15)
      writeToOLED(displayreg,0xF0);
   else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB3);
   if(i<4 || i>15)
      writeToOLED(displayreg,0x00);
    else
      writeToOLED(displayreg,0x0F);
  }
}

  //prints A onto OLED at a designated start points
  //each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void A_OLED(uint8_t start){
    for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB0);
   if(i<4 || i>15)
      writeToOLED(displayreg,0x00);
   else if(i>7 && i<12)
      writeToOLED(displayreg,0x0F);
   else
      writeToOLED(displayreg,0xF0);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB1);
   if(i<4 || i>15)
      writeToOLED(displayreg,0xFF);
    else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB2);
   if(i<4 || i>15)
      writeToOLED(displayreg,0xFF);
   else
      writeToOLED(displayreg,0x0F);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB3);
   if(i<4 || i>15)
      writeToOLED(displayreg,0x0F);
    else
      writeToOLED(displayreg,0x00);
  }
}

  //prints L onto OLED at a designated start points
  //each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void L_OLED(uint8_t start){
    for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB0);
   if(i<4)
      writeToOLED(displayreg,0xFF);
   else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB1);
   if(i<4)
      writeToOLED(displayreg,0xFF);
   else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB2);
    if(i<4)
      writeToOLED(displayreg,0xFF);
    else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB3);
    writeToOLED(displayreg,0x0F);
  }
}

  //prints F onto OLED at a designated start points
  //each four loop is an 8 (vertical) by 20 (horizontal) pixel coverage
void F_OLED(uint8_t start){
    for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB0);
   if(i<4)
      writeToOLED(displayreg,0xFF);
   else
      writeToOLED(displayreg,0x0F);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB1);
   if(i<4)
      writeToOLED(displayreg,0xFF);
   else if(i>3 && i<16)
      writeToOLED(displayreg,0xF0);
   else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB2);
    if(i<4)
      writeToOLED(displayreg,0xFF);
    else
      writeToOLED(displayreg,0x00);
  }
  for(uint8_t i = 0; i<20; i++){
    writeToOLED(LSBcolumn,(start+i)%16);
    writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
    writeToOLED(row,0xB3);
    if(i<4)
      writeToOLED(displayreg,0x0F);
    else
      writeToOLED(displayreg,0x00);
  }
}

//clears character space
void blank(uint8_t start){
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<20; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		writeToOLED(displayreg,0x00);
	}
}
//prints heart shape
void heart(uint8_t start){
	for(uint8_t i = 0; i<27; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB0);
		if(i == 0 || i == 26)
		writeToOLED(displayreg,0xE0);
		else if(i == 1 || i == 13 || i == 25)
		writeToOLED(displayreg,0xF0);
		else if(i == 2 || i == 12 || i == 14 || i == 24)
		writeToOLED(displayreg,0xF8);
		else if(i == 3 || i == 4 || i == 10 || i == 11 || i == 15 || i == 16 || i == 22 || i == 23)
		writeToOLED(displayreg,0xFC);
		else
		writeToOLED(displayreg,0xFE);
	}
	for(uint8_t i = 0; i<27; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB1);
		if(i == 0 || i == 26)
		writeToOLED(displayreg,0x07);
		else if(i == 1 || i == 25)
		writeToOLED(displayreg,0x1F);
		else if(i == 2 || i == 24)
		writeToOLED(displayreg,0x7F);
		else
		writeToOLED(displayreg,0xFF);
	}
	for(uint8_t i = 0; i<27; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB2);
		if(i == 4 || i == 22)
		writeToOLED(displayreg,0x01);
		else if(i == 5 || i == 21)
		writeToOLED(displayreg,0x03);
		else if(i == 6 || i == 20)
		writeToOLED(displayreg,0x07);
		else if(i == 7 || i == 19)
		writeToOLED(displayreg,0x0F);
		else if(i == 8 || i == 18)
		writeToOLED(displayreg,0x1F);
		else if(i == 9 || i == 17)
		writeToOLED(displayreg,0x3F);
		else if(i == 10 || i == 16)
		writeToOLED(displayreg,0x7F);
		else if(i > 10 && i <16)
		writeToOLED(displayreg,0xFF);
		else
		writeToOLED(displayreg,0x00);
	}
	for(uint8_t i = 0; i<27; i++){
		writeToOLED(LSBcolumn,(start+i)%16);
		writeToOLED(MSBcolumn,16+(((start+i)>>4)%16));
		writeToOLED(row,0xB3);
		if(i == 12 || i == 14)
		writeToOLED(displayreg,0x01);
		else if(i == 13)
		writeToOLED(displayreg,0x03);
		else
		writeToOLED(displayreg,0x00);
	}
}


#endif /* OLED_FUNCTIONS_H_ */