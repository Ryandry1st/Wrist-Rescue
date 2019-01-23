/**
* \file
* Data_collection.c
*
*  Created on: Dec 26th, 2018
*      Author: dreifuerstrm
*
*  Code for collecting heart rate and 3 -axis accelerometer data using an ATSAMD10D14 with the designated pin connections.
*  Data will store in a large array until full. Once it is full it will turn on the red and green LEDs.
*  If a user is "falling" they should press a button as they begin to fall and push again once they have completed the fall.
*  The user can push the other button to have the ATSAMD10D14 send the entire data array over serial communication.
*  The user can then either use the python script (recommended) for converting to csv, or attempt to copy and paste from the serial monitor.
*  The green LED will turn on at this point and turn off when the data has been transmitted, cleared, and the device is sampling again.
*
*  Last updated 12/26/18
*/

/************************************************************************/
/* PIN CONFIG
 PA06 = UART Tx
 PA07 = UART Rx
 PA11 = Console Rx
 PA10 = Console Tx
 PA02 = Analog Read
 PA30 = I2C SDA
 PA31 = I2C SCL
                                                                      */
/************************************************************************/

#include "asf.h"
#include "stdio_serial.h"
#include "conf_uart_serial.h"


#ifdef __cplusplus
extern "C" {
#endif

//Address for accelerometer
#define AccAddress 0x53
//Register Addresses for accelerometer
uint8_t X_LSB = 0x32;
#define X_MSB 0x33
uint8_t Y_LSB = 0x34;
#define Y_MSB 0x35
#define Z_LSB 0x36
#define Z_MSB 0x37
#define BW_RATE       0x2C
#define POWER_CTL     0x2D
#define INT_ENABLE    0x2E
#define INT_MAP       0x2F
#define DATA_FORMAT   0x31


static struct usart_module cdc_uart_module; // uart object
static struct usart_module bt_mod; // BT communication uart channel
static struct tc_module tc_instance; // timer counter object
static struct adc_module adc_mod; // adc object
static struct i2c_master_module i2c_mod; // i2C object

uint8_t accel_reading[6] = {}; // X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB values

uint8_t zero = 0;
struct i2c_master_packet i2c_packet = {.address = AccAddress, .data_length = sizeof(uint8_t), .data = &zero}; // global I2C packet


uint16_t adc_val[16] = {0};
uint8_t tx_val[14] = {65, 44, 54, 48, 56, 56, 48, 55, 55, 50, 52, 55, 3, 10};
uint8_t count = 0;

#define TC_COUNT_VALUE 55535

/************************************************************************/
/*  Function Prototypes                                                 */
/************************************************************************/
static void accel_test(void);
static void writeTo3Axis(uint8_t reg, uint8_t value);
static void AccSetup(void);
static void i2c_com(void);

/************************************************************************/
/*  Configure i2C master                                                */
/************************************************************************/
static void config_i2c(void){
	struct i2c_master_config i2c_conf; // configuration struct
	
	i2c_master_get_config_defaults(&i2c_conf);
	i2c_conf.baud_rate = I2C_MASTER_BAUD_RATE_100KHZ;
	i2c_conf.pinmux_pad0 = PINMUX_PA30C_SERCOM1_PAD0;
	i2c_conf.pinmux_pad1 = PINMUX_PA31C_SERCOM1_PAD1;
	
	i2c_master_init(&i2c_mod, SERCOM1, &i2c_conf);
	i2c_master_enable(&i2c_mod);
	//i2c_master_register_callback()
}

/************************************************************************/
/*  Configure UART for the BT                                           */
/************************************************************************/
static void config_uart(void){
	struct usart_config bt_conf;
	
	usart_get_config_defaults(&bt_conf);
	bt_conf.baudrate = 9600;
	bt_conf.mux_setting = USART_RX_2_TX_2_XCK_3;
	bt_conf.pinmux_pad0 = SERCOM0_PAD0_DEFAULT;
	bt_conf.pinmux_pad1 = SERCOM0_PAD1_DEFAULT;
	bt_conf.pinmux_pad2 = PINMUX_PA06D_SERCOM0_PAD2;
	bt_conf.pinmux_pad3 = PINMUX_PA07D_SERCOM0_PAD3;
	
	usart_init(&bt_mod, SERCOM0, &bt_conf);
	usart_enable(&bt_mod);
}


/************************************************************************/
/*  Configure ADC                                                       */
/************************************************************************/
static void adc_func(struct adc_module *const module){
	//printf("%u \n",system_cpu_clock_get_hz());
	adc_read_buffer_job(&adc_mod, adc_val, 16);
}

static void config_adc(void){
	struct adc_config adc_conf; // configuration struct
	
	adc_get_config_defaults((&adc_conf)); // initialize to default values
	adc_init(&adc_mod, ADC, &adc_conf);  // initialize the hardware with the configuration
	adc_enable(&adc_mod);                 // enable the ADC
	adc_register_callback(&adc_mod, adc_func, ADC_CALLBACK_READ_BUFFER);
	adc_enable_callback(&adc_mod, ADC_CALLBACK_READ_BUFFER);
	adc_start_conversion(&adc_mod);
	adc_read_buffer_job(&adc_mod, adc_val, 16);
}

/************************************************************************/
/*  Configure UART console                                              */
/************************************************************************/
static void configure_console(void){
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

/************************************************************************/
/*  Configure External Interrupts                                       */
/************************************************************************/
static void update_led_state(void){
	bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	if (pin_state) {
		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
	} else {
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
	}
}

/** Callback function for the EXTINT driver, called when an external interrupt
 *  detection occurs. */
static void extint_callback(void){
	update_led_state();
}

/** Configures the External Interrupt Controller to detect changes in the board
 *  button state. */
static void configure_extint(void){
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = BUTTON_0_EIC_PIN;
	eint_chan_conf.gpio_pin_mux       = BUTTON_0_EIC_MUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_BOTH;
	eint_chan_conf.filter_input_signal = true;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &eint_chan_conf);
	extint_register_callback(extint_callback, BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}


/************************************************************************/
/*  Configure Timer Counter                                             */
/************************************************************************/
static void tc_callback_to_counter(struct tc_module *const module_inst){
	static uint32_t tc_count = 0;
	tc_count ++;
	if(tc_count%800 == 0){
		//printf("The output is triggered %u \n", system_cpu_clock_get_hz());
		
	}

	tc_set_count_value(module_inst,TC_COUNT_VALUE);
}

/** Configures  TC function with the  driver.
 */
static void configure_tc(void){
	struct tc_config config_tc;

	tc_get_config_defaults(&config_tc);
	config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
	config_tc.counter_16_bit.value = TC_COUNT_VALUE;

	tc_init(&tc_instance, CONF_TC_INSTANCE, &config_tc);
	tc_register_callback(&tc_instance, tc_callback_to_counter, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable(&tc_instance);
}


int main(void){
	struct port_config pin;

	system_init();

	/* Configure UART console.*/
	configure_console();

	/* Configures the External Interrupt*/
	configure_extint();

	/* Configures  TC driver*/
	configure_tc();
	
	/* Initialize the Delay driver*/
	delay_init();
	
	/* Initialize ADC driver */
	config_adc();

	/*Enable system interrupt */
	system_interrupt_enable_global();
	
	/* Initialize UART driver */
	config_uart();
	
	/* Configure I2C */
	config_i2c();

    /*Configures PORT for LED0*/
	port_get_config_defaults(&pin);
	pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED0_PIN, &pin);

	port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);

	for (int i = 0; i < 3; i++) {
		port_pin_toggle_output_level(LED0_PIN);
		delay_s(1);
	}

	for (int i = 0; i < 20; i++) {
		port_pin_toggle_output_level(LED0_PIN);
		delay_ms(100);
	}

	port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);

	usart_write_buffer_wait(&bt_mod, tx_val, 14);
		
	AccSetup();
	accel_test();
	/*main loop*/
	while(1){
		i2c_com();
		delay_us(100);
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
	zero = POWER_CTL;
	i2c_packet.data = &zero;
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

static void accel_test(void) {
	//Test to see if working
	uint8_t whoami = 0;
	uint8_t error = 0;
	i2c_packet.data = &whoami;
	i2c_master_write_packet_wait(&i2c_mod, &i2c_packet);
	i2c_master_read_packet_wait(&i2c_mod, &i2c_packet);
	printf("test packet = %u, error = %u \n", whoami, error);
}

//request communication with device
void i2c_com(void) {
	count++;
	i2c_packet.data = &X_LSB;
	i2c_master_write_packet_wait_no_stop(&i2c_mod, &i2c_packet); //request start of data registers                                  // reads in 6 bytes of data, two for each of the x, y, and z
	i2c_packet.data = accel_reading;
	i2c_packet.data_length = 6;
	if(i2c_master_read_packet_wait(&i2c_mod, &i2c_packet) != STATUS_OK){
		printf("FAILURE");
	}
	i2c_packet.data_length = 1;
	//after 100 readings print to lcd screen
	if(count > 200){
		int16_t X = accel_reading[0] | (accel_reading[1]<<8);
		int16_t Y = accel_reading[2] | (accel_reading[3]<<8);
		int16_t Z = accel_reading[4] | (accel_reading[5]<<8);
		
		printf("X = %i, Y = %i, Z = %i \n", X, Y, Z);
		count = 0;    //reset data collection counter
	}
	
}

#ifdef __cplusplus
}
#endif
