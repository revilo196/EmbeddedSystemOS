#include <stdint.h>
#include <LPC177x_8x.h>
#include "ebsy_os.h"
#include "gpio.h"
#include "delay.h"
#define NUM_LEDS 8
#define PORT_LEDS 0
#define PIN_LEDS 8

GPIO_pin leds[NUM_LEDS];          //array von allen leds

/** LED ANIMATION DISCRIBED AS ARRAY */
#define FRAMES 6
uint8_t animation_array[FRAMES][NUM_LEDS] = 
											{{255,128, 64,  0,    0, 64,128,255},
											{128,255,128, 64,   64,128,255,128},
											{ 64,128,255,128,  128,255,128, 64},
											{  0, 64,128,255,  255,128, 64,  0},
											{ 64,128,255,128,  128,255,128, 64},
											{128,255,128, 64,   64,128,255,128}};
uint8_t * current_frame = animation_array[0]; // pointer to the current frame

// Global tick counter											
uint32_t tick_counter = 0;
uint32_t led_counter[NUM_LEDS] = {0,0,0,0,0,0,0,0};

/**
 * LED control function
 * needs as argument the led (int 0-7) that should be PWM controlled
 * the value of the pwm is read from the current_frame form the animation
 */
void led_func(int32_t argc, int32_t argv[]) {


	

	if (argc > 0) {

		int led = argv[0];
		uint8_t value = current_frame[led];
		GPIO_pin * g_led = leds+led;

		if (led_counter[led] % 16 < value/16) {
			gpio_write(g_led, HIGH);
		} else {
			gpio_write(g_led, LOW);
		}
		led_counter[led]++;
	}
}

/**
 Function that increments/ resets the current frame pointer
*/
void animation_func(int32_t argc, int32_t argv[]) {
	
		current_frame = current_frame + NUM_LEDS;
		if (current_frame > animation_array[FRAMES-1]) {
			current_frame = animation_array[0];
		}
	
}


int32_t led_nummers[8] =  {0,1,2,3,4,5,6,7}; 
#define MS 1

/**
	Init GPIO Pins to output
    Init proecess table
**/
void init() 
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = gpio_init(PORT_LEDS,PIN_LEDS+i);
		gpio_set_mode( &leds[i], OUTPUT);
	}
	
	init_proc_table(); // set all entrys in proctable to 0

	create(&led_func,   		1,led_nummers+0, 1*MS);
	create(&led_func,   		1,led_nummers+1, 1*MS);
	create(&led_func,   		1,led_nummers+2, 1*MS);
	create(&led_func,   		1,led_nummers+3, 1*MS);
	create(&led_func,   		1,led_nummers+4, 1*MS);
	create(&led_func,   		1,led_nummers+5, 1*MS);
	create(&led_func,  		 	1,led_nummers+6, 1*MS);
	create(&led_func,   		1,led_nummers+7, 1*MS);
	create(&animation_func,	0,0,			   200*MS);
}



int main(void)
{	
	init();

	while(1)
	{	
		tick_counter++; 
		
		// remove first led task afer 4 seconds
		if (tick_counter == 4000) destroy(1);
		
		// task schedule
		for(int  i = 0; i < NPROCS; i++) {
			if (processTable[i].pid > 0) { // only run valid tasks

				if ( processTable[i].intervall == 0) { // idle task
					processTable[i].func(processTable[i].argc, processTable[i].argv); // run with arguments
				}
				else if (processTable[i].last_tick + processTable[i].intervall <= tick_counter )
				{
					processTable[i].func(processTable[i].argc, processTable[i].argv); // run with arguments
					processTable[i].last_tick  = tick_counter;
				}

			}
		}
		delayms(1);   // Basis Tackt / Delay
	}
}
