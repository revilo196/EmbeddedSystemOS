#ifndef _EBBSY_OS
	#define _EBBSY_OS

#include <stdint.h>


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
	uint32_t * stackp;
	pstat_type state; 
} task_type;


void init_proc_table(void);


pid_t create(void (*func)(int32_t argc, int32_t argv[]) , int32_t argc, int32_t argv[], uint16_t intervall);


void destroy(pid_t pid);


void yield(void);


void switchContext( /** some importent arguments from what to what**/);

#endif // MACRO