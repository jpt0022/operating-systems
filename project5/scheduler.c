/* CPU SCHEDULER
 * scheduler.c 
 * simulates a cpu scheduler for FCFS, RR, and SRTF policies 
 * prints statistical informaion after running policies
 *
 * @author - Jessica Tomas - jpt0022
 * 
 * @date - November 17, 2019
 *
 */

/*IF U CANNOT FINISH ANYTHING ELSE ATLEAST DO MAKEFILE!!!!*/


/*
 *
 * INCLUDES & DEFINES
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#define MAX_TASK_NUM 32

/*
 * 
 *VARIABLES & STRUCTS
 *
 */
char filename[100];

//typedef unsigned int u_int;
typedef unsigned int usage_t;


typedef struct stat_info_t      {
        double avg_wait_time;
        double avg_response_time;
        double avg_ta_time;
        usage_t cpu_usage;
} stat_info_t;

u_int count;
time_t clock;
u_int total_sim_time;
u_int total_burst_time;
u_int quantum;
u_int run_time = 0;


Node temp;
Node head = NULL;
Node readyqueue = NULL;
Node future_task_list = NULL;
Node finish_task_list = NULL;
Node *rqhead = NULL;
Node *futurehead = NULL;
Node *finishhead = NULL;


stat_info_t runstats;

u_int temp_pid;
u_int temp_arrival;
u_int temp_burst;


int temp_remaining = 0;
int temp_start = 0;
int temp_finish = 0;


bool taskrunning;
bool new_arrival;



/*
 *PROTOTYPES
 *
 *
 */


int read_file(char filename[]);
void fcfs_policy();
void rr_policy(u_int quantum);
void srtf_policy();
void print_stat_info();


/*
 *MAIN 
 *@params -	int argc - stores number of command line aruments passed
 *		argv[] - array of char pointers listing all args
 *    
 */
int
main(int argc, char *argv[])    {

        if(argc == 1 || argc > 4)       {
                printf("Usage: command file_name [FCFS|RR|SRFT] [time_quantum]\n");
        return 0;
        }

        /*Print policy*/
        printf("Scheduling policy: %s\n", argv[2]);

        /*Handle the RR policy*/
        if (strcmp(argv[2], "RR") == 0) {
                if (argc == 4)
                        printf("Time quantum is set to %s\n", argv[3]);
                else printf("Please enter time_quantum for the RR policy!\n");
        }
//	int test = 0;
	if (argc == 4)	{
		quantum = atoi(argv[3]);
	
	}	

//	printf("time quantum: %u\n", quantum);
        strcpy(filename, argv[1]);

        read_file(filename);
        //printf("ok this?");
	printf("=========================================================\n");
	//printf("what about now\n");
        if (strcmp(argv[2], "FCFS") == 0)       {

                fcfs_policy();
        }
        else if (strcmp(argv[2], "SRTF") == 0)  {
                srtf_policy(2);
        }
	else if(strcmp(argv[2], "RR") == 0)	{
		//printf("and again\n");	
		rr_policy(quantum);	
	}

        printf("=========================================================\n");

        print_stat_info(finishhead);
        printf("=========================================================\n");
        return 0;

}



       

/*
 *  READ INPUT FILE
 *
 * @params - char file[] that holds a filename
 * 
 */
int
read_file(char file[])   {
        
	FILE *ptr;

	u_int i;

	if(! (ptr = fopen(file, "r")))	{
		printf("File %s can't be opened.\n", file);
		return EXIT_FAILURE;		
	}
	
//	printf("Open file: %s\n", file);
        
	count = 0;
		
	while(fscanf(ptr, "%u %u %u", &temp_pid, &temp_arrival, &temp_burst) != EOF)     {
		if (count == 0)	{
			createNode(temp_pid, temp_arrival, temp_burst);
		}
		else	{
			insert(&head, createNode(temp_pid, temp_arrival, temp_burst));
		}
		count++;
	}		
	printf("There are %u tasks loaded from \"%s\". ", count, file);	
	printf("Press any key to continue...\n");
	getchar();
	i = 0;

       	fclose(ptr);
	return 0;
}




/*
 *FCFS (First come first serve)
 *
 *simulates fcfs scheduling policy
 *@params - none
 *@return - no return, updates all task list and statistic variables
 *
 */

void
fcfs_policy()	{
	//figure out how to add to ready queue
	clock = 0;
	
	
	future_task_list = head;
	futurehead = &future_task_list;
	finishhead = &finish_task_list;

	while(future_task_list != NULL  || readyqueue != NULL  )	{
		printf("<time %u> ", clock);
	
	
		/*handle new arrivals*/

		while(future_task_list != NULL)	{
	
			if(future_task_list->arr_time <= clock)	{	//if task has arrived arrival time is <=  clock time
				Node futuretemp = *futurehead;
					
				temp_remaining = futuretemp->burst_time;//set remaining time	
					
				if (readyqueue == NULL)	{	//add to readyqueue
					
					prepend(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));
					
											
				}
				else	{
					insert(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));			
				
	
				}
				
				removeNode(futurehead, future_task_list); // remove node from future into ready
			

			}
			else break;
			
		}	//end inside while
//		
//		run first task in ready queue		
		rqhead = &readyqueue;
		taskrunning = false;
		if (!taskrunning)	{	//only first process should enter
			temp = *rqhead;
			taskrunning = true;
			if (temp->remaining_time == temp->burst_time)	{	//process just started, time left == total time
				temp->start_time = clock;
				
			}	
		}
		if (temp->remaining_time != 0)	{
		printf("process %u is running\n", temp->pid);
		}
 
		
		else {                  //(temp->remaining_time == 0)   if process is done
                        temp->finish_time = clock;      //set finish time
                        temp_finish = temp->finish_time;
                        temp_start = temp->start_time;
                        printf("process %u is finished...\n", temp->pid);
                        taskrunning = false;
                         if (finish_task_list == NULL) {        //at to finish_task_list
                                prepend(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time));

                        }
                         else    {
                                                insert(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time) );


                        }

                        removeNode(rqhead, readyqueue);

                        if (readyqueue == NULL) {
                                printf("<time %u> All processes finished ......\n", clock);
                        	total_sim_time = clock;
			}
                        else    {       //start next task
                                temp = *rqhead;
                                taskrunning = true;
                                temp->start_time = clock;
                                printf("<time %u> process %u is running\n", clock, temp->pid);
                        }

                }///end of if statement for if process is done
			
		temp->remaining_time--;
		clock++;

	
	} 	

}


/*
 *RR policy
 *
 *@params - quantum time specified by user
 *
 * @return - no return but prints scheduling policy timestamp
 */
void
rr_policy(u_int quantum)	{

        clock = 0;

        future_task_list = head;
        futurehead = &future_task_list;
        finishhead = &finish_task_list;
	run_time = 0;	
//		rqhead = &readyqueue;
//	temp = *rqhead;
	
        while(future_task_list != NULL  || readyqueue != NULL  )        {
                printf("<time %u>", clock);
                
		/*handle new arrivals*/
                while(future_task_list != NULL) {
                        if(future_task_list->arr_time <= clock) {       //if task has arrived arrival time is <=  clock time
                                Node futuretemp = *futurehead;
				new_arrival = true;
                                temp_remaining = futuretemp->burst_time;//set remaining time
                                if (readyqueue == NULL) {       //add to readyqueue
                                        prepend(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));
        				 rqhead = &readyqueue;
        				 temp = *rqhead;
	
	                        }
		
                                else    {
                              	        insert(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));
                                }
                                removeNode(futurehead, future_task_list); // remove node from future into ready

                        }
                        else break;

                }       //end inside while
		
		
				
		if (run_time == quantum)	{
; 
			
			if (temp->next != NULL)	{
				temp = temp->next;
			}
			else	{
				temp = *rqhead;
			}
			run_time = 0;
		}	
	
                if (temp->remaining_time == temp->burst_time)   {       //process just started, time left == total time
                	
		        temp->start_time = clock;
                }

		if (temp->remaining_time != 0)  {
                printf(" process %u is running\n", temp->pid);
                }


                else {                  //(temp->remaining_time == 0)   if process is done
                        temp->finish_time = clock;      //set finish time
                        temp_finish = temp->finish_time;
                        temp_start = temp->start_time;
                        printf(" process %u is finished...\n", temp->pid);
                        
                         if (finish_task_list == NULL) {        //at to finish_task_list
                                prepend(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time));

                        }
                         else    {
                                                insert(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time) );
                        }
                                                                    

			removeNode(rqhead, findNode(rqhead, temp->pid));
                	
		        if (readyqueue == NULL) {
                                printf("<time %u> All processes finished ......\n", clock);
                                total_sim_time = clock;
                        }
                        else    {       //start next task
                                
				temp = *rqhead;
                                temp->start_time = clock;
                                printf("<time %u> process %u is running\n", clock, temp->pid);
			//	printf("\t%u time left\n", temp->remaining_time);
				run_time = 0;
                        }

                }///end of if statement for if process is done
		if (temp->remaining_time != 0)	{
			temp->remaining_time--;
		}
		run_time++;
                clock++;

	}
}









/*
 *SRFT - shortest remainig time first
 *
 *@params - no params
 *@return - no return, updates all task lists and statistics variables
 */
void
srtf_policy()	{
	clock = 0;
 	

        future_task_list = head;
        rqhead = &readyqueue;
	futurehead = &future_task_list;
        finishhead = &finish_task_list;

        while(future_task_list != NULL  || readyqueue != NULL  )        {
	
 		printf("<time %u> ", clock);
//              handle new arrivals
		while(future_task_list != NULL) {
	
 	               	if(future_task_list->arr_time <= clock) {       //if task has arrived arrival time is <=  clock time
                      		new_arrival = true;
				Node futuretemp = *futurehead;
                       		temp_remaining = futuretemp->burst_time;//set remaining time
			   	if (readyqueue == NULL) {       //add to readyqueue
  	                	     	prepend(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));
                    			
				}	
		 		else	{    
					insert(&readyqueue, createNode(futuretemp->pid, futuretemp->arr_time, futuretemp->burst_time));
        	        	}
                		removeNode(futurehead, future_task_list); // remove node from future into ready
			}          
                	else break;
		}
	
		/*begin running*/
		Node rqrun = *rqhead;
		//temp = *rqhead;
		// if theres a new arrival find ready task with shortest remaining time to run
		if (new_arrival)	{	//run through list with rqrun to find what to run and set temp  to it
			temp = rqrun;
		
		
			while(rqrun->next != NULL)	{
		
		
				if (rqrun->remaining_time > rqrun->next->remaining_time)	{
            				temp = rqrun->next;
				}
				rqrun = rqrun->next;
			}
		}
	
	        if (temp->remaining_time == temp->burst_time)   {       //process just started, time left == total time
		        temp->start_time = clock;
        	}
	
		if (temp->remaining_time != 0)  {
        	        printf("process %u is running\n", temp->pid);
        	}

		else {                  //(temp->remaining_time == 0)   if process is done
        		temp->finish_time = clock;      //set finish time
                	temp_finish = temp->finish_time;
	                temp_start = temp->start_time;
       		        printf("process %u is finished...\n", temp->pid);
               		taskrunning = false;
          		if (finish_task_list == NULL) {        //at to finish_task_list
                		prepend(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time));
		
                        }
                	else    {
                        	insert(&finish_task_list, createNode(temp->pid, temp->arr_time, temp->burst_time) );

                	}	
		
		
                	Node *rmptr;
		
			rmptr = &temp;
		
			removeNode(&readyqueue, findNode(rmptr, temp->pid));
                	if (readyqueue == NULL) {
                		printf("<time %u> All processes finished ......\n", clock);
                		total_sim_time = clock;
			}
                	else    {       //start next task
                		rqrun = *rqhead;
				temp = rqrun;
        			while(rqrun->next != NULL)      {
	                 		if (rqrun->remaining_time > rqrun->next->remaining_time)        {
                                        temp = rqrun->next;
                        		}
					rqrun = rqrun->next;
                		}	

                	}

       		}
		temp->remaining_time--;
		clock++;

	}
}





/*
 *Print statistic information
 *
 *@params - Node pointer to head of list
 *@return - no return, prints all statistical information about simulation
 *
 *
 */
void
print_stat_info(Node *leader)	{
	Node temp = *leader;		
	
	while (temp != NULL)	{
		
		temp->response_time = temp->start_time - temp->arr_time;

		temp->ta_time = temp->finish_time - temp->arr_time;
		
		temp->wait_time = temp->ta_time - temp->burst_time;		

//		printf("Pid: %u\nstart: %u\tarrive: %u\tresponse: %u\nfinish: %u\tta: %u\nburst: %u\twait: %u\n", temp->pid, temp->start_time, temp->arr_time, temp->response_time, temp->finish_time, temp->ta_time, temp->burst_time, temp->wait_time);
	//	printf("process %u burst is %u\n", temp->pid, temp->burst_time);
		total_burst_time += temp->burst_time;
			
		runstats.avg_response_time += temp->response_time;
		runstats.avg_ta_time += temp->ta_time;		
		runstats.avg_wait_time += temp->wait_time;
		
		temp = temp->next;
	}

	runstats.avg_wait_time = runstats.avg_wait_time / count;
	runstats.avg_response_time = runstats.avg_response_time / count;
	runstats.avg_ta_time = runstats.avg_ta_time / count;
	runstats.cpu_usage = (total_burst_time / total_sim_time) * 100;

//	printf("Total burst time: %u\n", total_burst_time);
//	printf("Total sim time: %u\n", total_sim_time);

	printf("Average waiting time: \t\t%.2f\n", runstats.avg_wait_time);
	printf("Average response time: \t\t%.2f\n", runstats.avg_response_time);
	printf("Average turnaround time: \t%.2f\n", runstats.avg_ta_time);
	printf("Overall CPU usage: \t\t%u%\n", runstats.cpu_usage);
}


