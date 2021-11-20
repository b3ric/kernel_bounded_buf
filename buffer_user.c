#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_COUNT 2
#define LOOP_COUNT 100000
#define RAND_SEED 100

static ring_buffer_421_t buffer;
static sem_t mutex;
static sem_t fill_count;
static sem_t empty_count;
static int enqueue_char = 48; // ASCII for '0'

void print_buffer_421(void);
void * producer(void *args);
void * consumer(void *args);
char* prep_string(void);

long init_buffer_421(void) {
	
	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printf("init_buffer_421(): Buffer already exists. Aborting.\n");
		return -1;
	}

	// Create the root node.
	node_421_t *node;
	node = (node_421_t *) malloc(sizeof(node_421_t));
	// Create the rest of the nodes, linking them all together.
	node_421_t *current;
	int i;
	current = node;
	// Note that we've already created one node, so i = 1.
	for (i = 1; i < SIZE_OF_BUFFER; i++) {
		current->next = (node_421_t *) malloc(sizeof(node_421_t));
		current = current->next;
	}
	// Complete the chain.
	current->next = node;
	buffer.read = node;
	buffer.write = node;
	buffer.length = 0;

	// Initialize your semaphores here.
	sem_init(&mutex, 0, 1);
    sem_init(&empty_count, 0, SIZE_OF_BUFFER);
    sem_init(&fill_count, 0, 0);

	return 0;
}

long enqueue_buffer_421(char * data) {	
	
	if (!buffer.write) {
		printf("write_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	

	usleep(rand() % RAND_SEED);
	
	sem_wait(&empty_count);
	sem_wait(&mutex);
	
	printf(":: Enqueueing element into buffer. ::\n");
	printf("%s\n", data);
	
	memcpy(buffer.write->data, data, DATA_LENGTH);
	buffer.write = buffer.write->next;
	buffer.length++;
	
	printf("Size of buffer is %d\n\n", buffer.length);
	
	sem_post(&mutex);
	sem_post(&fill_count);	
	
	return 0;
}

long dequeue_buffer_421(char * data) {
	
	if (!buffer.read) {
		printf("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
			
	usleep(rand() % RAND_SEED);
	
	sem_wait(&fill_count);
	sem_wait(&mutex);
	
	printf(":: Dequeueing element into buffer. ::\n");
	
	memcpy(data,buffer.read->data, DATA_LENGTH);
	printf("%s\n", data);
	buffer.read = buffer.read->next;
	buffer.length--;
	
	printf("Size of buffer is %d\n\n", buffer.length);
	
	sem_post(&mutex);
	sem_post(&empty_count);
	
	return 0;
}

long delete_buffer_421(void) {
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printf("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	// Get rid of all existing nodes.
	node_421_t *temp;
	node_421_t *current = buffer.read->next;
	while (current != buffer.read) {
		temp = current->next;
		free(current);
		current = temp;
	}
	// Free the final node.
	free(current);
	current = NULL;
	// Reset the buffer.
	buffer.read = NULL;
	buffer.write = NULL;
	buffer.length = 0;
	return 0;
}

void print_semaphores(void) {
	// You can call this method to check the status of the semaphores.
	// Don't forget to initialize them first!
	// YOU DO NOT NEED TO IMPLEMENT THIS FOR KERNEL SPACE.
	int value;
	sem_getvalue(&mutex, &value);
	printf("sem_t mutex = %d\n", value);
	sem_getvalue(&fill_count, &value);
	printf("sem_t fill_count = %d\n", value);
	sem_getvalue(&empty_count, &value);
	printf("sem_t empty_count = %d\n", value);
	return;
}

#define ASCII_0 48
#define ASCII_9 57
char * prep_string(void){
	
	static char enqueue_str[DATA_LENGTH];
	int i;
	
	for (i = 0; i < DATA_LENGTH; i++){
		enqueue_str[i] = enqueue_char;
	}
	
	enqueue_str[DATA_LENGTH] = 0;
	enqueue_char++;
	
	if (enqueue_char > ASCII_9) {
		enqueue_char = ASCII_0;
	}
	
	return enqueue_str;
}

void print_buffer_421(void){
	int i;
	node_421_t *temp = buffer.read;
	
	for (i = 0; i < SIZE_OF_BUFFER; i++){
		printf("Address is %p  ||| Value is %s\n", temp, temp->data);
		temp = temp->next;
	}
	
	printf("Read points to %p\n", buffer.read);
	printf("Write points to %p\n\n", buffer.write);
}

void * producer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_SEED);
		enqueue_buffer_421(prep_string());
	}
	
	return NULL;
}

void * consumer(void *args){
	
	int i;
	
	for (i = 0; i < LOOP_COUNT; i++){
		usleep(rand() % RAND_SEED);
		char dequeue[DATA_LENGTH];
		dequeue_buffer_421(dequeue);
	}
	
	return NULL;
}

int main(void)
{
	init_buffer_421();

	pthread_t p,c;
	
	pthread_create(&p, NULL, &producer, NULL);
	pthread_create(&c, NULL, &consumer, NULL);
	pthread_join(p, NULL);
	pthread_join(c, NULL);
	
	sem_destroy(&empty_count);
	sem_destroy(&fill_count);
	sem_destroy(&mutex);
	
	delete_buffer_421();
	
	return 0;
}