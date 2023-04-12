/* Header file for the simple circular queue example */
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>

typedef struct _job
{
	int id;
	char *command;
	char *start_time;
	char *end_time;
	int 	status;



}job;

typedef struct _queue {
	int size;    /* maximum size of the queue */
	job **buffer; /* queue buffer */
	int start;   /* index to the start of the queue */
	int end;     /* index to the end of the queue */
	int count;   /* no. of elements in the queue */
} queue;


typedef struct _info
{

	int total;
	queue  *waiting_q;
	queue *finish_q;
	job **running_q;
	int p;
	 pthread_t *pid;

}info;


job *job_init (char* command, int id);
void job_cpy(job *dest, job *src );
info *info_init( int argc);


queue *queue_init(int n);
int queue_insert(queue *q, job *item);
job *queue_delete(queue *q);
void queue_display(queue *q);
void show_run_job(job **running_q, int length);
void queue_destroy(queue *q);
int parse_command (char* input);
int transfer_waiting_to_running(job **running_q, queue *waiting_q, int argc );
int p_popen(char* command, int job_id);
int empty_slot(job **running_q, queue *waiting_q, int argc );
void *call (void *ptr) ;
void *loop (void *ptr);

#endif
