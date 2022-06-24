#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>

typedef struct {
    size_t task_id;
    char task_name[TASK_NAME_MAX_LEN];
    struct thread_t* threads;
    //TODO: add list of child threads
} task_t;

#endif
