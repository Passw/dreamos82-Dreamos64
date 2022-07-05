#include <runtime_tests.h>
#include <thread.h>
#include <logging.h>
#include <spinlock.h>

spinlock_t* test_lock;
    char b = 'r';
    char c = 's';

void execute_runtime_tests() {
    //TODO create threads

    test_lock = spinlock_init();
    create_thread("spinlock_1", NULL, test_spinlock, &b);
    create_thread("spinlock_2", NULL, test_spinlock, &c);
    spinlock_free(test_lock);

}

void test_spinlock(char *c) {
    spinlock_acquire(&(test_lock->locked));
    loglinef(Verbose, " [Spinlock_test] I am thread: %c", *c);
    thread_sleep(1000);
    spinlock_release(&(test_lock->locked));
    thread_sleep(500);
    loglinef(Verbose, " [Spinlock_test] I am thread: %c", *c);
    thread_sleep(1000);
    spinlock_release(&(test_lock->locked));
}
