/**
* \file
* main.c

	Final implementation level code for the Wrist Rescue device. 
	
*  Last updated 5/12/19
*/

/************************************************************************/
/* PIN CONFIG
PA00 = Audio shutdown
PA01 = BT_status_int
PA02 = Heartbeat
PA03 = Vref
PA04 = OLED_RESET
PA05 = Battery Monitoring
PA06 = BT_Command to control BT overwrite settings
PA07 Butt2
PA08 = SDA
PA09 = SCL
PA10 = uC_Tx
PA11 = uC_Rx
PA14 = .... Floating
PA15 = Butt1
PA16 = LED4 = 
PA17 = LED5 = 
PA18 = LED6 = 
PA19 = LED1 = 
PA22 = LED2 = 
PA23 = LED3 = 
PA24 = console Tx
PA25 = console Rx
PA27 = Unused
PA28 = Unused
PA30 = SWCLK
PA31 = SWDIO
*/
/************************************************************************/

#include <asf.h>
#include "conf_uart_serial.h"
#include "RF_2_Algo.h"
#include "Test_data.h"
#include "OLED_functions.h"

#define startup "Compiled on "__DATE__" at "__TIME__"\n"

#define TC_COUNT_VALUE 3036
#define num_packets 30
#define sensitivity 10

#define AccAddress 0x53         // Address for accelerometer
#define BW_RATE       0x2C      // Register Address for accelerometer
#define POWER_CTL     0x2D
#define INT_ENABLE    0x2E
#define INT_MAP       0x2F
#define DATA_FORMAT   0x31
#define connected_stat 67        // ASCII C which correlates to a connection with audio and hands free profile

 #define AUD_SHDN_n 0
 #define BT_STATUS_INT 1
 #define HEARTSIGNAL 2
 #define OLED_RESET 4
 #define BATT_MON 5
 #define BT_COMMAND 6
 #define BUTT2 7
 #define SDA 8
 #define SCL 9
 #define UC_TX 10
 #define UC_RX 11
 #define BUTT1 15
 #define LED4 16
 #define LED5 17
 #define LED6 18
 #define LED1 19
 #define LED2 22
 #define LED3 23
 #define CONSOLE_TX 24
 #define CONSOLE_RX 25

#define LED_ON false
#define LED_OFF true
#define adc_averages 16
#define heart_averages 10

enum states{normal, disconnected, falling, calling, reset} state=disconnected; // state machine states
enum oled_stat{on, off} oled_state=off; // oled states
	
uint8_t leds[6] = {LED1, LED2, LED3, LED4, LED5, LED6};
uint8_t X_LSB = 0x32;

static struct usart_module cdc_uart_module;     // uart object
static struct usart_module bt_mod;              // BT communication uart channel
static struct tc_module tc_instance;            // timer counter object
static struct tc_module tc2_instance;           // timer 2 object
static struct adc_module adc_mod;               // adc object for HR
static struct adc_module adc2_mod;               // adc object for power
static struct i2c_master_module i2c_mod;        // i2C object
static struct rtc_module rtc_mod;               // Real time clock object
struct rtc_calendar_time time;

uint8_t accel_reading[6] = {0};                  // X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB values

uint8_t dummy = 0; // dummy variable for i2c packet
struct i2c_master_packet i2c_packet = {.address = AccAddress, .data_length = sizeof(uint8_t), .data = &dummy}; // global I2C packet
struct i2c_master_packet luke_packet = {.address = OLED_ADD, .data_length = sizeof(uint8_t)};

typedef struct datapacket { // data packet for machine learning algorithm
	int16_t xavg, yavg, zavg, dx, dy, dz, x, y, z;    // accelerometer measurements for the x, y, and z
	
} datapacket;

const uint16_t adc_threshold = 1300;                                                                 // 1.8V/3.3V*4095div/V ~ 2350div
uint16_t adc_val[adc_averages] = {0};                                                                    // average adc measurements for smoothing
bool midpulse = 0;                                                                                   // records if the heart pulse is above the threshold
uint8_t timesinceheart = 1;                                                                          // measures time since the last heartbeat, should be 122 for 60bpm
uint8_t filter_index = 0;                                                                            // index for circular buffered FIR filter
uint8_t heartfilter[heart_averages] = {122, 122, 122, 122, 122, 122, 122, 122, 122, 122};            // previous heart rate values, 122 corresponds to 60 BPM
uint8_t heart_filter_old = 122;                                                                      // most recent heart time rate

uint16_t adc_pwr[adc_averages] = {0};																	 //to store adc value measuring battery power
uint8_t sys_pwr = 0;																				 //indicator that the system is normal 0, low power 1, or no power 2
const uint16_t low_pwr_threshold = 3970;															 //low power threshold which is 3.2/3.3*4095
const uint16_t no_pwr_threshold = 3722;																 //no power threshold which is 3/3.3*4095

uint8_t ryan_num[14] = {65, 44, 54, 48, 56, 56, 48, 55, 55, 50, 52, 55, 13, 10};                     // Ryan's phone number in ASCII                  
uint8_t lukes_num[14] = {65, 44, 53, 55, 52, 53, 50, 57, 48, 56, 54, 55, 13, 10};                    // Luke's phone number in ASCII
uint8_t end_call[3] = {69, 13, 10};                                                                  // End call ASCII commands
uint8_t Q_code[3] = {81, 13, 10};                                                                    // Q - status ASCII command
uint8_t rediscover[5] = {64, 44, 49, 13, 10};                                                        // Rediscoverable ASCII command
uint16_t fall_timer, BT_timer, t_heart, oled_timer = 0;                                              // timers for various peripherals

// machine learning data packets
// datapacket test_data[num_packets] = {data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, data13, data14, data15, data16, data17, data18, data19, data20};
datapacket data_packets[num_packets] = {0};
uint8_t data_index = 0;                                                                              // tracks the current data slot to fill
uint8_t bt_failures = 0;                                                                             // number of failures in a row with BT
bool HR_on = 1;


/************************************************************************/
/*  Function Prototypes                                                 */
/************************************************************************/
static bool accel_test(void);
static void writeTo3Axis(uint8_t reg, uint8_t value);
static void AccSetup(void);
static void i2c_com(void);
static void configure_LEDS(void);
bool fall_test(struct datapacket *const packs);
static void make_call(void);
static void LEDS_off(void);
static void check_BT_status(void);
static void moving_average_filter(uint8_t count_value);
static bool BT_Status_read(void);
static void BT_programming(void);
static void BT_Vol_Set(void);

/************************************************************************/
/*  Configure RTC                                                       */
/************************************************************************/
static void config_rtc(void){
	struct rtc_calendar_config rtc_config;
	rtc_calendar_get_config_defaults(&rtc_config);
	rtc_calendar_init(&rtc_mod, RTC, &rtc_config);
	rtc_calendar_enable(&rtc_mod);
}

/************************************************************************/
/*  Configure ADC                                                       */
/************************************************************************/
static void adc_func(struct adc_module *const module){
	// determines when a heart beat has occurred based on the adc measurements
	uint16_t adc_avg = 0;
	for(int i = 0; i < adc_averages; i++){  // smooth the adc readings
		adc_avg += adc_val[i];
	}
	adc_avg /= adc_averages;
	//printf("ADC = %u", adc_avg);
	
	if(adc_avg >= adc_threshold){
		if(midpulse == 0){ // check if value was not already above the threshold
			midpulse = 1;
			timesinceheart = t_heart; // record the time
			t_heart = 0;
			moving_average_filter(timesinceheart); // apply moving average filter
		}
	}
	else{
		if(midpulse){ // if it is below the threshold, ensure that this is noted
			midpulse = 0;
		}
	}
	adc_read_buffer_job(&adc_mod, adc_val, adc_averages); // start preparing the next reading
}

static void adc_power(struct adc_module *const module){
	uint16_t adc_avg = 0;
	for(int i = 0; i < adc_averages; i++){  // smooth the adc readings
		adc_avg += adc_pwr[i];
	}
	adc_avg /= adc_averages;	
	if(adc_avg < no_pwr_threshold){
		sys_pwr = 2; //system has reached lowest power and needs to shutdown
		//maybe put shutdown code here/for now just blinking LED
		if(state != calling){
			port_pin_toggle_output_level(LED4); //make orange LED?
			port_pin_toggle_output_level(LED5);	
		}
	}
	else if((adc_avg < low_pwr_threshold)&&(sys_pwr!=1)){
		sys_pwr = 1; //low power has been reached, system can act accordingly
		if(state != calling){
			port_pin_set_output_level(LED4, LED_ON); //make orange LED?
			port_pin_set_output_level(LED5, LED_ON);
		}
	}
	else if(sys_pwr!=0){
		sys_pwr = 0; //system is normal mode, power is sufficient
		if(state != calling){
			port_pin_set_output_level(LED4, LED_OFF); //make orange LED?
			port_pin_set_output_level(LED5, LED_OFF);
		}
	}
	adc_read_buffer_job(&adc2_mod, adc_pwr, adc_averages); // start preparing the next reading
}

static void config_adc(void){
	//heartbeat adc configuration
	struct adc_config adc_conf;                                                // configuration struct
															                 
	adc_get_config_defaults((&adc_conf));                                      // initialize to default values
	adc_conf.clock_source = GCLK_GENERATOR_4;                                  // 8 MHz clock
	adc_conf.clock_prescaler  = ADC_CLOCK_PRESCALER_DIV256;                    // Divide by 256 to 31.25 kHz
	adc_conf.accumulate_samples  = ADC_ACCUMULATE_SAMPLES_16;                  // store 16 samples per reading
	adc_conf.reference = ADC_REFCTRL_REFSEL_AREFA;                             // external voltage as aref
	adc_conf.positive_input = ADC_POSITIVE_INPUT_PIN0 ;		                 
	adc_init(&adc_mod, ADC, &adc_conf);                                        // initialize the hardware with the configuration
	adc_enable(&adc_mod);                                                      // enable the ADC
	adc_register_callback(&adc_mod, adc_func, ADC_CALLBACK_READ_BUFFER);
	adc_enable_callback(&adc_mod, ADC_CALLBACK_READ_BUFFER);                 
															                 
	//power adc configuration								                 
	struct adc_config adc2_conf;                                                // configuration struct
															                 
//	adc_get_config_defaults((&adc2_conf));                                      // initialize to default values
//	adc2_conf.clock_source = GCLK_GENERATOR_5;                                  // 7.8125 kHz clock
//	adc2_conf.clock_prescaler  = ADC_CLOCK_PRESCALER_DIV512;                    // Divide by 512 to ~15.26 Hz
//	adc2_conf.accumulate_samples  = ADC_ACCUMULATE_SAMPLES_16;                  // store 16 samples per reading
//	adc2_conf.reference = ADC_REFCTRL_REFSEL_AREFA;                             // external voltage as aref
//	adc_conf.positive_input = ADC_POSITIVE_INPUT_PIN5 ;		                 
//	adc_init(&adc2_mod, ADC, &adc2_conf);                                        // initialize the hardware with the configuration
//	adc_enable(&adc2_mod);                                                      // enable the ADC
//	adc_register_callback(&adc2_mod, adc_power, ADC_CALLBACK_READ_BUFFER);
//	adc_enable_callback(&adc2_mod, ADC_CALLBACK_READ_BUFFER);
	
	//begin conversions
	adc_start_conversion(&adc_mod);
	adc_read_buffer_job(&adc_mod, adc_val, adc_averages);                  
//	adc_start_conversion(&adc2_mod);
//	adc_read_buffer_job(&adc2_mod, adc_pwr, adc_averages);                  \
	
	//adc_conf.positive_input = ADC_
}

/************************************************************************/
/*  Configure i2C master                                                */
/************************************************************************/
static void config_i2c(void){
	struct i2c_master_config i2c_conf;                            // configuration struct
	
	i2c_master_get_config_defaults(&i2c_conf);
	i2c_conf.generator_source = GCLK_GENERATOR_4;                 // 8MHz clock
	i2c_conf.baud_rate = I2C_MASTER_BAUD_RATE_100KHZ;
	i2c_conf.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0;
	i2c_conf.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1;
	
	i2c_master_init(&i2c_mod, SERCOM0, &i2c_conf);
	i2c_master_enable(&i2c_mod);
}

/************************************************************************/
/*  Configure UART for the BT                                           */
/************************************************************************/
static void config_uart(void){
	struct usart_config bt_conf;                           // configuration struct
	
	usart_get_config_defaults(&bt_conf);
	bt_conf.baudrate = 115200;                             // Baud to 115200
	bt_conf.generator_source = GCLK_GENERATOR_4;           // 8MHz clock
	bt_conf.mux_setting = USART_RX_3_TX_2_XCK_3;           // Rx on pad 3, Tx on pad 2
	bt_conf.pinmux_pad0 = SERCOM2_PAD0_DEFAULT;
	bt_conf.pinmux_pad1 = SERCOM2_PAD1_DEFAULT;
	bt_conf.pinmux_pad2 = PINMUX_PA10D_SERCOM2_PAD2;
	bt_conf.pinmux_pad3 = PINMUX_PA11D_SERCOM2_PAD3;
	
	usart_init(&bt_mod, SERCOM2, &bt_conf);
	usart_enable(&bt_mod);
}

/************************************************************************/
/*  Configure External Interrupts                                       */
/************************************************************************/

static void butt1_callback(void){
	// button 1 functions including call initiation and hour incrementing
	if(state == normal){
		LEDS_off();
		port_pin_set_output_level(LED1, LED_ON); // turn on red led
		if(sys_pwr > 0){
			port_pin_set_output_level(LED4, LED_ON); //make orange LED?
			port_pin_set_output_level(LED5, LED_ON);
		}
		printf("You have 10s to decline before call is made");
		state = falling;
		cleardisplay();
		fall_timer = 0;
	}
	else if(state == disconnected){
		if(time.hour == 23){
			time.hour = 0;
		}
		else{
			time.hour += 1;
		}
		rtc_calendar_set_time(&rtc_mod, &time);
		rtc_calendar_get_time(&rtc_mod, &time);
		timeselect(time.hour, time.minute);
		Screenon();
	}
	
}

static void butt2_callback(void){
	// button 2 functions like declining a call, turning on the OLED, incrementing the minute, and end the call
	printf("data index = %u\n", data_index);
	if(state == falling){
		printf("Call declined\n");
		LEDS_off();
		if(sys_pwr > 0){
			port_pin_set_output_level(LED4, LED_ON); //make orange LED?
			port_pin_set_output_level(LED5, LED_ON);
		}
		state = normal;
		Screenoff();
		rtc_calendar_get_time(&rtc_mod, &time);
		timeselect(time.hour, time.minute);
	}
	else if(state == normal){
		printf("State is normal\n");
		check_BT_status();
		if(oled_state == on){
			if(HR_on == 0){
				HR_on = 1;
				cleardisplay();
				heartrate(60);
				oled_timer = 0;
			}
			else{
				cleardisplay();
				rtc_calendar_get_time(&rtc_mod, &time);
				timeselect(time.hour, time.minute);
				oled_timer = 0;
				HR_on = 0;
			}
		}
		else{
			rtc_calendar_get_time(&rtc_mod, &time);
			timeselect(time.hour, time.minute);
			Screenon();
			HR_on = 0;
		}
	}
	else if(state == calling){
		usart_write_buffer_wait(&bt_mod, end_call, 3);
		printf("Call ended\n");
		// disable audio
		port_pin_set_output_level(AUD_SHDN_n, true);
		uint8_t dumb[5];
		usart_read_buffer_job(&bt_mod, dumb, 5);
		Screenoff();
	}
	else if(state == disconnected){
		rtc_calendar_get_time(&rtc_mod, &time);
		time.minute += 1;
		rtc_calendar_set_time(&rtc_mod, &time);
		timeselect(time.hour, time.minute);
		Screenon();
	}
}


static void read_UART_callback(void){ // resets the check BT status timer when the pin is pulled low
	BT_timer = 0;
}


static void check_BT_status(){
	if(BT_Status_read()){
		delay_ms(10);
		if(BT_Status_read()){
			printf("double bt fail\n");
			LEDS_off();
			port_pin_set_output_level(LED3, LED_ON); //one blue LED
			if(sys_pwr > 0){
				port_pin_set_output_level(LED4, LED_ON); //make orange LED?
				port_pin_set_output_level(LED5, LED_ON);
			}
			state = disconnected;
			BT_timer = 0;
			bt_failures += 1;
		}
		else{
			bt_failures = 0;
		}
	}
	else{
		bt_failures = 0;
	}
	if(bt_failures >= 5){
		usart_write_buffer_wait(&bt_mod, rediscover, 5);
		uint8_t status[6] = {0};
		usart_read_buffer_job(&bt_mod, status, 6);
		bt_failures = 0;
		printf("Super rediscover\n");
	}
}

static void configure_extint(void){
	// set up the two buttons and BT interrupt pins 
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = BUTT1;
	eint_chan_conf.gpio_pin_mux       = 0;
	eint_chan_conf.gpio_pin_pull = EXTINT_PULL_UP;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_FALLING;
	eint_chan_conf.filter_input_signal = true;
	extint_chan_set_config(BUTT1, &eint_chan_conf);
	extint_register_callback(butt1_callback, BUTT1, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTT1, EXTINT_CALLBACK_TYPE_DETECT);
	
	struct extint_chan_conf butt2_conf;
	extint_chan_get_config_defaults(&butt2_conf);
	butt2_conf.gpio_pin = BUTT2;
	butt2_conf.gpio_pin_mux = 0;
	butt2_conf.gpio_pin_pull = EXTINT_PULL_UP;
	butt2_conf.filter_input_signal = true;
	butt2_conf.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(BUTT2, &butt2_conf);
	extint_register_callback(butt2_callback, BUTT2, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTT2, EXTINT_CALLBACK_TYPE_DETECT);
	
	struct extint_chan_conf BT_int_conf;
	extint_chan_get_config_defaults(&BT_int_conf);
	BT_int_conf.gpio_pin = BT_STATUS_INT;
	BT_int_conf.gpio_pin_mux = 0;
	BT_int_conf.gpio_pin_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	BT_int_conf.filter_input_signal = true;
	BT_int_conf.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(BT_STATUS_INT, &BT_int_conf);
	extint_register_callback(read_UART_callback, BT_STATUS_INT, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BT_STATUS_INT, EXTINT_CALLBACK_TYPE_DETECT);
	
}

/************************************************************************/
/*  Configure UART console                                              */
/************************************************************************/
static void configure_console(void){
	struct usart_config usart_conf;                                // serial console configuration struct

	usart_get_config_defaults(&usart_conf);                        
	usart_conf.generator_source = GCLK_GENERATOR_4;                // 8 MHz clock
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate = CONF_STDIO_BAUDRATE;                     // Baud to 115200

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

/************************************************************************/
/*  Configure Timer Counter                                             */
/************************************************************************/
static void tc_callback_to_counter(struct tc_module *const module_inst){ // fast clock counter occurs every 8.192ms
	t_heart++;
	tc_set_count_value(module_inst, 0);                      // reset to 0
}

static void tc2_callback_to_counter(struct tc_module *const module_inst){ // slow clock counter occurs every 0.5s
	fall_timer++;
	BT_timer++;
	if(oled_state == on){
		oled_timer++;
	}
	if(oled_timer >= 19){                                                 // 10 second timeout on screen
		Screenoff();
		oled_timer = 0;
	}
	if(BT_timer == 8){                                                   // checks the bt status after 6s
		check_BT_status();
	}
	if(!(fall_timer%15) && (state == normal || state == disconnected)){                           // update the OLED time every 5s
		rtc_calendar_get_time(&rtc_mod, &time);
		timeselect(time.hour, time.minute);
	}
	
	if(fall_timer >= 20 && state == falling){                           // 10s after a fall is detected
		printf("It has been 10s without a decline, calling now\n");
		make_call();
		fall_timer = 0;

	}
	if(fall_timer > 21){                                                // record the heart rate every 10s or so
		printf("BPM = %u, heartfil = %u", 60/(heart_filter_old/122.0), heart_filter_old);
		fall_timer = 0;
	}
	tc_set_count_value(module_inst, TC_COUNT_VALUE);                     // reset to 3036 to make it a perfect 0.5s
}

/** Configures  TC function with the  driver.
*/
static void configure_tc(void){
	struct tc_config tc_config;
	struct tc_config tc2_config;

	tc_get_config_defaults(&tc_config);
	tc_config.clock_source = GCLK_GENERATOR_4;
	tc_config.counter_size = TC_COUNTER_SIZE_16BIT;
	tc_config.counter_16_bit.value = 0;
	tc_config.clock_prescaler = TC_CLOCK_PRESCALER_DIV1; // 8MHz clock @16b = ovf 8.192ms

	tc_init(&tc_instance, TC1, &tc_config);
	tc_register_callback(&tc_instance, tc_callback_to_counter, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable(&tc_instance);
	
	tc_get_config_defaults(&tc2_config);
	tc2_config.clock_source = GCLK_GENERATOR_4;
	tc2_config.counter_size = TC_COUNTER_SIZE_16BIT;
	tc2_config.counter_16_bit.value = TC_COUNT_VALUE;       // reset to 3036 for 0.5s period
	tc2_config.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;  // 8MHz clock / 64 (@16b - 3036) = ovf 0.5s
	
	tc_init(&tc2_instance, TC2, &tc2_config);
	tc_register_callback(&tc2_instance, tc2_callback_to_counter, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc2_instance, TC_CALLBACK_OVERFLOW);
	tc_enable(&tc2_instance);
}

/************************************************************************/
/*  Configure LEDS                                       */
/************************************************************************/
static void configure_LEDS(void){
	struct port_config pins;       
	port_get_config_defaults(&pins);
	pins.direction = PORT_PIN_DIR_OUTPUT;
	for(int i = 0; i<6; i++){
		port_pin_set_config(leds[i], &pins); // set each of the led pins to outputs
	}
	port_pin_set_config(LED0_PIN, &pins);
	port_pin_set_config(BT_COMMAND, &pins);
	port_pin_set_output_level(BT_COMMAND, false);
	LEDS_off();
	port_pin_set_config(AUD_SHDN_n, &pins);
	port_pin_set_output_level(AUD_SHDN_n, false);
	port_pin_set_config(BATT_MON, &pins);
	port_pin_set_output_level(BATT_MON, true);
	
}

int main (void)
{
	bool fell = 0; // not falling unless the system determines it is
	
	bool reprogram = 0; // value if you want to reprogram the BT module
	system_init();
	
	configure_console();
	for(int k=0; k<3; k++){
		port_pin_set_output_level(leds[k], true);
	}
	
	delay_init();
	
	delay_ms(100);
	
	printf(startup);
	
	configure_LEDS();
	delay_ms(500);
	
	configure_tc();
	
	system_interrupt_enable_global();
	
	config_uart();
	
	if(reprogram == 1){
		BT_programming();
	}
	else{
		uint8_t D[3] = {68, 13, 10};
		usart_write_buffer_wait(&bt_mod, D, 3);
		delay_s(1);
	}
	
	config_i2c();
	
	uint8_t ft2_var = 52; // random variable that is greater than 10 so that the code executes on the first run of falling
	
	configure_extint();
	
	OLEDSetup();
	
	cleardisplay();

	time.year = 2019;
	time.month = 1;
	time.day = 1;
	time.hour = 1;
	time.minute = 1;
	time.second = 00;
	config_rtc();
	rtc_calendar_set_time(&rtc_mod, &time);
	
	timeselect(time.minute, time.second);
	Screenon();
	
	config_adc();
	
	AccSetup();
	
//	while(1){
//		delay_s(5);
//		rtc_calendar_get_time(&rtc_mod, &time);
//		timeselect(time.minute, time.second);
//	}
	
	if(accel_test()){
		printf("\nAccel not working properly\n");
		}

	LEDS_off();
	
	while(1){
		switch(state){
			case normal:
			if(data_index < num_packets){
				i2c_com();
				//if(!(data_index % 5)){
					//printf("%i %i %i %i %i %i %i %i %i \n", data_packets[data_index-1].xavg,
					 //data_packets[data_index-1].yavg, data_packets[data_index-1].zavg, data_packets[data_index-1].dx, data_packets[data_index-1].dy, data_packets[data_index-1].dz,
					  //data_packets[data_index-1].x, data_packets[data_index-1].y, data_packets[data_index-1].z);
				//}
			}
			else{
				fell = fall_test(data_packets);
				data_index = 0;
				if(fell){
					LEDS_off();
					port_pin_set_output_level(LED1, LED_ON); // turn on red led
					if(sys_pwr > 0){
						port_pin_set_output_level(LED4, LED_ON); //make orange LED?
						port_pin_set_output_level(LED5, LED_ON);
					}
					printf("You have 10s to decline before call is made\n");
					state = falling;
					fall_timer = 0;
					cleardisplay();
				}
			}
			
			delay_ms(30);
			break;
		case falling:
			if(ft2_var != fall_timer/2){
				Screenon();
				// enable audio
				port_pin_set_output_level(AUD_SHDN_n, true);
				Fall_OLED(9-(fall_timer/2));
				oled_timer = 0;
				ft2_var = fall_timer/2;
			}
			break;
		case calling:
			if(fall_timer == 1){
				cleardisplay();
				Call_OLED();
			}
			oled_timer = 0;
			delay_ms(100);
			break;
		default:
			delay_ms(100);
			break;	
		}
	}
}

//function to write to a register in accelerometer
//needs register address reg, and value to write to it
static void writeTo3Axis(uint8_t reg, uint8_t value) {
	uint8_t reg_val[2] = {reg, value};
	i2c_packet.data = reg_val;
	i2c_packet.data_length = 2;
	i2c_master_write_packet_wait(&i2c_mod, &i2c_packet);
	//i2c_packet.data = &value;
	//i2c_master_write_packet_wait(&i2c_mod, &i2c_packet);
	i2c_packet.data_length = 1;
}

//Function to setup Accelerometer
static void AccSetup(void) {
	writeTo3Axis(POWER_CTL, 0x08);    // Set accelerometer to normal measurement mode
	dummy = POWER_CTL;
	i2c_packet.data = &dummy;
	i2c_master_write_packet_wait_no_stop(&i2c_mod, &i2c_packet);
	i2c_master_read_packet_wait(&i2c_mod, &i2c_packet);
	//printf("Power_CTL = %u ", zero);

	//Set to normal operation and max 800Hz data rate good noise performance
	writeTo3Axis(BW_RATE, 0x0C);
	// High resolution mode, right justified, 16G range
	writeTo3Axis(DATA_FORMAT, 0b0001011);
	// enable data ready interrupt on INT1
	writeTo3Axis(INT_ENABLE, 0x80);
	writeTo3Axis(INT_MAP, 0x00);
}

static bool accel_test(void) {
	//Test to see if working
	uint8_t whoami = 0;
	uint8_t error = 0;
	i2c_packet.data = &whoami;
	i2c_master_write_packet_wait(&i2c_mod, &i2c_packet);
	i2c_master_read_packet_wait(&i2c_mod, &i2c_packet);
	if(whoami != 229){
		error = 1;
	}
	
	return error;
	//printf("Who am I = %u, error = %u \n", whoami, error);
}

static void make_call(void){
	// function for initiating a call
	LEDS_off();
	port_pin_set_output_level(LED1, LED_ON); //red on one LED
	port_pin_set_output_level(LED6, LED_ON); //blue on other LED
	uint8_t waste[6] = {0};
	usart_write_buffer_wait(&bt_mod, ryan_num, 14); // send call command
	usart_read_buffer_wait(&bt_mod, waste, 5); // read AOK back
//	for(int i =0; i<6;i++){
//		printf("%i ", waste[i]);
//	}
	printf("\n");
	state = calling;
	data_index = 0;
}

//request communication with device
void i2c_com(void) {
	uint8_t error = 0;
	i2c_packet.data = &X_LSB;
	i2c_master_write_packet_wait_no_stop(&i2c_mod, &i2c_packet); //request start of data registers                                  // reads in 6 bytes of data, two for each of the x, y, and z
	i2c_packet.data = accel_reading;
	i2c_packet.data_length = 6;
	if(i2c_master_read_packet_wait(&i2c_mod, &i2c_packet) != STATUS_OK){
		printf("FAILURE");
		AccSetup(); // try to reset acc
		error = accel_test();
	}
	i2c_packet.data_length = 1;
	
	if(data_index < num_packets && error == 0){
		data_packets[data_index].x = accel_reading[0] | (accel_reading[1]<<8);
		data_packets[data_index].y = accel_reading[2] | (accel_reading[3]<<8);
		data_packets[data_index].z = accel_reading[4] | (accel_reading[5]<<8);
		data_packets[data_index].dx = data_packets[data_index].x - data_packets[data_index-1].x;
		data_packets[data_index].dy = data_packets[data_index].y - data_packets[data_index-1].y;
		data_packets[data_index].dz = data_packets[data_index].z - data_packets[data_index-1].z;
		// adjusted moving average so that if there is only 1 point it does one, if there are two it does two, or if three or more, it averages the three
		data_packets[data_index].xavg = (data_index < 1) ? data_packets[data_index].x : ((data_index < 2) ? (data_packets[data_index].x + data_packets[data_index-1].x)/2 : (data_packets[data_index].x + data_packets[data_index-1].x + data_packets[data_index-2].x)/3);
		data_packets[data_index].yavg= (data_index < 1) ? data_packets[data_index].y : ((data_index < 2) ? (data_packets[data_index].y + data_packets[data_index-1].y)/2 : (data_packets[data_index].y + data_packets[data_index-1].y + data_packets[data_index-2].y)/3);
		data_packets[data_index].zavg = (data_index < 1) ? data_packets[data_index].z : ((data_index < 2) ? (data_packets[data_index].z + data_packets[data_index-1].z)/2 : (data_packets[data_index].z + data_packets[data_index-1].z + data_packets[data_index-2].z)/3);
	
		data_index+= 1; // record that another data point has been taken
	}	
	//printf("X = %i, Y = %i, Z = %i \n", data_packets[data_index].x, data_packets[data_index].y, data_packets[data_index].z);
	
}

bool fall_test(struct datapacket *const packs){
	bool dangerval[num_packets] = {0};
	for(int k = 0; k<num_packets; k++){
		dangerval[k] = RF_predict(packs[k].xavg, packs[k].yavg, packs[k].zavg, packs[k].dx, packs[k].dy, packs[k].dz, packs[k].x, packs[k].y, packs[k].z);
	}
	uint8_t total = 0;
	for(int k =0; k<num_packets; k++){
		total += dangerval[k];
	}
	
	if(total >= sensitivity){ // if more than 10 of the num_packets of data packets are classified as falling, return a fall detected
		printf("You had %u falls detected", total);
		return 1;
	}
	
	else{
		return 0;
	}
}

static void LEDS_off(void){
	for(int i = 0; i<6; i++){                            // set the LEDS to be outputs and off
		port_pin_set_output_level(leds[i], LED_OFF);
	}
}

//Turns OLED screen off
void Screenoff(void){
	writeToOLED(0xAE, 0xAE);
	cleardisplay();
	colon();
	oled_state = off;
	HR_on = 0; // TODO check here for why this would have had HR_on to be on???
}
//Turns OLED screen on
void Screenon(void){
	writeToOLED(0xAF, 0xAF);
	oled_state = on;
	oled_timer = 0;
}


//function to write to a register in OLED
//needs register address reg, and value to write to it
void writeToOLED(uint8_t reg, uint8_t value) {
	uint8_t reg_val[2] = {reg, value};
	luke_packet.data = reg_val;
	luke_packet.data_length = 2;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	//i2c_packet.data = &value;
	//i2c_master_write_packet_wait(&i2c_mod, &i2c_packet);
	luke_packet.data_length = 1;
}



//Function to setup OLED
static void OLEDSetup(void) {
	//Reset OLED screen
	struct port_config pins;
	port_get_config_defaults(&pins);
	pins.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(OLED_RESET, &pins); // set each of the led pins to outputs
	port_pin_set_output_level(OLED_RESET, true);
	delay_ms(1);
	port_pin_set_output_level(OLED_RESET, false);
	delay_ms(10);
	port_pin_set_output_level(OLED_RESET, true);
	
	//I2C setup
	//Turn screen off ans set clock division
	uint8_t reg_val5[5] = {0x00, 0xAE, 0xD5, 0x80, 0xA8};
	luke_packet.data = reg_val5;
	luke_packet.data_length = 5;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	
	//set height (32 pixels)
	writeToOLED(0x00, 32-1);
	
	//Set start line and display offset for 128 by 32
	reg_val5[0]=0x00;
	reg_val5[1]= 0xD3;
	reg_val5[2]= 0x00;
	reg_val5[3]= 0x40;
	reg_val5[4]= 0x8D;
	luke_packet.data = reg_val5;
	luke_packet.data_length = 5;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	
	//Set screen type
	writeToOLED(0x00, 0x14);
	
	//Set memory and segment maps
	reg_val5[0]= 0x00;
	reg_val5[1]= 0x20;
	reg_val5[2]= 0x00;
	reg_val5[3]= 0xA1;
	reg_val5[4]= 0xC8;
	luke_packet.data = reg_val5;
	luke_packet.data_length = 5;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	
	//Set com pins and contrast
	reg_val5[0]= 0x00;
	reg_val5[1]= 0xDA;
	reg_val5[2]= 0x02;
	reg_val5[3]= 0x81;
	reg_val5[4]= 0x8F;
	luke_packet.data = reg_val5;
	luke_packet.data_length = 5;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	
	//Set to 3.3v input
	writeToOLED(0x00, 0xAF);
	
	//Set precharge
	writeToOLED(0x00, 0xD9);
	
	//Set 3.3v reference
	writeToOLED(0x00, 0xF1);
	
	//deactivated the scroll, set to normal display, and turn screen on
	uint8_t reg_val7[7] = {0x00, 0xDB, 0x40, 0xA4, 0xA6, 0x2E, 0xAF};
	luke_packet.data = reg_val7;
	luke_packet.data_length = 7;
	i2c_master_write_packet_wait(&i2c_mod, &luke_packet);
	luke_packet.data_length = 1;
}

// recursive moving average filter
static void moving_average_filter(uint8_t count_value){
	uint16_t avg;
	avg = (count_value - heartfilter[filter_index])/heart_averages + heart_filter_old;
	heart_filter_old = avg;

	heartfilter[filter_index] = count_value;
	filter_index = (filter_index == (heart_averages - 1)) ? 0 : filter_index+1;
}

static bool BT_Status_read(void){
	bool error = 0;
	uint8_t status[6] = {0};
	usart_write_buffer_wait(&bt_mod, Q_code, 3);       // request status
	usart_read_buffer_wait(&bt_mod, status, 6);        // read the status
	
//	for(int i =0; i<6;i++){                            // print the status values
//		printf("%i ", status[i]);
//	}
	//check for immediate errors in the transmission
	if((status[4] != 13) || (status[5] != 10)){ // should end in 13 10
		error = 1;
	}
	for(int i = 0; i<4; i++){ // any value below 48 in the data is garbage
		if(status[i] < 48){
			error = 1;
		}
	}
	if(error){
		uint8_t flush[32] = {0};
		usart_read_buffer_wait(&bt_mod, flush, 32); 
//		uint8_t index_of_end = 100; // default of 100
//		for(int i = 0; i<6; i++){
//			if(status[i] == 10){
//				index_of_end = i;
//			}
//		}
//		usart_read_buffer_wait(&bt_mod, status, index_of_end+1);
	}
	
	if(error == 0){
		// check status[1] for connected or disconnected
		if(status[1] < connected_stat){                    // device is no longer connected
			state = disconnected;
			printf("Disconnected \n");
			LEDS_off();
			port_pin_set_output_level(LED3, LED_ON);   // Turn on blue led for 
			if(sys_pwr > 0){
				port_pin_set_output_level(LED4, LED_ON); //make orange LED?
				port_pin_set_output_level(LED5, LED_ON);
			}
		}
		else{                                              // status is connected
			if(state == disconnected){
				printf("Reconnected\n");
				BT_Vol_Set();
				LEDS_off();
				if(sys_pwr > 0){
					port_pin_set_output_level(LED4, LED_ON); //make orange LED?
					port_pin_set_output_level(LED5, LED_ON);
				}
				state = normal;
			}
		}
		// check status[3] for calling or not calling
		if(status[3] == 51){
			printf("Normal");
			state = normal;
			LEDS_off();
			if(sys_pwr > 0){
				port_pin_set_output_level(LED4, LED_ON); //make orange LED?
				port_pin_set_output_level(LED5, LED_ON);
			}
		}
		else if(status[3] > 51 && status[3] < 68){
			printf("Calling");
		}
		else if(status[3] >= 68){
			error = 1;
			return error;
		}
	}
	
	return error;
}


static void BT_programming(){
	// send SN,Wrist Rescue
	// send R,1
	// wait 2s
	// send S%,07
	// send R,1
	// wait 2s
	uint8_t reprogrammer[17] = {83, 78, 44, 87, 114, 105, 115, 116, 32, 82, 101, 115, 99, 117, 101, 13, 10};
	usart_write_buffer_wait(&bt_mod, reprogrammer, 17);
	uint8_t reseter [5] = {82, 44, 49, 13, 10};
		delay_ms(10);
	//usart_write_buffer_wait(&bt_mod, reseter, 5);
	delay_ms(3000);
	reprogrammer[1] = 37;
	reprogrammer[2] = 44;
	reprogrammer[3] = 48;
	reprogrammer[4] = 55;
	reprogrammer[5] = 13;
	reprogrammer[6] = 10;
	usart_write_buffer_wait(&bt_mod, reprogrammer, 7);
	delay_ms(10);
	usart_write_buffer_wait(&bt_mod, reseter, 5);
	delay_ms(3000);
	usart_write_buffer_wait(&bt_mod, rediscover, 5);
	uint8_t status[6] = {0};
	usart_read_buffer_job(&bt_mod, status, 6);
}

static void BT_Vol_Set(void){
	//send AV+ about 10 times
	uint8_t vol_plus[5] = {65, 86, 43, 13, 10};
	uint8_t dummy2[5] = {0};
	for(int i = 0; i< 10; i++){
		usart_write_buffer_wait(&bt_mod, vol_plus, 5);
		usart_read_buffer_wait(&bt_mod, dummy2, 5);
		delay_ms(10);
	}
}