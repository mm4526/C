#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

#define LIST_SIZE 100000

typedef struct __node_t 
{
	int key;
	struct __node_t *next;
} node_t;

typedef struct __list_t
{
	node_t *head;
	pthread_mutex_t lock;
} list_t;

void listInitialize(list_t *L)
{
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

void listInsert(list_t *L, int key)
{
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL)
	{
		perror("malloc");
		return;
	}
	new->key = rand();
	pthread_mutex_lock(&L->lock);
	new->next = L->head;
	L->head = new;
	pthread_mutex_unlock(&L->lock);
}

int listLookup(list_t *L, int key)
{
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	node_t *current = L->head;
	while(current)
	{
		if (current->key == key)
		{
			rv = 0;
			break;
		}
		current = current->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv;
}

void * operation1(void *L)
{
	printf("\t-> Operation 1: begin (thread id: %ld)\n", pthread_self());
	for (int i = 0; i < (LIST_SIZE/2); i++)
	{
		int randKey = rand();
		listInsert(L, randKey);
	}
	printf("\tOperation 1: end (thread id: %ld)\n", pthread_self());
	return NULL;
}

void * operation2Ins(void *L)
{
	printf("\t-> Operation 2 Insert: begin (thread id: %ld)\n", pthread_self());
	for (int i = 0; i < (LIST_SIZE); i++)
	{
		int randKey = rand();
		listInsert(L, randKey);
	}
	printf("\tOperation 2 Insert: end (thread id: %ld)\n", pthread_self());
	return NULL;
}

void * operation2Look(void *L)
{
	printf("\t-> Operation 2 Lookup: begin (thread id: %ld)\n", pthread_self());
	for (int i = 0; i < (LIST_SIZE); i++)
	{
		int randKey = rand();
		listLookup(L, randKey);
	}
	printf("\tOperation 2 Lookup: end (thread id: %ld)\n", pthread_self());
	return NULL;
}

void * operation3(void *L)
{
	printf("\t-> Operation 3: begin (thread id: %ld)\n", pthread_self());
	for (int i = 0; i < (LIST_SIZE); i++)
	{
		int randKey = rand();
		listLookup(L, randKey);
	}
	printf("\tOperation 3: end (thread id: %ld)\n", pthread_self());
	return NULL;
}

int main(int argc, char *argv[])
{
	printf("main: begin (thread id = %ld)\n", pthread_self());

	struct timeval start1, start2, start3;
	struct timeval end1, end2, end3;
	long t1_microsecs, t2_microsecs, t3_microsecs;

	srand(time(0));

	list_t L;
	listInitialize(&L);
	list_t L2;
	listInitialize(&L2);

	pthread_t p1, p2;

	gettimeofday(&start1, NULL);
	pthread_create(&p1, NULL, operation1, &L);
	pthread_create(&p2, NULL, operation1, &L);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	gettimeofday(&end1, NULL);

	t1_microsecs = ((end1.tv_sec*1e6 + end1.tv_usec) - (start1.tv_sec*1e6 + start1.tv_usec));
	printf("\n\tTime of Op1: %ld microseconds\n\n", t1_microsecs);

	gettimeofday(&start2, NULL);
	pthread_create(&p1, NULL, operation2Ins, &L);
	pthread_create(&p2, NULL, operation2Look, &L);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	gettimeofday(&end2, NULL);

	t2_microsecs = ((end2.tv_sec*1e6 + end2.tv_usec) - (start2.tv_sec*1e6 + start2.tv_usec));
	printf("\n\tTime of Op2: %ld microseconds\n\n", t2_microsecs);

	gettimeofday(&start3, NULL);
	pthread_create(&p1, NULL, operation3, &L);
	pthread_create(&p2, NULL, operation3, &L);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	gettimeofday(&end3, NULL);

	t3_microsecs = ((end3.tv_sec*1e6 + end3.tv_usec) - (start3.tv_sec*1e6 + start3.tv_usec));
	printf("\n\tTime of Op3: %ld microseconds\n\n", t3_microsecs);

	printf("main: end (thread id = %ld)\n", pthread_self());
}