#include "fiber_channel.h"
#include "fiber_manager.h"
#include "test_helper.h"

fiber_blocking_bounded_channel_t* channel = NULL;
#define PER_FIBER_COUNT 100000
#define NUM_FIBERS 100
#define NUM_THREADS 2

int results[PER_FIBER_COUNT] = {};

void* send_function(void* param)
{
    intptr_t i;
    for(i = 1; i < PER_FIBER_COUNT + 1; ++i) {
        fiber_blocking_bounded_channel_send(channel, (void*)i);
    }
    return NULL;
}

int main()
{
    fiber_manager_init(NUM_THREADS);

    channel = fiber_blocking_bounded_channel_create(10);

    fiber_t* send_fibers[NUM_FIBERS];
    int i;
    for(i = 0; i < NUM_FIBERS; ++i) {
        send_fibers[i] = fiber_create(20000, &send_function, NULL);
    }

    for(i = 0; i < NUM_FIBERS * PER_FIBER_COUNT; ++i) {
        intptr_t result = (intptr_t)fiber_blocking_bounded_channel_receive(channel);
        results[result - 1] += 1;
    }

    for(i = 0; i < NUM_FIBERS; ++i) {
        fiber_join(send_fibers[i], NULL);
    }

    for(i = 0; i < PER_FIBER_COUNT; ++i) {
        test_assert(results[i] == NUM_FIBERS);
    }
    fiber_blocking_bounded_channel_destroy(channel);

    return 0;
}
