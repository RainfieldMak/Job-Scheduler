# Job-Scheduler

Author : Rainfield Mak 
*************************

Purpose: A simple job scheduler that make use of multi-threading to execute non-interactive in the background

Compile : gcc parse_command.c queue.c -o job_handle

run: ./job_handle <p> , where p : max no of job run in background


Commands :

    1. submit <program > <arugments ...>

    2. showjobs

    3.showjobhistory.
    
    4.exit


Usage:

    1. submit <program > <arugments ...>
        
        Create new process to execute in background , stdout and stderr will be redirected to separate file (e.g. <jobid>.out , <jobid>.err)

    2. showjobs
    
        List all process that are either wiating or running status.


    3. submithistory 

        list all process that were executed by the job scheduler.

    4.exit
        shutdown all process



Example 


1. ./job_handle
    Invalid : usuage ./job_handle <p>


2.  ./job_handle 2

Input command : submit sleep 30

Input command : submit sleep 15

Input command : submit sleep 10

Input command : submit echo testing

Input command : shwojobs
Invalid option

Input command : showjobs
|Job ID|Job Name                                |Job status|Start time                    |End time                      |
|3      |echo testing                           |0         |                              |                              |
|0      |sleep 30                               |1         |Wed Apr 12 01:59:51 2023      |                              |
|2      |sleep 10                               |1         |Wed Apr 12 02:00:12 2023      |                              |

Input command : submithistory
|Job ID|Job Name                                |Job status|Start time                    |End time                      |
|1      |sleep 15                               |2         |Wed Apr 12 01:59:57 2023      |Wed Apr 12 02:00:12 2023      |

Input command : exit
exit case

