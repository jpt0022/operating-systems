#ifndef _NODE_H_
#define _NODE_H_
/*
 *NODE HEADER FILE
 *
 *@author - Jessica Tomas - jpt0022
 *@date - Novemeber 18, 2019
 * 
 */
typedef unsigned int u_int;


struct taskNode {
        u_int pid;
        u_int arr_time;
        u_int burst_time;
        u_int wait_time;
        u_int response_time;
        u_int ta_time;
        u_int remaining_time;
        u_int start_time;
        u_int finish_time;
        struct taskNode *next;
};
typedef struct taskNode *Node;

/*Prepend
 * adds node to begining of list creating list
 */
void prepend(Node *head, Node node);
/*CreateNode
 * creates a node with input amounts
 */
Node createNode(u_int pid, u_int arr_time, u_int burst_time);
/*Insert
 * Inserts Node to end of list pointed to by pointer
 */
void insert(Node *head, Node task);
/*RemoveNode
 * removes given node from list
 */
void removeNode(Node *head, Node task);
/*findNode
 *finds node process is given
 */
Node findNode(Node *head, u_int pid);








#endif /*_CLOCK_H_ */
