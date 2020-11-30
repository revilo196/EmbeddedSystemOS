#include "ebsy_os.h"

/*
	Process Table and stack
*/
#define NPROCS 10
#define STACK_SIZE 64
task_type processTable[NPROCS]; 
uint32_t stack[NPROCS][STACK_SIZE];

pid_t current_pid; //aktuell laufender proc
uint32_t tick_counter = 0; //globaler tick counter

// flag wird gesetzt um zwischen os code und task code zu untescheiden 
// für fehler behandlung
uint32_t _os_exec_flag = 1; 


void HardFault_Handler(void)
{
	if(_os_exec_flag) {
		while (1)
		{
			//OS ERROR
			// kann nicht geklärt werden stop programm
		}
	} else {
		//proc error
		task_type * c_pcb = current_proc();
		c_pcb->state = TERMINATED;  // stop Faulty Proc

		start(); // restart schedue
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

// lookup task by its pid 
// important pid != index in procTable (if a task destroid and recreated pid's always increment)
task_type * task_from_pid(pid_t pid) {
		for(int  i = 0; i < NPROCS; i++) {
			if(processTable[i].pid == pid) {
				return & processTable[i];
			}
		}
		return 0;
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
			processTable[i].stackp = &stack[i][STACK_SIZE-1];
			processTable[i].stackp = processTable[i].stackp - 3;  //decremnet stackpointer to fit Function and arguments
			
			processTable[i].stackp[0] = (uintptr_t)argc; //fill stack with initial values
			processTable[i].stackp[1] = (uintptr_t)argv;
			processTable[i].stackp[2] = (uintptr_t)func;
			
		    processTable[i].stackp = processTable[i].stackp - 9;  //decremnet sp more
			
			processTable[i].state = READY;
			return pid_counter;
		}
	}
	
	return 0;
}

/**
 * @brief remove one process from the processTable
 * 
 * sets the pid and function pointer to 0
 * 
 * @param pid the pid of the process to remove
 */
void destroy(pid_t pid) {
		task_type *  pcb = task_from_pid(current_pid);
		if (pcb != 0) {
			pcb->pid = 0;
			pcb->func = 0;
		}
		return;
}



// find the current pcb
task_type * current_proc(void) {
	return task_from_pid(current_pid);
}

// den nächsten prozess der ausgefürt werden soll bestimmen
task_type * next_proc() {
	
	static uint32_t last_proc_table = 0;
	
	for(uint32_t  i = 0; i < NPROCS; i++) {
		uint32_t proc_index = (last_proc_table + i + 1) % NPROCS;
		
		if(processTable[proc_index].pid  > 0 && processTable[proc_index].state == READY) { 
			last_proc_table++;
			return & processTable[proc_index];
		}
	}
	
	return 0;
}

// task wechseln 
void yield(void) {
	_os_exec_flag = 1;

	
	//update waiting tasks from the tick counter
	for(int  i = 0; i < NPROCS; i++) {
		 // check all waiting proc for changes
		 if(processTable[i].pid  > 0 && processTable[i].state == WAINTING) {
			 if (processTable[i].last_tick + processTable[i].intervall <= tick_counter )
				{
					processTable[i].state = READY;
				}
		 }
	 }
	 
	 task_type * c_pcb = current_proc(); // aktuellen task ermitteln
	 task_type * n_pcb = next_proc(); // nächsten task bestimmen
	 
	//change state
	if (c_pcb-> intervall > 0) {
		c_pcb->state = WAINTING;		
		c_pcb->last_tick = tick_counter; // save last tick
	} else {
		c_pcb->state = READY;	

	}

	 n_pcb->state = RUNNING;
	 
	 current_pid = n_pcb->pid; //set the running pid

	 //store the current stackpointer in switchContext 
	 _os_exec_flag = 0;
	
	 switchContext(&c_pcb->stackp, &n_pcb->stackp);  

	
}

// den ersten Task starten und schedule starten
void start(void) {
	task_type * n_pcb = next_proc();
	current_pid = n_pcb->pid;
	_os_exec_flag = 0;
	firstContext(n_pcb->stackp);
}

