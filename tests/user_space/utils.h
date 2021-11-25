#ifndef UTILS  
#define UTILS

#define LOOP_COUNT 10000
#define RAND_MOD 10 /* Modulus for rand() */
#define ASCII_0 48 /* Representation of '0' */
#define ASCII_9 57 /* Representation of '9' */
#define PRINT_N 10 /* Printing PRINT_N lines of buffer data only */

void * producer(void *args);
void * consumer(void *args);

#endif