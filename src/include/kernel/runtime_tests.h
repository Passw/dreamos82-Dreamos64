#ifndef _RUNTIME_TESTS_H
#define _RUNTIME_TESTS_H

#include <spinlock.h> 

extern spinlock_t* test_lock;

void execute_runtime_tests();
void test_spinlock(char *c);
#endif
