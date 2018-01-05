// I implementate the FIFO queue with a linked list to hold the
// items which holds pointers to the first and list items to allow fast
// FIFO operations.
// I used semphores set the size of a queue and allow of blocking and
// used a mutex as a lock so only one thread can access the queue at a
// time.
// The code is easy enough to follow without comments, only the error
// checking on system calls and function obscuring the code. 

#include "queue.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct NodeStruct {
	void* item;
	struct NodeStruct* next;
} Node;

typedef struct QueueStruct {
	pthread_mutex_t lock;
    sem_t get;
    sem_t put;
	Node *head;
	Node *tail;
} Queue;


Queue *queue_alloc(int size) {
    Queue* queue = malloc(sizeof(Queue));
    if (queue == NULL)
		handle_error("malloc: allocating queue failed");
	
	int err;
    if ((err = pthread_mutex_init(&queue->lock, NULL)) != 0)
		handle_error_en(err, "pthread_mutex_init: failed");
		
    if (sem_init(&queue->get, 0, 0) == -1)
		handle_error_en(errno, "sem_init: failed");
    if (sem_init(&queue->put, 0, size) == -1)
		handle_error_en(errno, "sem_init: failed");
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void queue_free(Queue *queue) {
    while (queue->head != NULL) {
		Node* temp = queue->head;
		queue->head = queue->head->next;
		free(temp);
	}
	
	int err;
    if ((err = pthread_mutex_destroy(&queue->lock)) != 0)
		handle_error_en(err, "pthread_mutex_destroy: failed");
	
	if (sem_destroy(&queue->get) == -1)
		handle_error_en(errno, "sem_close: failed");
	if (sem_destroy(&queue->put) == -1)
		handle_error_en(errno, "sem_close: failed");
		
	free(queue);
}

void queue_put(Queue *queue, void *item) {
	Node* node = malloc(sizeof(Node));
	if (node == NULL)
		handle_error("malloc: allocating node failed");
	
	node->item = item;
	node->next = NULL;
	int err;
	
	if (sem_wait(&queue->put) == -1)
		handle_error_en(errno, "sem_wait: failed");
	
	if ((err = pthread_mutex_lock(&queue->lock)) != 0)
		handle_error_en(err, "pthread_mutex_lock: failed");
	
	if (queue->head == NULL) { // queue is len 0
		queue->head = node;
		queue->tail = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}	
	
	if ((err = pthread_mutex_unlock(&queue->lock)) != 0)
		handle_error_en(err, "pthread_mutex_unlock: failed");
	
	if (sem_post(&queue->get) == -1)
		handle_error_en(errno, "sem_post: failed");
}

void *queue_get(Queue *queue) {
	Node* node = NULL;
	int err;
	
	if (sem_wait(&queue->get) == -1)
		handle_error_en(errno, "sem_wait: failed");
	
	if ((err = pthread_mutex_lock(&queue->lock)) != 0)
		handle_error_en(err, "pthread_mutex_lock: failed");
	
	node = queue->head;
	queue->head = queue->head->next;
	if (queue->head == NULL) { // if queue was len 1
		queue->tail = NULL;
	}

	if ((err = pthread_mutex_unlock(&queue->lock)) != 0)
		handle_error_en(err, "pthread_mutex_unlock: failed");
	
	if (sem_post(&queue->put) == -1)
		handle_error_en(errno, "sem_post: failed");
	
	void* item = node->item;
	free(node);

	return item;
}
