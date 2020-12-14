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
		if(sys_tick_counter == 4000)  {
			destroy(4);
		}
	}
}

void rec_test(uint32_t * a, uint32_t b) {
	if(--b > 0) {
		(*a)++;
		rec_test(a,b);
	}
}


uint32_t test_inc_func(uint32_t a ,uint32_t b) {
	uint32_t x = a + b;
	x = x - b;
	rec_test(&x, 16);
	return x;
}


uint32_t test_var = 0;

void test_01_func(int32_t argc, int32_t * arcv) {
	while (1)
	{
		test_var += argc*8;
	}
} 

void test_02_func(int32_t argc, int32_t * arcv){
	while (test_var < 0x00FFFFFF )
	{
		test_var = test_inc_func(test_var, 42);
	}
}

int main(void)
{	
	init_os();
	
	// this runs P2(leds) with scheduler from P3
	init_led();
	// tick task handler
	//create(&test_01_func, 1, (int32_t*)2, 0);
	create(&test_02_func, 1, (int32_t*)2, 0);


	start(); //starts schedule with firstContext

}
