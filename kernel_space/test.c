#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define __NR_init_buffer_421 442
#define __NR_enqueue_buffer_421 443
#define __NR_dequeue_buffer_421 444
#define __NR_delete_buffer_421 445

#define LOOP_COUNT 10
#define RAND_MOD 100
#define DATA_LENGTH 1024

long init_buffer_421_syscall(void){
	return syscall(__NR_init_buffer_421);
}

long enqueue_buffer_421_syscall(char * data){
	return syscall(__NR_enqueue_buffer_421, data);
}

long dequeue_buffer_421_syscall(char * data){
	return syscall(__NR_print_buffer_421);
}

long delete_buffer_421_syscall(void){
	return syscall(__NR_delete_buffer_421);
}

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_MOD);
		enqueue_buffer_421_syscall(prep_string());
		//print_semaphores();
		printf("============================\n");
	}
	
	return NULL;
}

void * consumer(void *args){
	
	int i;

	for (i = 0; i < LOOP_COUNT; i++){
		char buffer[DATA_LENGTH] = "";
		usleep(rand() % RAND_MOD);
		dequeue_buffer_421_syscall(buffer);
		//print_semaphores();
		printf("============================\n");
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