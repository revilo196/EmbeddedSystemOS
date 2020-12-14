#include "ebsy_os.h"

/*
	Process Table and stack
*/
#define NPROCS 10
#define STACK_SIZE 128
task_type processTable[NPROCS]; 
task_type * ready_list[NPROCS];

uint32_t stack[NPROCS][STACK_SIZE];


task_type * current_task;
task_type * next_task;
pid_t current_pid; //aktuell laufender proc
uint32_t sys_tick_counter = 0; //globaler tick counter

// flag wird gesetzt um zwischen os code und task code zu untescheiden 
// für fehler behandlung
uint32_t _os_exec_flag = 1; 
uint32_t _os_wait_flag = 0; 

#define STCTRL_BASE           0xE000E010
#define STCTRL ((uint32_t volatile*) STCTRL_BASE)
#define STRELOAD_BASE 0xE000E014
#define STRELOAD ((uint32_t volatile*) STRELOAD_BASE)
#define ICSR_BASE 0xE000ED04
#define ICSR ((uint32_t volatile*) ICSR_BASE)

task_type ** next_ready_proc = ready_list;
task_type ** last_waiting_proc = ready_list;

task_type * pop_task(void){
	
	if (last_waiting_proc == next_ready_proc)  {
		return 0;
	}
	
	task_type * p = (*next_ready_proc);
	next_ready_proc++;
	
	 if (next_ready_proc >= next_ready_proc + NPROCS) {
		 task_type ** next_ready_proc = ready_list;
	 }
	 
	 return p;
}

 void push_task(task_type * task) {
	(*last_waiting_proc) = task;
	 last_waiting_proc++;
	 if (last_waiting_proc >= last_waiting_proc + NPROCS) {
		 task_type ** last_waiting_proc = ready_list;
	 }
}



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


void SysTick_Handler(void) {
	_os_exec_flag = 1;
	// update waiting tasks from the tick counter
	for(int  i = 0; i < NPROCS; i++) {
			// check all waiting proc for changes
			if(processTable[i].pid  > 0 && processTable[i].state == WAINTING) {
				if (processTable[i].last_tick + processTable[i].intervall <= sys_tick_counter )
				{
					processTable[i].state = READY;
					push_task(&processTable[i]);
				}
			}
		}
	

	sys_tick_counter++;

	current_task = current_proc(); // aktuellen task ermitteln
	next_task = pop_task();       // nächsten task bestimmen

	if(current_task != next_task && next_task != 0) {

		if (current_task->state == RUNNING) {		

				//change state
			if (current_task-> intervall > 0) {
				current_task->state = WAINTING;		
				current_task->last_tick = sys_tick_counter; // save last tick
			} else {
				current_task->state = READY;
				push_task(current_task);
				

			}
			
		
		} else if (current_task->state == TERMINATED) {

			current_task->pid = 0; //free the current task
			current_task->func = 0;
			
		} else if (current_task->state == WAINTING_ON) {
			// set waiting on resource
		}

		next_task->state = RUNNING;   // 
		current_pid = next_task->pid; // set the running pid

		(*ICSR) = 0x10000000; // PendSVtriggern

	}
	_os_exec_flag = 0;
}


/*
	clear Process Table
	pid 0 is reserved to represent emtpy space in the processTable
*/
void init_os(void) {
	_os_exec_flag = 1;

	for(int  i = 0; i < NPROCS; i++) {
		processTable[i].pid = 0;
		processTable[i].func = 0;
	}

	(*STRELOAD) = 47999999;
	(*STCTRL) |= 0x07;
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
			processTable[i].last_tick = sys_tick_counter;
			processTable[i].argc = argc;
			processTable[i].argv = argv;
			processTable[i].stackp = &stack[i][STACK_SIZE-1];
			processTable[i].stackp = processTable[i].stackp - 16;  //decremnet stackpointer to fit Function and arguments
			
			processTable[i].stackp[0] = 1; // r4
			processTable[i].stackp[1] = 2; // r5
			processTable[i].stackp[2] = 3; // r6
			processTable[i].stackp[3] = 4; // r7
			processTable[i].stackp[4] = 5; // r8
			processTable[i].stackp[5] = 6; // r9
			processTable[i].stackp[6] = 7; // r10
			processTable[i].stackp[7] = 8; // r11

			processTable[i].stackp[8] = (uintptr_t)argc; //r0
			processTable[i].stackp[9] = (uintptr_t)argv; //r1

			processTable[i].stackp[10] = 9; // r2
			processTable[i].stackp[11] = 10; // r3
			processTable[i].stackp[12] = 11; // r12

			processTable[i].stackp[13] = (uintptr_t)&stop; // LR 
			processTable[i].stackp[14] = (uintptr_t)func; // PC
			processTable[i].stackp[15] = 0x41000000; // xPSR
			
		  //  processTable[i].stackp = processTable[i].stackp - 1;  //decremnet sp more
			
			processTable[i].state = READY;
			push_task(&processTable[i]);
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
		task_type *  pcb = task_from_pid(pid);
		if (pcb != 0) {
			pcb->pid = 0;
			pcb->func = 0;
		}
		return;
}

/**
 * @brief fuction to stop a task
 * 
 */
void stop() {
	task_type *  pcb = current_proc();
	if (pcb != 0) {
			pcb->state = TERMINATED;
	}
	while (1){/* wait for next systick to close this task */}
}

void wait(){
	task_type *  pcb = current_proc();
	_os_wait_flag = 1;
		(*ICSR) = 0x04000000; // SysTick mit wait flag triggern
	while(_os_wait_flag);
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
		
		if(processTable[proc_index].pid  > 0 && (processTable[proc_index].state == READY || processTable[proc_index].state == RUNNING)) { 
			last_proc_table++;
			return & processTable[proc_index];
		}
	}
	
	return 0;
}



// den ersten Task starten und schedule starten
void start(void) {
	task_type * n_pcb = pop_task();
	current_pid = n_pcb->pid;
	n_pcb->state = RUNNING;
	_os_exec_flag = 0;
	firstContext(n_pcb->stackp);
}



// task wechseln 
/*void yield(void) {
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

	
}*/

/**
switchContext FUNCTION  ; start of function 

    EXPORT switchContext 
	;alten Kontext Sichern
	PUSH { r1,r0,lr }
	PUSH { r4-r7 }
	PUSH { r8-r12 }  
	
	; ALTEN STACKPOINTER im PCB SPEICHERN
	STR SP, [R0,#0x00] ;r0 addresse zu altem stackpointer
	
	LDR SP, [R1,#0x00]  ;swap stackpointer R1(ADRESSE ZU NEUEN STACKPOINTER im PCB)

   	; hole den neuen Kontext
	; vomneuen Stack 
	pop { r8-r12 }      
	pop { r4-r7 }
	pop { r1,r0,lr }  ;!! R1 und R0 halten argumente zum start eines neuen Task's
	
    BX  LR

	ENDFUNC  ; end of function
**/
