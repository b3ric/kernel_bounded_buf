#ifndef UTILS  
#define UTILS

#define LOOP_COUNT 100000
#define RAND_MOD 100
#define ASCII_0 48
#define PRINT_N 10

extern int enqueue_char_lower; /* To represent ASCII '0' */  
extern int enqueue_char_upper; /* To represent ASCII '9' */

char * prep_string(void);
void * producer(void *args);
void * consumer(void *args);

#endif