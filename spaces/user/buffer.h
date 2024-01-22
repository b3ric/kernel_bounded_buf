#ifndef BUFFER_H
#define BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KERNEL__
#include <semaphore.h>
#else
#include <linux/semaphore.h>
#endif

#define SIZE_OF_BUFFER 20
#define DATA_LENGTH 1024

	typedef struct node {
		    struct node *next;
		        char data[DATA_LENGTH];
	} node_t;

	typedef struct ring_buffer {
		    int length;
		        node_t *read;
			    node_t *write;
	} ring_buffer_t;

#ifndef __KERNEL__
	long init_buffer(void);
	long enqueue_buffer(char *data);
	long dequeue_buffer(char *data);
	long delete_buffer(void);
	void print_semaphores(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
