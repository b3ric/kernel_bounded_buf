#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"
#include "buffer.h"


void * producer(void *args){
	
	int enq_char = 48; /* char to be enqueued, ASCII '0' initially */
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		
		// Sleep random ms
		usleep(rand() % RAND_MOD);
		
		// Check if over ASCII '9', if so go back to '0'
		if (enq_char > ASCII_9) { enq_char = ASCII_0; }
		
		// Declare data buf, + 1 for null terminator
		char data[DATA_LENGTH + 1];
		
		// Fill up buffer with enq_char
		memset(data, enq_char, DATA_LENGTH);
		
		// Null terminator
		data[DATA_LENGTH] = '\0';
		
		enq_char++; /* Increment char */
		
		enqueue_buffer(data);
		printf("============================\n\n");
	}
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		
		// sleep random ms
		usleep(rand() % RAND_MOD);
		
		// Declare data buf, + 1 for null terminator
		char data[DATA_LENGTH + 1];
		
		// Fill up buffer with empty chars ' '
		memset(data, ' ', DATA_LENGTH);
		
		// Null terminator
		data[DATA_LENGTH] = '\0';
		
		dequeue_buffer(data);
		printf("============================\n\n");
	}
	return NULL;
}

int main(void)
{
	init_buffer();

	pthread_t p,c;
	
	pthread_create(&p, NULL, &producer, NULL);
	pthread_create(&c, NULL, &consumer, NULL);
	
	pthread_join(p, NULL);
	pthread_join(c, NULL);
	
	delete_buffer();
	
	return 0;
}