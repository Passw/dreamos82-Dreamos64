#include <thread.h>
#include <task.h>
#include <kheap.h>

task_t* create_task(char* task_name, thread_t* thread) {
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    new_task->task_id = next_task_id++;
    new_task->next_thread_id = 0;
    strcpy(task_name, new_task->task_name);
    if (thread != NULL) {
        new_task->threads = thread;
        thread->tid = new_task->next_thread_id++;
    }
    // TODO
}

