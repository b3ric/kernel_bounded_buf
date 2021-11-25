#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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
		
		enqueue_buffer_421(data);
		print_semaphores();
		printf("============================\n");
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
		
		dequeue_buffer_421(data);
		print_semaphores();
		printf("============================\n");
	}
	return NULL;
}