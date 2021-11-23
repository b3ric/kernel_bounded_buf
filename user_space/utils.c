#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
#include "buffer.h"

int enqueue_char_lower = 48;
int enqueue_char_upper = 57;
                       
char * prep_string(void){
	
	static char enqueue_str[DATA_LENGTH];
	int i;
	
	for (i = 0; i < DATA_LENGTH; i++){
		enqueue_str[i] = enqueue_char_lower;
	}
	
	enqueue_str[DATA_LENGTH] = 0;
	enqueue_char_lower++;
	
	if (enqueue_char_lower > enqueue_char_upper) {
		enqueue_char_lower = ASCII_0;
	}
	
	return enqueue_str;
}

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		enqueue_buffer_421(prep_string());
		print_semaphores();
		printf("============================\n");
	}
	
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		char buffer[DATA_LENGTH] = "";
		usleep(rand() % RAND_MOD);
		dequeue_buffer_421(buffer);
		print_semaphores();
		printf("============================\n");
	}
	
	return NULL;
}