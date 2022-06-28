#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <stddef.h>

#define TASK_NAME_MAX_LEN 32

typedef struct task_t {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];
    struct thread_t* threads;
    //TODO: add list of child threads
    
    struct task_t* next;
} task_t;

extern size_t next_task_id;

task_t* create_task(char *name);

#endif
