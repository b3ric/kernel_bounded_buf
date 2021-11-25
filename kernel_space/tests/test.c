#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define __NR_init_buffer_421 442
#define __NR_enqueue_buffer_421 443
#define __NR_dequeue_buffer_421 444
#define __NR_delete_buffer_421 445

#define LOOP_COUNT 100000
#define RAND_MOD 10
#define DATA_LENGTH 1024
#define ASCII_0 48

int enqueue_char_lower = 48;
int enqueue_char_upper = 57;

long init_buffer_421_syscall(void){
	return syscall(__NR_init_buffer_421);
}

long enqueue_buffer_421_syscall(char * data){
	return syscall(__NR_enqueue_buffer_421, data);
}

long dequeue_buffer_421_syscall(char * data){
	return syscall(__NR_dequeue_buffer_421, data);
}

long delete_buffer_421_syscall(void){
	return syscall(__NR_delete_buffer_421);
}

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		if (enqueue_char_lower > enqueue_char_upper){
			enqueue_char_lower = ASCII_0;
		}
		char data[DATA_LENGTH+1];
		memset(data,enqueue_char_lower, DATA_LENGTH);
		data[DATA_LENGTH] = '\0';
		enqueue_char_lower++;
		enqueue_buffer_421_syscall(data);
	}
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		char buf[DATA_LENGTH+1];
		memset(buf, ' ', DATA_LENGTH);
		buf[DATA_LENGTH] = '\0';
		dequeue_buffer_421_syscall(buf);
	}
	
	return NULL;
}

int main(int argc, char *argv[]){
	
	init_buffer_421_syscall();
	
	
	pthread_t p,c;
	
	pthread_create(&p, NULL, &producer, NULL);
	pthread_create(&c, NULL, &consumer, NULL);
	pthread_join(p, NULL);
	pthread_join(c, NULL);
	
	delete_buffer_421_syscall();
	
	printf("Check dmesg\n");

	return 0;
}
