#ifndef _EBBSY_OS
	#define _EBBSY_OS

#include <stdint.h>

extern uint32_t tick_counter;

typedef enum pstat_enum {
    READY,
    RUNNING,
    WAINTING,
} pstat_type;

/*
 Task Structure
*/
typedef uint32_t pid_t; 
typedef struct pcb_s
{
	pid_t pid;
	int32_t argc;
	int32_t *argv;
	uint32_t last_tick;
	uint16_t intervall;
	void (*func)(int32_t argc, int32_t argv[]);
	uintptr_t * stackp;
	pstat_type state; 
} task_type;


void init_proc_table(void);


pid_t create(void (*func)(int32_t argc, int32_t argv[]) , int32_t argc, int32_t argv[], uint16_t intervall);


void destroy(pid_t pid);


void yield(void);


void schedule(void);


void switchContext(uintptr_t ** old_stack, uintptr_t ** new_stack);


void start(void);


void firstContext(uintptr_t * new_stack);


#endif // MACRO
