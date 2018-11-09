#include <Wire.h>
#include <math.h>
#include <Adafruit_LiquidCrystal.h>

//Address for device
#define AccAddress 0x1D
//Register Addresses
#define X_MSB 0x01
#define X_LSB 0x02
#define Y_MSB 0x03
#define Y_LSB 0x04
#define Z_MSB 0x05
#define Z_LSB 0x06
#define G_CFG 0x0E
#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E
#define MMA8451_REG_PL_CFG        0x11
//8G value for G_CFG register
#define _8G    0b00000010
long count=0; //For how many readings per showing on LCD screen
int16_t z = 0; //Holds the register data from accelerometer in 14 bits
int16_t x = 0;
int16_t y = 0;
float z_g = 0; //converted data to gforce in each direction
float x_g = 0;
float y_g = 0;
float zmax=0; //holds the max zvalue for fall test

//create lcd object for lcd screen
Adafruit_LiquidCrystal lcd(0);



void setup() {
  //start serial monitor so zmax can be traced
  Serial.begin(9600);
  //begin I2C communication and interrupts
  Wire.begin();
  AccSetup(_8G); //setup Accelerometer, 8G mode
  lcd.begin(16, 2); //initialize lcd
  DDRD&= ~(0x08); // make interrupt pin int1 (pin3) input
  PORTD&= ~(0x08);
  EIMSK = 0x02; //enable int1 external interrupt
  EICRA = 0b00001100; //interrupt on rising edge
  sei(); //enable all interrupts
}

void loop() {
  //request communication with device
  Wire.beginTransmission(AccAddress); 
  Wire.write(X_MSB); //request start of data registers
  Wire.endTransmission(false); //end writing communication
  Wire.requestFrom(AccAddress,6); //get 6 bytes of data, x, y, z MSB and LSB parts
  x = Wire.read(); x <<= 8; x |= Wire.read(); x >>= 2; //organize data to 14 bits
  y = Wire.read(); y <<= 8; y |= Wire.read(); y >>= 2;
  z = Wire.read(); z <<= 8; z |= Wire.read(); z >>= 2;
  x_g = (float)x/1024; //convert data to g force by *8/8192
  y_g = (float)y/1024;
  z_g = (float)z/1024;
  if(abs(zmax) < abs(z_g)){ //calculate zmax to put on serial monitor
    zmax = abs(z_g);
  }
  //after 25 readings print to lcd screen
  if(count >= 25){
    lcd.setCursor(0, 0);
    lcd.print("X"); //print data values, placement depends on + or -
    if(x_g < 0) lcd.print(x_g);
    else{
      lcd.print(" ");
      lcd.print(x_g);
    }
    lcd.setCursor(7,0);
    lcd.print("Y");
    if(y_g < 0) lcd.print(y_g);
    else{
      lcd.print(" ");
      lcd.print(y_g);
    }
    lcd.setCursor(0,1);
    lcd.print("Z");
    if(z_g < 0) lcd.print(z_g);
    else{
      lcd.print(" ");
      lcd.print(z_g);
    }
    lcd.setCursor(7,1);
    lcd.print("BPM123"); //place for BPM
    Serial.print("Zmax: "); //put Zmax on serial monitor
    Serial.println(zmax);
    count = 0; //reset data collection counter
  }
}


//test function
//Should print 11010 to the monitor
//to make sure I2C communication is working
void test(){
    //Test to see if working
  int who = 0;
  Wire.beginTransmission(AccAddress);
  Wire.write(0x0D);

  Wire.endTransmission(false);
  Wire.requestFrom(AccAddress,1);
  if(Wire.available()){
     who= Wire.read();
  }
  Serial.println(who, BIN);
}


//function to write to a register in accelerometer
//needs register address reg, and value to write to it
void writeTo3Axis(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(AccAddress);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)(value));
  Wire.endTransmission();
}

//Function to setup Accelerometer
void AccSetup(uint8_t Gval){
    writeTo3Axis(MMA8451_REG_CTRL_REG2, 0x40); // reset the 3Axis Accelerometer
  delay(100);

  //Turn this to the G mode wanted
  writeTo3Axis(G_CFG, Gval);
    // High resoultion mode
  writeTo3Axis(MMA8451_REG_CTRL_REG2, 0x02);
  // enable interrupt on INT1
  writeTo3Axis(MMA8451_REG_CTRL_REG4, 0x01);
  writeTo3Axis(MMA8451_REG_CTRL_REG5, 0x01);

  // Turn on orientation config
  writeTo3Axis(MMA8451_REG_PL_CFG, 0x40);

  // Activate at max rate, No low noise mode
  writeTo3Axis(MMA8451_REG_CTRL_REG1, 0x01);

}
//interrupt for when data is ready each time
ISR(INT1_vect){
    count++; //increment data received counter
}


