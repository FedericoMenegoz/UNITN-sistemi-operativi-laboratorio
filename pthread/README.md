# [Counter](counter/main.c)
Write a C program that takes an integer N as a command line input.
The program defines a global integer variable count initialized to 0, and creates N threads. Each thread increments the global variable count and prints its own index followed by the new value of the variable. The index of the thread is determined as follows: the first thread created has index 0, the second 1, ..., the Nth has index N-1. \
The program must ensure that the last printed value of count is N.

# [Producer Consumer](producer_consumer/main.c)
Consider a circular character array (buffer) of size LEN = 20. The buffer is managed with a FIFO (First In First Out) strategy for insertions and extractions. The program creates 3 threads that operate concurrently on the array:

- Thread 1 (producer) performs the following operation LEN times, each time in mutual exclusion:
If the buffer is not full, it adds the character 'o' to the end; otherwise, it waits for a buffer cell to become free.
- Thread 2 behaves exactly like thread 1, but writes 'x' instead of 'o'.
- Thread 3 (consumer) performs the following operation 2*LEN times, each time in mutual exclusion: 
    1. if the buffer is not empty, it removes a character from the head of the buffer and prints it; 
    2. otherwise, it waits for the buffer to become non-empty.
 

Try to solve the problem using classical semaphores and/or condition variables.

# [Condition Variable](condition_variable/main.c)
Write a C program that uses three threads to manage a global integer variable count, initialized to zero.
The first thread waits for the value of count to become greater than or equal to THRESH=15, and when this happens, it prints the message "Threshold reached".
The other two threads perform the following operations 20 times in mutual exclusion:
1. Increment the count variable.
2. Print their own index and the updated value of count.
3. Appropriately notify the first thread. 


The program must be implemented using condition variables.