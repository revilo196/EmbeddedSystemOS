#ifndef _EBBSY_OS
	#define _EBBSY_OS

#include <stdint.h>

extern uint32_t sys_tick_counter;

typedef enum pstat_enum {
	UNINITLIZED,
    READY,
    RUNNING,
    WAINTING,
	  WAINTING_ON,
	TERMINATED,
} pstat_type;

/*
 Task Structure
*/
typedef int32_t pid_t; 
typedef struct pcb_s
{
	uintptr_t * stackp;
	pid_t pid;
	int32_t argc;
	int32_t *argv;
	uint32_t last_tick;
	uint16_t intervall;
	void (*func)(int32_t argc, int32_t argv[]);
	pstat_type state; 
} task_type;


void init_os(void);

pid_t create(void (*func)(int32_t argc, int32_t argv[]) , int32_t argc, int32_t argv[], uint16_t intervall);

pid_t fork();

void destroy(pid_t pid);

void stop(void);

void switchContext(uintptr_t ** old_stack, uintptr_t ** new_stack);

void start(void);

void firstContext(uintptr_t * new_stack);


task_type * current_proc(void);
task_type * next_proc(void);

#endif // MACRO
