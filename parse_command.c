#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "queue.h"
#include <pthread.h>


pthread_mutex_t lock_m;

int main( int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Invalid : usuage %s <p> \n", argv[0]);
		exit(-1);
	}

	char* input;
	int exit_flag;
	int job_id;
	int i;
	int j;
	int side_c;
	int side_j;
	pthread_t side_pid;
	
	pthread_t background;

	exit_flag=0;
	job_id=0;
	j=0;
	input= (char*) malloc(sizeof (char) * 100);
	info *all = info_init(atoi(argv[1]));

	
	side_c= pthread_create(&side_pid,NULL, loop , (void*) all);
	while (exit_flag!=1)
	{
		printf("\nInput command : ");
		fgets (input, 99, stdin);
		char *token= strtok (input, "\n");
		//printf("%s \n" , token );

		

		switch ( parse_command (token)) 
		{
			/*showjobs*/
			case 0:
				
				queue_display(all->waiting_q);
				show_run_job(all->running_q, argc);

				break;

			/*job history */
			case 1:
				queue_display(all->finish_q);
				break;

			/*submit job */
			case 2:
				
				
				job *j= job_init(&token[7] ,job_id);
				

				/*insert job*/
				pthread_mutex_lock(&lock_m);
				if ( queue_insert(all->waiting_q, j) ==-1)	
				{
					printf("Insert waiting wrong \n");
				}
			
				pthread_mutex_unlock(&lock_m);
				
				job_id++;



				break;

			/*exit*/
			case 3:
				exit_flag=1;
				printf("exit case\n");
				break;

			case -1:
				printf("Invalid option\n");
				break;			
		
			default :
				printf("Invalid option\n");

		}

		sleep(1);
		

	}




	/* free varibale*/

	free(input);
	free(all);	
	return 0;


}
