#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#include "buffer.h"

static ring_buffer_421_t buffer;
const int PRINT_N = 10;

struct semaphore mutex;
struct semaphore fill_count;
struct semaphore empty_count;

SYSCALL_DEFINE0(init_buffer_421){
	
	// root node
	node_421_t *node;
	// remaining nodes
	node_421_t *curr;
	int i;

	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printk("init_buffer_421(): Buffer already exists. Aborting.\n");
		return -1;
	}

	node = (node_421_t *) kmalloc(sizeof(node_421_t), GFP_KERNEL);
	
	curr = node;
	// Note that we've already created one node, so i = 1.
	for (i = 1; i < SIZE_OF_BUFFER; i++) {
		curr->next = (node_421_t *) kmalloc(sizeof(node_421_t), GFP_KERNEL);
		curr = curr->next;
	}
	// Complete the chain.
	curr->next = node;
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

	// Safety check
	if (!buffer.write) {
		printk("write_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	
	// Acquire locks
	down(&empty_count);
	down(&mutex);
	
	printk(":: Enqueueing element into buffer. ::\n");
	printk("%.*s...\n", PRINT_N, data);
	
	// Write to buffer
	if(copy_from_user(buffer.write->data, data, DATA_LENGTH)){
		printk("Something went wrong with <copy_from_user> ...\n");
		return -EFAULT;
	}
	
	// Adjust pointer and length
	buffer.write = buffer.write->next;
	buffer.length++;
	
	printk("Size of buffer is %d\n", buffer.length);
	
	// Release locks
	up(&mutex);
	up(&fill_count);	
	
	return 0;
}

SYSCALL_DEFINE1(dequeue_buffer_421,char *,data){
	
	// Safety check
	if (!buffer.read) {
		printk("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
				
	// Acquire locks
	down(&fill_count);
	down(&mutex);
	
	printk(":: Dequeueing element into buffer. ::\n");
	
	// Copy from buffer to user 
	if(copy_to_user(data,buffer.read->data, DATA_LENGTH)){
		printk("Somethig went wrong with <copy_to_user> ...\n");
		return -EFAULT;
	}
	
	printk("%.*s...\n", PRINT_N, data);
	
	// Adjust pointer and length
	buffer.read = buffer.read->next;
	buffer.length--;
	printk("Size of buffer is %d\n", buffer.length);
	
	// Release locks
	up(&mutex);
	up(&empty_count);
	
	return 0;
}

SYSCALL_DEFINE0(delete_buffer_421){
	
	node_421_t *temp;
	node_421_t *curr = buffer.read->next;

	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printk("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	// Get rid of all existing nodes.
	while (curr != buffer.read) {
		temp = curr->next;
		kfree(curr);
		curr = temp;
	}
	// Free the final node.
	kfree(curr);
	curr = NULL;
	// Reset the buffer.
	buffer.read = NULL;
	buffer.write = NULL;
	buffer.length = 0;
	
	return 0;
}
