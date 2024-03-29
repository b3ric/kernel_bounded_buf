#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#include "buffer.h"
#include "utils.h"

static ring_buffer_t buffer;
static sem_t mutex;
static sem_t fill_count;
static sem_t empty_count;

long init_buffer(void) {
	
	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printf("init_buffer(): Buffer already exists. Aborting.\n");
		return -1;
	}

	// Create the root node.
	node_t *node;
	node = (node_t *) malloc(sizeof(node_t));
	// Create the rest of the nodes, linking them all together.
	node_t *current;
	int i;
	current = node;
	// Note that we've already created one node, so i = 1.
	for (i = 1; i < SIZE_OF_BUFFER; i++) {
		current->next = (node_t *) malloc(sizeof(node_t));
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

long enqueue_buffer(char * data) {	
	
	// Safety check
	if (!buffer.write) {
		printf("write_buffer(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	
	// Acquiring locks
	sem_wait(&empty_count);
	sem_wait(&mutex);
	
	printf(":: Enqueueing element into buffer. ::\n");
	printf("%.*s...\n", PRINT_N, data);
	
	// Write to buffer
	memcpy(buffer.write->data, data, DATA_LENGTH);
	
	// Adjust pointer and length
	buffer.write = buffer.write->next;
	buffer.length++;
	
	print_semaphores();
	printf("Size of buffer is %d\n", buffer.length);
	
	// Releasing locks
	sem_post(&mutex);
	sem_post(&fill_count);	
	
	return 0;
}

long dequeue_buffer(char * data) {
	
	// Safety check
	if (!buffer.read) {
		printf("delete_buffer(): The buffer does not exist. Aborting.\n");
		return -1;
	}
				
	// Acquiring locks
	sem_wait(&fill_count);
	sem_wait(&mutex);
	
	printf(":: Dequeueing element into buffer. ::\n");
	
	// Consuming data from buffer 
	memcpy(data , buffer.read->data , DATA_LENGTH);
	printf("%.*s...\n", PRINT_N, data);
	
	// Adjust pointer and length
	buffer.read = buffer.read->next;
	buffer.length--;
	
	print_semaphores();
	printf("Size of buffer is %d\n", buffer.length);
	
	// Releasing locks
	sem_post(&mutex);
	sem_post(&empty_count);
	
	return 0;
}

long delete_buffer(void) {
	
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printf("delete_buffer(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	
	// Get rid of all existing nodes.
	node_t *temp;
	node_t *current = buffer.read->next;
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
	
	// Destroy semaphores
	sem_destroy(&empty_count);
	sem_destroy(&fill_count);
	sem_destroy(&mutex);
	
	return 0;
}

void print_semaphores(void) {
	int value;
	sem_getvalue(&mutex, &value);
	printf("sem_t mutex = %d\n", value);
	sem_getvalue(&fill_count, &value);
	printf("sem_t fill_count = %d\n", value);
	sem_getvalue(&empty_count, &value);
	printf("sem_t empty_count = %d\n", value);
	return;
}