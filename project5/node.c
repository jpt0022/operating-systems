/*
 *node.c
 *source code for node implementation
 *
 * @author - Jessica Tomas - jpt0022
 * @date - Novemeber 18, 2019 
 */


#include "node.h"
#include <stdio.h>
#include <stdlib.h>


Node head;

int temp_remaining;
int temp_start;
int temp_finish;


/*
 *
 *Prepend - Adds node to front of linked list
 *
 *@params - Node *head  - pointer to head of desired list
 *@         Node task - process desired to be added
 */
void
prepend(Node *head, Node task) {
        task->next = *head;
	*head = task;
        }

/*
 *CreateNode - creates a task node of a process with process information
 *
 *@params - process id, arrival time, burst time
 *
 *@return - returns the node created
 */
Node
createNode(u_int pid, u_int arr_time, u_int burst_time) {
        Node task;
        task =(Node) malloc(sizeof(struct taskNode));
        if(task == NULL)        {
                return NULL;
        }
        task->pid = pid;
        task->arr_time = arr_time;
        task->burst_time = burst_time;
        task->next = NULL;
        if (head == NULL)       {
                prepend(&head, task);
        }
        if (temp_remaining > 0) {
                task->remaining_time = temp_remaining;
        }
        if (temp_start > 0)     {
                task->start_time = temp_start;
        }
        if (temp_finish > 0)    {
                task->finish_time = temp_finish;
        }


        return task;
}



/*
 *Insert Node - inserts a node to the end of list
 *
 *@params - Node *leader - pointer to head of desired list
 *      Node newNode - node desired to be inserted
 *
 *@return - no return
 */
void
insert(Node *leader, Node newNode)    {

        Node tmp = *leader;
        newNode->next = NULL;
        if (leader == NULL)      {
                prepend(leader, newNode);
                return;
        }
        else    {

                while(tmp->next)       {
                        tmp = tmp->next;
                }
                tmp->next = newNode;
        }

}


/*
 *RemomveNode - removes node from list
 *
 *@params - Node *leader - pointer to head of list
 *      NOde task - task desired to be removed
 *
 * @return - no return
 */
void
removeNode(Node *leader, Node task)  {
        Node temp = *leader;
        //if removing first element - mostly fcfs
        if(temp->pid == task->pid)      {
		
                *leader = task->next;
                free(task);
		 
       }
        else    {
                while(temp->next != NULL)       {
                        /*found element*/
                        if (temp->next->pid == task->pid)       {
                                temp->next = temp->next->next;
                                free(task);
                                return;
                        }
                        temp = temp->next;
                }
        }
}

/*
 *findNode - finds a node given the process ID
 *
 *@params - Node *leader - list to search for node in
 *      Process ID - ID of task user is looking for
 *
 * @return - returns node containing the process ID
 */
Node
findNode(Node *leader, u_int id)        {
        Node found;
        found = *leader;
        while (found !=NULL)    {
                if (found->pid == id)   {
                        return found;
                }
                found = found->next;
        }
        printf("Cannot find process");
        return;
}


                       

