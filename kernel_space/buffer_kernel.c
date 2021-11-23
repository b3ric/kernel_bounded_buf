#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/semaphore.h>

#include "buffer.h"
#include "utils.h"

static ring_buffer_421_t buffer;

struct semaphore mutex;
struct semaphore fill_count;
struct semaphore empty_count;

SYSCALL_DEFINE0(init_buffer_421){
	
	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printk("init_buffer_421(): Buffer already exists. Aborting.\n");
		return -1;
	}

	// Create the root node.
	node_421_t *node;
	node = (node_421_t *) kmalloc(sizeof(node_421_t));
	// Create the rest of the nodes, linking them all together.
	node_421_t *current;
	int i;
	current = node;
	// Note that we've already created one node, so i = 1.
	for (i = 1; i < SIZE_OF_BUFFER; i++) {
		current->next = (node_421_t *) kmalloc(sizeof(node_421_t));
		current = current->next;
	}
	// Complete the chain.
	current->next = node;
	buffer.read = node;
	buffer.write = node;
	buffer.length = 0;

	// Initialize your semaphores here.
	sema_init(&mutex, 1);
    sema_init(&empty_count, SIZE_OF_BUFFER);
    sema_init(&fill_count, 0);

	return 0;
}

SYSCALL_DEFINE1(enqueue_buffer_421, char * ,data){	
	
	if (!buffer.write) {
		printk("write_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
		
	down(&empty_count);
	down(&mutex);
	
	printk(":: Enqueueing element into buffer. ::\n");
	printk("%.*s...\n", PRINT_N, data);
	
	memcpy(buffer.write->data, data, DATA_LENGTH);
	buffer.write = buffer.write->next;
	buffer.length++;
	
	printk("Size of buffer is %d\n", buffer.length);
	
	up(&mutex);
	up(&fill_count);	
	
	return 0;
}

SYSCALL_DEFINE1(dequeue_buffer_421,char *,data){
	
	if (!buffer.read) {
		printk("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
				
	down(&fill_count);
	down(&mutex);
	
	printk(":: Dequeueing element into buffer. ::\n");
	//printk("%s \n", buffer.read->data);
	
	memcpy(data,buffer.read->data, DATA_LENGTH);
	
	printk("%.*s...\n", PRINT_N, data);
	
	buffer.read = buffer.read->next;
	buffer.length--;
	printk("Size of buffer is %d\n", buffer.length);
	
	up(&mutex);
	up(&empty_count);
	
	return 0;
}

SYSCALL_DEFINE0(delete_buffer_421){
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printk("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	// Get rid of all existing nodes.
	node_421_t *temp;
	node_421_t *current = buffer.read->next;
	while (current != buffer.read) {
		temp = current->next;
		kfree(current);
		current = temp;
	}
	// Free the final node.
	kfree(current);
	current = NULL;
	// Reset the buffer.
	buffer.read = NULL;
	buffer.write = NULL;
	buffer.length = 0;
	
	return 0;
}
