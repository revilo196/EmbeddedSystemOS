#include "led.h"
#include "gpio.h"
#include "ebsy_os.h"

#define NUM_LEDS 8
#define PORT_LEDS 0
#define PIN_LEDS 8

GPIO_pin leds[NUM_LEDS];          //array von allen leds

//Alte animation
uint8_t animation_array0[6][NUM_LEDS] = 
											{{255,128, 64,  0,    0, 64,128,255},
											{128,255,128, 64,   64,128,255,128},
											{ 64,128,255,128,  128,255,128, 64},
											{  0, 64,128,255,  255,128, 64,  0},
											{ 64,128,255,128,  128,255,128, 64},
											{128,255,128, 64,   64,128,255,128}};
					
											
/** LED ANIMATION DISCRIBED AS ARRAY */
#define FRAMES 14											
uint8_t animation_array[FRAMES][NUM_LEDS] = 
											{{255,128, 64, 32,    0, 0,  0,  0},
											{128,255,128, 64,     32, 0,  0,  0},
											{ 64,128,255,128,    64, 32,  0,  0},
											{  32, 64,128,255,   128, 64, 32,  0},
											{   0, 32, 64,128,   255, 128,64, 32},
											{   0,  0, 32, 64,   128,255,128,64}, 
											{   0,  0, 0,  32,     64,128, 255,128},
											{   0,  0, 0,  0,      32, 64, 128,255},
											{   0,  0, 0,  32,     64,128, 255,128},
											{   0,  0, 32, 64,   128,255,128,64},
											{   0, 32, 64,128,   255, 128,64, 32},
											{  32, 64,128,255,   128, 64, 32,  0},
											{ 64,128,255,128,    64, 32,  0,  0},
											{128,255,128, 64,     32, 0,  0,  0}};
uint8_t * current_frame = animation_array[0]; // pointer to the current frame

// Global led tick counter											
uint32_t led_counter[NUM_LEDS] = {0,0,0,0,0,0,0,0};

/*
 * Allgemine funktion zur Animation Einer LED mit PWM
 */
void led_func(int32_t argc, int32_t argv[]) {
    while (1)
    {
        if (argc > 0) {

            int led = argv[0];
            uint8_t value = current_frame[led]; // hole den wert der led aus dem animations array
            GPIO_pin * g_led = leds+led;

            if (led_counter[led] % 8 < value/32) {  //PWM counter
                gpio_write(g_led, HIGH);
            } else {
                gpio_write(g_led, LOW);
            }
            led_counter[led]++;
						
						// Testing HardFalultHandler with NullPointerException
				    // if (led == 0) {
						// *((volatile uint8_t*)0x000000) = 1; 
					  // }
        }

        yield();
    }
}

/**
 Function that increments/ resets the current frame pointer
*/
void animation_func(int32_t argc, int32_t argv[]) {
	while (1)
    {
		current_frame = current_frame + NUM_LEDS;
		if (current_frame > animation_array[FRAMES-1]) {
			current_frame = animation_array[0];
		}

        yield();
    }
}

int32_t led_nummers[8] =  {0,1,2,3,4,5,6,7}; 
#define MS 1

/*
 * Hier Werden alle led prozesse angelegt 
 * und die gpio_ports inits
 */
void init_led(void) {
	
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = gpio_init(PORT_LEDS,PIN_LEDS+i);
		gpio_set_mode( &leds[i], OUTPUT);
	}
	
	create(&led_func,   		1,led_nummers+0, 1);
	create(&led_func,   		1,led_nummers+1, 1);
	create(&led_func,   		1,led_nummers+2, 1);
	create(&led_func,   		1,led_nummers+3, 1);
	create(&led_func,   		1,led_nummers+4, 1);
	create(&led_func,   		1,led_nummers+5, 1);
	create(&led_func,  		 	1,led_nummers+6, 1);
	create(&led_func,   		1,led_nummers+7, 1);
	create(&animation_func,	0,0,			   150*MS);
}
