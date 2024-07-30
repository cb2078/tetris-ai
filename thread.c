#include <threads.h>
#include <stdatomic.h>

#define THREAD_COUNT 7 // TODO do this based on the system threads

static void run_jobs(thrd_start_t f, void *x)
{
	thrd_t threads[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; ++i)
		thrd_create(&threads[i], f, x);
	for (int i = 0; i < THREAD_COUNT; ++i)
		thrd_join(threads[i], NULL);
}

struct work_queue {
	volatile int next;
	volatile int count;
};

static atomic_int thread_rand(void)
{
	static _Atomic unsigned seed = 314159;
	seed = (75 * seed + 74) % (1 << 16 + 1);
	return seed;
}
