#include <stdint.h>
#include <LPC177x_8x.h>
#include "ebsy_os.h"
#include "delay.h"
#include "led.h"

// simple test task
void test(int32_t argc, int32_t * arcv) {

	int count = argc;
	while(1) {
		count++;
		yield();
	}	

}

uint32_t test_add(uint32_t a, uint32_t b) {

	volatile int solution = 0;
	solution = a+b;
	yield();
	return solution;
}

// test task with yield in subfunction
void test2(int32_t argc, int32_t * arcv) {

	uint32_t count = argc;
	
	while(1) {
		count = test_add(count , count + 1);
	}	

}


void tick_task_handler(int32_t argc, int32_t * arcv) {
	while (1)
	{
		delayms(1);
		tick_counter++;
		yield();
	}
}


int main(void)
{	
	init_proc_table();
	
	
	//uncomment this to run P2(leds) with scheduler from P3
	init_led();
	create(&tick_task_handler, 0, 0, 0);

	
	//create simple test function
	//create(&test,   2, 0, 0);
	//create(&test2, 32, 0, 0);
	
	start(); //starts schedule with firstContext

}
