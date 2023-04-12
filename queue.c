/* Implementation of a simple circular queue using a static array */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>


pthread_mutex_t lock;

/* create the queue and job data structure and initialize it */

job *job_init (char* command, int id)
{
	job *j = (job*) malloc (sizeof (job)); 
	j->id= id;
	j->command=(char*) malloc (sizeof(char) * 100);
	strcpy(j->command , command);
	j->start_time=(char*) malloc (sizeof(char) * 100);
	j->end_time=(char*) malloc (sizeof(char) * 100);
	strcpy(j->start_time," " );
	strcpy(j->end_time," " );

	/*waiting =0, running=1 , success =2 , fail =-1 */
	j->status=0;

}

void job_cpy(job *dest, job *src )
{
	
	dest->id= src->id;
	strcpy(dest->command, src->command);
	strcpy(dest->start_time, src->start_time);
	strcpy(dest->end_time, src->end_time);
	dest->status= src->status;

}



info *info_init( int argc)
{
	info *all= (info *) malloc(sizeof(info));
	all->waiting_q = queue_init(100);
	all->finish_q= queue_init(100);
	all->running_q=(job **)malloc(sizeof(job*) * argc);
	all->total=0;
	all->p =argc;
	all->pid=( pthread_t *) malloc (sizeof ( pthread_t) * argc);
	for (int i =0; i < argc; i ++)
	{
		all->running_q[i]= job_init (" ", -1);
	}

	return all;
}	


queue *queue_init(int n) {
	queue *q = (queue *)malloc(sizeof(queue));
	q->size = n;
	q->buffer = (job**)malloc(sizeof(job*) *n);

	int i =0;
	for(i=0 ; i <n ; i ++)
	{
		q->buffer[i]= (job*) malloc (sizeof (job) ); 

	}

	q->start = 0;
	q->end = 0;
	q->count = 0;

	return q;
}

/* insert an item into the queue, update the pointers and count, and
   return the no. of items in the queue (-1 if queue is null or full) */
int queue_insert(queue *q, job *item) {
	if ((q == NULL) || (q->count == q->size))
	   return -1;

	q->buffer[q->end % q->size] = item;	
	q->end = (q->end + 1) % q->size;
	q->count++;

	return q->count;
}

/* delete an item from the queue, update the pointers and count, and 
   return the item deleted (-1 if queue is null or empty) */
job *queue_delete(queue *q) {
	if ((q == NULL) || (q->count == 0))
	   return NULL;

	job *j = q->buffer[q->start];
	q->start = (q->start + 1) % q->size;
	q->count--;

	return j;
}

/* display the contents of the queue data structure */
void queue_display(queue *q) {
	int i;
	char id[100]= "Job ID";
	char name[100]="Job Name";
	char status[100]="Job status";
	char s_time[100]= "Start time";
	char e_time [100]="End time";


	if (q != NULL && q->count != 0) {
		
		printf("|%-3s|%-40s|%-10s|%-30s|%-30s|\n", id,name,status,s_time, e_time);

		for (i = 0; i < q->count; i++)
		{
	    		printf("|%-7d|", q->buffer[(q->start + i) % q->size]->id);
			printf("%-39s|",q->buffer[(q->start + i) % q->size]->command);
			printf("%-10d|",q->buffer[(q->start + i) % q->size]->status);
			printf("%-30s|",q->buffer[(q->start + i) % q->size]->start_time);
			printf("%-30s|",q->buffer[(q->start + i) % q->size]->end_time);
			printf("\n");

		}
	}
}

/*display running job */
void show_run_job(job **running_q, int length)
{
	int i;
	for ( i=0; i < length;i ++)
	{
		if (running_q[i]->id != -1)
		{
				printf("|%-7d|",running_q[i]->id);
				printf("%-39s|",running_q[i]->command);
				printf("%-10d|",running_q[i]->status);
				printf("%-30s|",running_q[i]->start_time);
				printf("%-30s|",running_q[i]->end_time);
				printf("\n");
		}
	} 
}

/* delete the queue data structure */
void queue_destroy(queue *q) {
	free(q->buffer);
	free(q);
}



/*parse command and return as integer */
// 4 commands: submit (2), showjobs(0), submithistory(1), else (-1)
int parse_command (char* input)
{
	char temp[100];
	strcpy(temp , input);

	if ((strcmp(temp,"showjobs"))==0)
	{
		return 0;
	}else if ( (strstr(temp, "submit"))!= NULL )
	{

		if((strcmp(temp, "submithistory"))==0 )
		{
			return 1;
		}else if ( (strcmp( (strtok (temp ," ")), "submit" ))==0  )
		{
			return 2;
		
		}else
		{
			return -1;
		}

	}else if ((strcmp(temp,"exit"))==0)		
	{
		return 3;
	}else
	{
		return-1;
	}


}

/*return slot being copied to running q */
int transfer_waiting_to_running(job **running_q, queue *waiting_q, int argc )
{
	int i ;
	int copy=0;
	for (i=0; i < argc; i ++)
	{
		
		if (running_q[i]->id == -1 && waiting_q->count > 0)
		{
			job_cpy(running_q[i], queue_delete(waiting_q) )	;
			copy=1;
			break;	
		}
	}
	if (copy ==1)
	{
		return i;
	}else
	{
		return -1;
	}
	
}


int p_popen(char* command, int job_id)
{
	struct stat st;
	char out[100];
	FILE *f_out;

	/*combine command for redirect I/O */
	sprintf(out," %s > %d.out 2> %d.err",command, job_id, job_id);
	
	/*create pipe, fork and execvp */

	if ((f_out=popen(out, "w")) == NULL)
	{
		fprintf(stderr, "pipe or fork fail \n");
		pclose(f_out);
		return -1;
	
	}
	
	pclose(f_out);


	/*check if err file is empty*/
	sprintf(out, "./%d.err", job_id);
	stat(out, &st);
	if (st.st_size >0)
	{
		
		return -1;
	

	}else 
	{
		

		return 0;
	}

	
}



/*actual running thread i.e. thread that will call popen */

void *call(void *ptr) 
{
	int success;
	job *j;
	time_t curtime;
	info *all = (info *) ptr;
	
	pthread_mutex_lock(&lock);
	all->total++;
	pthread_mutex_unlock(&lock);

	int slot=transfer_waiting_to_running(all->running_q , all->waiting_q, all->p );
	
	
	if (slot != -1)
	{
			
		all->running_q[slot]->status=1;	
		time(&curtime);
		strcpy(all->running_q[slot]->start_time, strtok(ctime(&curtime), "\n") );
		

			
		if ((success=p_popen(all->running_q[slot]->command, all->running_q[slot]->id))==-1)
		{
		
			all->running_q[slot]->status=-1;
			time(&curtime);
			strcpy(all->running_q[slot]->end_time,strtok(ctime(&curtime), "\n") );
	


		}else
		{
	
			all->running_q[slot]->status=2;
			time(&curtime);
			strcpy(all->running_q[slot]->end_time,strtok(ctime(&curtime), "\n") );
	

		}

		
		job *temp=job_init(" ",-2);

		pthread_mutex_lock(&lock);
		job_cpy(temp, all->running_q[slot])	;
		


		if ( queue_insert(all->finish_q, temp) ==-1)	
		{
			write(1, "Insert fail : Finish que \n", strlen("Insert fail : Finish que \n"));
		}
		

		all->running_q[slot]->id=-1;
		pthread_mutex_unlock(&lock);

		

		pthread_mutex_lock(&lock);
		all->total--;
		pthread_mutex_unlock(&lock);
			
	}
			
}



/*thread function for looping in the background */
int empty_slot(job **running_q, queue *waiting_q, int argc )
{
	int i ;
	int copy=0;
	for (i=0; i < argc; i ++)
	{
		
		if (running_q[i]->id == -1 && waiting_q->count > 0)
		{
			
			copy=1;
			break;	
		}
	}
	if (copy ==1)
	{
		return i;
	}else
	{
		return -1;
	}
	
}



void *loop (void *ptr)
{

	int i;
	int slot_;
	info *all = (info *) ptr;
	while (1)
	{
		if ( (all->total < all->p)  || (all->waiting_q->count > 0))
		{
			//pthread_mutex_lock(&lock);
			slot_=empty_slot(all->running_q , all->waiting_q, all->p );
			
			//printf (" Slot_: %d \n", slot_);
			//pthread_mutex_unlock(&lock);



			if (slot_ != -1 && (all->waiting_q->count > 0) )
			{
			
	
				i = pthread_create(&all->pid[slot_],NULL, call , (void*) all);
				

			
			}
			
			
		}

	}


}


