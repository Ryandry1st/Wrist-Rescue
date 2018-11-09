#include <Wire.h>
#include <math.h>
#include <Adafruit_LiquidCrystal.h>

//Address for device
#define AccAddress 0x53
//Register Addresses
#define X_LSB 0x32
#define X_MSB 0x33
#define Y_LSB 0x34
#define Y_MSB 0x35
#define Z_LSB 0x36
#define Z_MSB 0x37
#define BW_RATE       0x2C
#define POWER_CTL     0x2D
#define INT_ENABLE    0x2E
#define INT_MAP       0x2F
#define DATA_FORMAT   0x31
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
  AccSetup(); //setup Accelerometer, 16G mode
  lcd.begin(16, 2); //initialize lcd
  lcd.setCursor(14,0);
  lcd.print(12);
  lcd.setCursor(14,1);
  lcd.print("OK");
  DDRD&= ~(0x08); // make interrupt pin int1 (pin3) input
  PORTD&= ~(0x08);
  EIMSK = 0x02; //enable int1 external interrupt
  EICRA = 0b00001100; //interrupt on rising edge
  sei(); //enable all interrupts
}

void loop() {
  //request communication with device
  Wire.beginTransmission(AccAddress); 
  Wire.write(X_LSB); //request start of data registers
  Wire.endTransmission(false); //end writing communication
  Wire.requestFrom(AccAddress,6); //get 6 bytes of data, x, y, z MSB and LSB parts
  x = Wire.read(); x |= (Wire.read() << 8); //organize data to 13 bits
  y = Wire.read(); y |= (Wire.read() << 8);
  z = Wire.read(); z |= (Wire.read() << 8);
  x_g = (float)x/256; //convert data to g force by *16/4096
  y_g = (float)y/256;
  z_g = (float)z/256;
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
//Should print 11100101 to the monitor
//to make sure I2C communication is working
void test(){
    //Test to see if working
  int who = 0;
  Wire.beginTransmission(AccAddress);
  Wire.write(0x00);

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
void AccSetup(){
  writeTo3Axis(POWER_CTL, 0x08); // Set accelerometer to normal measurement mode

  //Set to normal operation and max 800Hz data rate good noise performance
  writeTo3Axis(BW_RATE, 0x0D);
    // High resoultion mode, right justified, 16G range
  writeTo3Axis(DATA_FORMAT, 0b0001011);
  // enable data ready interrupt on INT1
  writeTo3Axis(INT_ENABLE, 0x80);
  writeTo3Axis(INT_MAP, 0x00);

}
//interrupt for when data is ready each time
ISR(INT1_vect){
    count++; //increment data received counter
    //Serial.println("interrupt");
}


