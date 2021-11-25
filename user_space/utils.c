#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "buffer.h"

int enqueue_char_lower = 48;
int enqueue_char_upper = 57;

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		if (enqueue_char_lower > enqueue_char_upper){
			enqueue_char_lower = ASCII_0;
		}
		
		char data[DATA_LENGTH + 1];
		
		memset(data, enqueue_char_lower, DATA_LENGTH);
		
		data[DATA_LENGTH] = '\0';
		
		enqueue_char_lower++;
		enqueue_buffer_421(data);
		print_semaphores();
		printf("============================\n");
	}
	
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		
		char data[DATA_LENGTH + 1];
		
		memset(data, ' ', DATA_LENGTH);
		
		data[DATA_LENGTH] = '\0';
		
		dequeue_buffer_421(data);
		print_semaphores();
		printf("============================\n");
	}
	
	return NULL;
}