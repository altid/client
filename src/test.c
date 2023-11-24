#include <stdlib.h>
#include <stdio.h>
#include <theft.h>

#include "queue.h"

lqueue_t *list;

static enum theft_trial_res
prop_encoded_and_decoded_data_should_match(struct theft *t, void *arg1) {
    struct buffer *input = (struct buffer *)arg1;
    return THEFT_RUN_PASS;
}

bool test_encode_decode_roundtrip(void) {
    /* Get a seed based on the current time */
    theft_seed seed = theft_seed_of_time();

    /* Property test configuration.
     * Note that the number of type_info struct pointers in
     * the .type_info field MUST match the field number
     * for the property function (here, prop1). */
    struct theft_run_config config = {
        .name = __func__,
        .prop1 = prop_encoded_and_decoded_data_should_match,
        //.type_info = { &random_buffer_info },
        .seed = seed,
    };

    /* Run the property test. */
    enum theft_run_res res = theft_run(&config);
    return res == THEFT_RUN_PASS;
}

int
main(int argc, char **argv)
{
    /**
     * Testing Queue
     *  - feed an array to the queue, read it back from all of our threads
     *    then verify all elements exist 
     *    - sort before and after
     *    - use a simple loop-offset assertion
     * Testing Client
     *  - create a mock client to feed data
     *    testing alt_next is a very thin wrapper over queue tests, may skip
     *  - cmds parse to struct
     */
    for(int i = 0; i < 7500; i++) {
        queue_push(list, i % 8);
    }

    queue_push(list, -1);
    for(int i = 0; i < 7600; i++) {
        if(queue_pop(list) < 0)
            break;
    }

    queue_free(list);
}

/*
Hard to Prove, Easy to Verify
Sorting: each element should be greater than or equal to the previous element
Tokenizing: concatenating tokens with separator interposed should equal the original string, number of tokens should equal number of separators in original string - 1
*/
