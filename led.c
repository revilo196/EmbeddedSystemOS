#include "led.h"
#include "gpio.h"
#include "ebsy_os.h"

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

// Global led tick counter											
uint32_t led_counter[NUM_LEDS] = {0,0,0,0,0,0,0,0};

void led_func(int32_t argc, int32_t argv[]) {
    while (1)
    {
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
void init_led(void) {
	
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = gpio_init(PORT_LEDS,PIN_LEDS+i);
		gpio_set_mode( &leds[i], OUTPUT);
	}
	
	create(&led_func,   		1,led_nummers+0, 0);
	create(&led_func,   		1,led_nummers+1, 0);
	create(&led_func,   		1,led_nummers+2, 0);
	create(&led_func,   		1,led_nummers+3, 0);
	create(&led_func,   		1,led_nummers+4, 0);
	create(&led_func,   		1,led_nummers+5, 0);
	create(&led_func,  		 	1,led_nummers+6, 0);
	create(&led_func,   		1,led_nummers+7, 0);
	create(&animation_func,	0,0,			   200*MS);
}
