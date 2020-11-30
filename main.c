#include <stdint.h>
#include <LPC177x_8x.h>
#include "ebsy_os.h"
#include "delay.h"
#include "led.h"

/**
* Increments the tick_counter used for timing leds
* not perfect timing but good enougth
*/
void tick_task_handler(int32_t argc, int32_t * arcv) {
	while (1)
	{
		delayms(1);
		tick_counter++;
		
		if(tick_counter == 4000)  {
			destroy(4);
		}
		
		
		yield();
	}
}


int main(void)
{	
	init_proc_table();
	
	// this runs P2(leds) with scheduler from P3
	init_led();
	// tick task handler
	create(&tick_task_handler, 0, 0, 0);

	start(); //starts schedule with firstContext

}
