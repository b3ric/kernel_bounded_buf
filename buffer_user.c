#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define THREAD_COUNT 8

static ring_buffer_421_t buffer;
static sem_t mutex;
static sem_t fill_count;
static sem_t empty_count;

long init_buffer_421(void) {
	
	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printf("init_buffer_421(): Buffer already exists. Aborting.\n");
		return -1;
	}
	
	// Initializing semaphores.
	
	sem_init(&mutex, 0, 1);
    sem_init(&empty_count, 0, SIZE_OF_BUFFER);
    sem_init(&fill_count, 0, 0);

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
	// TODO

	return 0;
}

long enqueue_buffer_421(char * data) {
	// NOTE: You have to modify this function to use semaphores.
	if (!buffer.write) {
		printf("write_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	
	print_buffer_421();
	print_semaphores();
	
	sem_wait(&empty_count);
	sem_wait(&mutex);
	
	memcpy(buffer.write->data, data, DATA_LENGTH);
	// Advance the pointer.
	buffer.write = buffer.write->next;
	buffer.length++;
	
	sem_post(&mutex);
	sem_post(&fill_count);

	return 0;
}

long dequeue_buffer_421(char * data) {
	
	//
	if (!buffer.read) {
		printf("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	
	if (buffer.length == 0){
		return -1; // FOR NOW... LATER BLOCK USING SEMAPHORE
	}
		
	print_buffer_421();
	print_semaphores();
	
	sem_wait(&fill_count);
	sem_wait(&mutex);
	
	// Copies 1024 bytes from the read node into the provided buffer data.
	memcpy(data,buffer.read->data, DATA_LENGTH);
	
	// Correctly update the buffer's length and read pointer
	buffer.read = buffer.read->next;
	buffer.length--;
	
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

void print_buffer_421(void){
	int i;
	node_421_t *temp = buffer.read;
	
	for (i = 0; i < SIZE_OF_BUFFER; i++){
		printf("Address is %p  ||| Value is %s\n", temp, temp->data);
		temp = temp->next;
	}
	
	//printf("Read points to %p\n", buffer.read);
	//printf("Write points to %p\n", buffer.write);
}

int main(void)
{
	
	init_buffer_421();
	srand(time(NULL));
	pthread_t th[THREAD_COUNT];
	
	char dequeue[DATA_LENGTH];
	int i, time;
	
	const char *arr[SIZE_OF_BUFFER] = {"01", "02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20"};
	
	
	for (i = 0; i < THREAD_COUNT; i++) {
        if (i % 2 == 0) {
			// sleep
			time = rand() % 100;
			usleep(time);
            if (pthread_create(&th[i], NULL, &enqueue_buffer_421, arr[rand() % SIZE_OF_BUFFER]) != 0) {
                perror("Failed to create thread");
            }
        } else {
			//sleep here
			time = rand() % 100;
			usleep(time);
            if (pthread_create(&th[i], NULL, &dequeue_buffer_421, &dequeue) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    for (i = 0; i < THREAD_COUNT; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
	sem_destroy(&empty_count);
	sem_destroy(&fill_count);
	sem_destroy(&mutex);
	
	delete_buffer_421();
	
	return 0;
}