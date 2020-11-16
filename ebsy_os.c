#include "ebsy_os.h"

/*
	Process Table and stack
*/
#define NPROCS 9
#define STACK_SIZE 256
task_type processTable[NPROCS]; 
uint32_t stack[NPROCS][STACK_SIZE];
pid_t current_pid;


void HardFault_Handler(void)
{
    while (1)
    {
        //NOP
    }
} 
/*
	clear Process Table
	pid 0 is reserved to represent emtpy space in the processTable
*/
void init_proc_table(void) {
	for(int  i = 0; i < NPROCS; i++) {
		processTable[i].pid = 0;
		processTable[i].func = 0;
	}
}


/**
 * @brief create a new process in the processTable that will be run by the scheduler
 * 
 * @param func function pointer to the task that should be run
 * @param argc arguments count
 * @param argv argument pointer  
 * @param intervall intervall between the tasks
 * @return pid_t retuns the assigned pid id or -1 if there is no more space in the processTable
 */
pid_t create(void (*func)(int32_t argc, int32_t argv[]) , int32_t argc, int32_t argv[], uint16_t intervall) {
	static pid_t pid_counter = 0;
	
	pid_counter++;
	
	for(int  i = 0; i < NPROCS; i++) {
		if(processTable[i].pid == 0) { // look for the first free space in the processTable
			processTable[i].pid = pid_counter;
			processTable[i].func = func;
			processTable[i].intervall = intervall;
			processTable[i].argc = argc;
			processTable[i].argv = argv;
			return pid_counter;
		}
	}
	
	return -1;
}

/**
 * @brief remove one process from the processTable
 * 
 * sets the pid and function pointer to 0
 * 
 * @param pid the pid of the process to remove
 */
void destroy(pid_t pid) {
	for(int  i = 0; i < NPROCS; i++) {
		if(processTable[i].pid == pid) {
			processTable[i].pid = 0;
			processTable[i].func = 0;
			return;
		}
	}
}