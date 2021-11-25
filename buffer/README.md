# Project 3: UMBCTube and the Bounded Buffer
### Group members:
- ERIC BAPTISTA - ebaptis1@umbc.edu
- JASON NACHMAN - jnachma1@umbc.edu

Project 3 aims to solve the producer-consumer problem using a circular buffer and semaphores. The following will describe the approach taken in order to provide a circular buffer available to concurrent consumer and producer threads in both kernel (system calls) and user-space(functions).

### Make and run:
#### User space
```c
cd proj3/buffer/tests/user_space/
make test
./test
```
#### Kernel space
```c
cd proj3/buffer/tests/kernel_space/
make test
./test
```
---
## Buffer Initialization
We first run a safety check to ensure we're not initializing an already existing buffer. We do so by checking whether either pointers ```read``` or ```write``` exist.

We initialize two nodes of type ```node_421_t```:
```c
node_421_t *node;
node_421_t *curr;
```
Node ```*node``` is created by allocating memory to it. We then point ```*curr``` to ```*node```.

Now we're ready to iterate ```SIZE_OF_BUFFER``` times and create the initial linked list. Finally, we proceed to make the linked list circular. 

We also initialize the three semaphores as such:
```c
sema_init(&mutex, 1);
sema_init(&empty_count, SIZE_OF_BUFFER);
sema_init(&fill_count, 0);
```
## Producer
The producer is represented by ```SYSCALL_DEFINE1(enqueue_buffer_421, char * ,data)``` in kernel-space, and ```long enqueue_buffer_421(char * data)``` in user-space. Apart from minor syntactical differences, both have the same approach. 

After running a safety check to ensure the buffer in fact exists, we are ready to use the locking mechanisms offered by ```semaphore.h``` as such: 
```c
// Acquire locks
sem_wait(&empty_count);
sem_wait(&mutex);		

    /*

    1. Enqueue data into buffer
    2. Adjust write pointer
    3. Adjust length
    4. Print current state of semaphores for testing purposes (user-space only)
 
    */

// Release locks 
sem_post(&mutex);
sem_post(&fill_count);	
```
## Consumer
The consumer is represented by ```SYSCALL_DEFINE1(dequeue_buffer_421,char *,data)``` in kernel-space, and ```long dequeue_buffer_421(char * data)``` in user-space. Apart from minor syntactical differences, both have the same approach. 

After running a safety check to ensure the buffer in fact exists, we are ready to use the locking mechanisms offered by ```semaphore.h``` as such: 
```c
// Acquire locks
down(&fill_count);
down(&mutex);

	/*

    1. Dequeue data from buffer to user
    2. Consumer buffer
    3. Adjust read pointer
    4. Adjust length 
    5. Print current state of semaphores for testing purposes (user-space only)
    
    */
	
// Release locks
up(&mutex);
up(&empty_count);
```
## Deleting the Buffer
After running a safety check to ensure the buffer in fact exists, we run a while loop and free the memory allocated one-by-one. We reset the buffer pointers to ```NULL``` and its length to ```0```. Finally, we destroy all three semaphores previously initialized. 

## Testing
#### User Buffer Testing
```c
cd proj3/buffer/tests/user_space/
make test
./test
```
#### Kernel Buffer Testing
```c
cd proj3/buffer/tests/kernel_space/
make test
./test
```

We first define some constants:
```c
#define LOOP_COUNT 100000
#define RAND_MOD 10 /* Modulus for rand() */
#define ASCII_0 48 /* Representation of '0' */
#define ASCII_9 57 /* Representation of '9' */
```
We then define the ```producer``` and ```consumer``` routines:

- Producer
     - Initialize ```enq_char``` to ```48``` (ASCII for '0')
     - Loop ```LOOP_COUNT``` times 
       - Sleep for ```rand() % RAND_MOD``` milliseconds  
       - Check if ```enq_char``` has passed '9', if so, go back to '0'
       - Declare the data buffer with enough space for the null terminator
       ```c
       char data[DATA_LENGTH + 1];
       ```
       - Fill the buffer with ```enq_char```, ```DATA_LENGTH``` bytes. 
       ```c
       memset(data, enq_char, DATA_LENGTH);
       ```
       - Null terminate ```data```
       - Call ```enqueue_buffer_421``` passing ```data``` to it

- Consumer
     - Loop ```LOOP_COUNT``` times 
       - Sleep for ```rand() % RAND_MOD``` milliseconds  
       - Check if ```enq_char``` has passed '9', if so, go back to '0'
       - Declare the data buffer with enough space for the null terminator
         ```c
         char data[DATA_LENGTH + 1];
         ```
       - Fill the buffer with empty char ```' '```, ```DATA_LENGTH``` bytes. 
         ```c
         memset(data, ' ', DATA_LENGTH);
         ```
       - Null terminate ```data```
       - Call ```dequeue_buffer_421``` passing ```data``` to it

- Main
     - Initialize the buffer
     - Declare two pthreads, one for ```producer```, one for ```consumer```, as such: 
         ```c
         pthread_t p,c;
         ```
     - Create the threads, then join them:
        ```c
        pthread_create(&p, NULL, &producer, NULL);
	    pthread_create(&c, NULL, &consumer, NULL);
	
	    pthread_join(p, NULL);
	    pthread_join(c, NULL);
        ```
     - Delete the buffer

Please note: When testing the kernel-space implementations, we are calling the actual system calls instead of functions.