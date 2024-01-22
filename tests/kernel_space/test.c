#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define __NR_init_buffer 442
#define __NR_enqueue_buffer 443
#define __NR_dequeue_buffer 444
#define __NR_delete_buffer 445

#define LOOP_COUNT 100000
#define RAND_MOD 10 /* modulus for rand()*/
#define DATA_LENGTH 1024

#define ASCII_0 48 /* Representation of '0' */
#define ASCII_9 57 /* Representation of '9' */

int enq_char = 48; /* Char to be enqueued*/


long init_buffer_syscall(void){
	return syscall(__NR_init_buffer);
}

long enqueue_buffer_syscall(char * data){
	return syscall(__NR_enqueue_buffer, data);
}

long dequeue_buffer_syscall(char * data){
	return syscall(__NR_dequeue_buffer, data);
}

long delete_buffer_syscall(void){
	return syscall(__NR_delete_buffer);
}

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		
		// Sleep
		usleep(rand() % RAND_MOD);
		
		// Check if over '9', if so, back to '0' 
		if (enq_char > ASCII_9){ enq_char = ASCII_0; }
		
		// Initialize buffer, with + 1 for null terminator
		char data[DATA_LENGTH+1];
		
		// Fill buffer with enq_char
		memset(data,enq_char, DATA_LENGTH);
		
		// Null terminate
		data[DATA_LENGTH] = '\0';
		
		enq_char++;
		enqueue_buffer_syscall(data);
	}
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		
		// Sleep
		usleep(rand() % RAND_MOD);
		
		// Initialize buffer, with + 1 for null terminator
		char buf[DATA_LENGTH+1];
		
		// Fill buffer with empty char ' '
		memset(buf, ' ', DATA_LENGTH);
		
		// Null terminate
		buf[DATA_LENGTH] = '\0';
		dequeue_buffer_syscall(buf);
	}
	return NULL;
}

int main(int argc, char *argv[]){
	
	init_buffer_syscall();
	
	
	pthread_t p,c;
	
	pthread_create(&p, NULL, &producer, NULL);
	pthread_create(&c, NULL, &consumer, NULL);
	pthread_join(p, NULL);
	pthread_join(c, NULL);
	
	delete_buffer_syscall();
	
	printf("Check dmesg\n");

	return 0;
}
