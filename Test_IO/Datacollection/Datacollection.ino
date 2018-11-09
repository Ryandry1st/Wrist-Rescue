/*
 * Data_collection.c
 *
 *  Created on: Oct 18th, 2018
 *      Author: dreifuerstrm
 *
 *  Code for collecting heart rate and 3 -axis accelerometer data using an arduino with the designated pin connections.
 *  Data will store in a large array until full. Once it is full it will flash the Red LED.
 *  The green LED will flash every so often to indicate that it is operational.
 *  If a user is "falling" they should press a button as they begin to fall and push again once they have completed the fall.
 *  The user can push the other button to have the arduino send the entire data array over serial communication.
 *  The user can then either use the python script (recommended) for converting to csv, or attempt to copy and paste from the serial monitor.
 *  The green LED will turn on at this point and turn off when the data has been transmitted, cleared, and the device is sampling again.
 *
 *  Last updated 11/6/18
 */


#include <arduino.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_LiquidCrystal.h>

#define heart_pulse_threshold 640
#define heart_averages 9
#define button1 PB0 // fall button
#define button2 PB1 // report data button
#define greenled PB2
#define redled PB3
#define heartbeat A2
#define accel_int PD3
#define maxdata 2000

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

typedef struct datapacket { // 57 bit data package
  bool danger;                      // 1 for dangerous, 0 for safe
  int8_t heartrate;                   // heart rate measurement
  int16_t accelx, accely, accelz;    // accelerometer measurements for the x, y, and z
} datapacket;

int count = 0;    //For how many readings per showing on LCD screen
int16_t z = 0;    //Holds the register data from accelerometer in 14 bits
int16_t x = 0;
int16_t y = 0;
bool a_state = 1;    // 0 = relax, 1 = read accel
bool interrupt_occured = 0; // checks if interrupt occured during the writing of the LCD

unsigned int timesinceheart = 1;    // time between heart beat readings for determining bpm
bool midpulse = 0;
int heartfilter[heart_averages] = { 16927, 16927, 16927, 16927, 16927, 16927,
    16927, 16927, 16927 };    // set to initialize with 65 BPM
int clocks_to_heart = 0;
int8_t filterindex = 0;    // sample number from heartfilter to replace

Adafruit_LiquidCrystal lcd(0);    //create lcd object for lcd screen

// function prototypes
void AccSetup(uint8_t Gval);
void writeTo3Axis(uint8_t reg, uint8_t value);
int moving_average_filter(int newvalue, int values[], int length);
void accel_test();
void lcd_setup();
void i2c_com();

// waits for button pushes and then toggles LEDs
ISR(PCINT0_vect) {
  if (!(PINB & (1 << button1))) {
    PORTB ^= 1 << greenled;
    Serial.println("Fall button was pushed and released");
  } else if (!(PINB & (1 << button2))) {
    PORTB ^= 1 << redled;
    Serial.println("Utility button was pushed");
  }
}

// interrupt for when data is ready each time
ISR(INT1_vect) {
  count++;    //increment data received counter
  a_state = 1;
  interrupt_occured = 1;
}

// reads the heart rate sensor and determines if a heart beat has occurred
ISR(ADC_vect) {
  Serial.println(ADCW);
  ADCSRA |= 1 << ADSC;
  ADCSRA |= 1 << ADIF;

  if (ADCW > heart_pulse_threshold) {    // pulse is being detected
    if (midpulse == 0) {    // if the value has not been high previously
      midpulse = 1;
      //Serial.println(timesinceheart);
      timesinceheart = TCNT1;    // track the time since the last high value
      clocks_to_heart = moving_average_filter(timesinceheart, heartfilter);
      TIFR1 |= 1 << TOV1;
      TCNT1 = 0;    // reset timer
    }
  } else {
    if (midpulse == 1) {
      midpulse = 0;
    }
  }
}

ISR(TIMER1_OVF_vect) {
  //Serial.println("User is disconnected/heartrate below 15 BPM");
  //PORTB  ^= 1<< redled;
  TIFR1 |= 1 << TOV1;
  timesinceheart = 1;    // TODO pick a value here that evaluates to a clear value
  TCNT1 = 0;
}

void setup() {                // initial call to setup hardware pins
  Serial.begin(19200);
  DDRB &= ~1 << button1;    // pins for buttons
  DDRB &= ~1 << button2;
  PORTB |= 1 << button1;    // enable pull ups on buttons
  PORTB |= 1 << button2;
  DDRB |= 1 << greenled;    // LED pins
  DDRB |= 1 << redled;

  // Button interrupts
  PCICR |= 1 << PCIE0;     // enables pin change interrupts on PCINT[7:0]
  PCIFR |= 1 << PCIF0;     // clears the interrupt flag
  PCMSK0 = 0x03;           // enables pin change interrupts on pin 8 and 9

  // ADC Interrupts
  ADCSRB = 0x00;    // compare with internal analog and free running mode
  ADMUX = 0x43;    // selects ADC3 as the pin to use with AVcc reference
  ADCSRA |= 0xFD;    // Enable ADC, Start conversion, auto trigger, clear flag, enable interrupts, set prescaler to 128 (max)

  // Heartbeat Timer Setup
  TCCR1A = 0x00;    // no output compare, normal timer operation
  TCCR1B = 0x05;    // no input capture, 1024 prescaler = 64us resolution, max of 4.19s until overflow
  TCCR1C &= ~0xC0;    // no forcing output compare
  TIMSK1 |= 1 << TOIE1;    // turn on overflow interrupt
  TIFR1 |= 1 << TOV1;    // reset the overflow flag

  // General Timer Setup
  
  

  //begin I2C communication and interrupts
  Wire.begin();
  AccSetup();    //setup Accelerometer, 8G mode
  lcd.begin(16, 2);    //initialize lcd
  lcd.clear();
  lcd.setCursor(14, 0);
  lcd.print(12);
  lcd.setCursor(14, 1);
  lcd.print("OK");
  DDRD &= ~(0x08);    // make interrupt pin int1 (pin3) input
  PORTD &= ~(0x08);
  EIMSK = 0x02;    //enable int1 external interrupt
  EICRA = 0b00001100;    //interrupt on rising edge
  sei();
  // enable interrupts
  lcd_setup();
}

void loop() {
  
    //Serial.println(a_state);
    if (a_state == 1) {
      i2c_com();
  }
}

//configures the LCD to have the correct display characters
void lcd_setup() {
  lcd.setCursor(0, 0);
  lcd.print("X");
  lcd.setCursor(7, 0);
  lcd.print("Y");
  lcd.setCursor(0, 1);
  lcd.print("Z");
  lcd.setCursor(7, 1);
  lcd.print("BPM");

}

//function to write to a register in accelerometer
//needs register address reg, and value to write to it
void writeTo3Axis(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(AccAddress);
  Wire.write((uint8_t) reg);
  Wire.write((uint8_t) (value));
  Wire.endTransmission();
}

//Function to setup Accelerometer
void AccSetup() {
  writeTo3Axis(POWER_CTL, 0x08);    // Set accelerometer to normal measurement mode

  //Set to normal operation and max 400Hz data rate good noise performance
  writeTo3Axis(BW_RATE, 0x0C);
  // High resoultion mode, right justified, 16G range
  writeTo3Axis(DATA_FORMAT, 0b0001011);
  // enable data ready interrupt on INT1
  writeTo3Axis(INT_ENABLE, 0x80);
  writeTo3Axis(INT_MAP, 0x00);

}

// applies a filter to the heartbeat data to smooth it out
int moving_average_filter(int newvalue, int values[]) {
  int avg = newvalue / (heart_averages + 1);
  for (int i = 0; i < heart_averages; i++) {
    avg += values[i] / (heart_averages + 1);
  }
  values[filterindex] = avg;
  filterindex = (filterindex == heart_averages - 1) ? 0 : ++filterindex;    // if filter index is at length go to 0, otherwise add one to it
  return avg;
}

// accelerometer unit test
void accel_test() {
  //Test to see if working
  int who = 0;
  Wire.beginTransmission(AccAddress);
  Wire.write(0x00);

  Wire.endTransmission(false);
  Wire.requestFrom(AccAddress, 1);
  if (Wire.available()) {
    who = Wire.read();
  }
  Serial.println(who, BIN);
}

//request communication with device
void i2c_com() {
  Wire.beginTransmission(AccAddress);
  Wire.write(X_LSB);    //request start of data registers
  Wire.endTransmission(false);    //end writing communication
  Wire.requestFrom(AccAddress, 6);    //get 6 bytes of data, x, y, z MSB and LSB parts
  x = Wire.read();
  x |= (Wire.read() << 8);    //organize data to 13 bits
  y = Wire.read();
  y |= (Wire.read() << 8);
  z = Wire.read();
  z |= (Wire.read() << 8);
  //after 25 readings print to lcd screen
  if (count >= 100) {
    lcd.setCursor(1, 0);
    //print data values, placement depends on + or -

    lcd.setCursor(10, 1);
    uint8_t BPM = 60 / (clocks_to_heart / 15625.0);    // 15625 = clock ticks per second
    lcd.print(BPM);
    lcd.print(" ");
    count = 0;    //reset data collection counter
  }
  a_state = interrupt_occured;    // reset state
  interrupt_occured = 0;
}

