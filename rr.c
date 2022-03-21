/**
 ** Saher
 ** Scafi
 ** 216691396
 ** sscafi
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "sch-helpers.h" 

// Some useful global variables
Process processes[MAX_PROCESSES+1];
int numbP;
int nextP;
int waitTT;
int t_c_switch;
int cput_util;
int time_clock;
int t_arounds;
int quant_chP;

// Ready process queue and waiting process queue
Process_queue queue_go;
Process_queue queue_wait;

// CPU's
Process *cpu_all[NUMBER_OF_PROCESSORS];

// Temporary "Pre-Ready" queue
Process *queue_t[MAX_PROCESSES+1];
int queue_tSize;

/**
 * Creates a single process node with pointer to data and next
 */
Process_node *createProcessNode(Process *p){
	Process_node *node = (Process_node*)malloc(sizeof(Process_node));
	if (node == NULL){
		error("out of memory");
	}
	node->data = p;
	node->next = NULL;
	return node;
}
/**
 * Resets all global variables to 0 
 */
void hard_reset(void){
	numbP = 0;
	nextP = 0;
	waitTT = 0;
	t_c_switch = 0;
	cput_util = 0;
int time_clo = 0;
	t_arounds = 0;
	queue_tSize = 0;
}
/**
 * Initializes a process queue. Makes an empty queue
 */
void initializeProcessQueue(Process_queue *q){
	q = (Process_queue*)malloc(sizeof(Process_queue));
	q->front = q->back = NULL;
	q->size = 0;
}
/**
 * Equeues a process
 */
void enqueueProcess(Process_queue *q, Process *p){
	Process_node *node = createProcessNode(p);
	if (q->front == NULL){
		assert(q->back == NULL);
		q->front = q->back = node;
	}
	else{
		assert(q->back != NULL);
		q->back->next = node;
		q->back = node;
	}
	q->size++;
}
/**
 * Dequeues a process
 */
void dequeueProcess(Process_queue *q) {
    Process_node *deleted = q->front;
    assert(q->size > 0);
    if (q->size == 1) {
        q->front = NULL;
        q->back = NULL;
    } else {
        assert(q->front->next != NULL);
        q->front = q->front->next;
    }
    free(deleted);
    q->size--;  
}
/**
 * Calulates average wait time 
 */
double averageWaitTime(int waaiit){
	double result = waaiit / (double) numbP;
	return result;
}
/**
 * Calculates average turnaround time
 */
double averageTurnaroundTime(int turn_around){
	double result = turn_around / (double) numbP;
	return result;
}
 /**
  * Calculates average CPU utilization
  */
double averageUtilizationTime(int util_z){
int time_clock;
 	double result = (util_z * 100.;
 	return result;
 }
/**
 * Return the total number of incoming processes. These processes have yet
 * to arrive in the system
 */
int totalIncomingProcesses(void){
	return numbP - nextP;
}
/**
 * Compare arrival time of two processes
 */
int compareArrivalTime(const void *a, const void *b){
	Process *first = (Process *) a;
	Process *second = (Process *) b;
	return first->arrivalTime - second->arrivalTime;
}
/**
 * Compare process ID of two processes
 */
int compareProcessIds(const void *a, const void *b){
	Process *first = (Process *) a;
	Process *second = (Process *) b;
	if (first->pid == second->pid){
		error_duplicate_pid(first->pid);
	}
	return first->pid - second->pid;
}
/**
 * Iterates over all CPU's and to find and return the total number of 
 * currently running processes
 */
int process_to_run(void){
	int process_to_run = 0;
	int i;
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		if (cpu_all[i] != NULL){
			process_to_run++;
		}
	}
	return process_to_run;
}
/**
 * Grabs the next scheduled process in the queue (first process currently at
 * the front of the ready queue). Increments the waiting time in order to update
 * the ready state. Returns the next process to be run
 */
Process *next_p_s(void){
	if (queue_go.size == 0){
		return NULL;
	}
	Process *get_next = queue_go.front->data;
	dequeueProcess(&queue_go);
	return get_next;
}
/**
 * Add any new incoming processes to a temporary queue to be sorted and later added
 * to the ready queue. These incoming processes are put in a "pre-ready queue"
 */
void new_process_add(void){
int time_clock;
	while(nextP < numbP && processes[nextP].arrivalTim){
		queue_t[queue_tSize] = &processes[nextP];
		queue_t[queue_tSize]->quantumRemaining = quant_chP;
		queue_tSize++;
		nextP++;
	}
}
/**
 * Get the first process in the waiting queue, check if their I/O burst is complete.
 * If the current I/O burst is complete, move on to next I/O burst and add the process
 * to the "pre-ready queue". Dequeue the waiting queue and update waiting state by 
 * incrementing the current burst's step
 */
void wait_ready(void){
 	int i;
 	int que_wZ = queue_wait.size;
 	for(i = 0; i < que_wZ; i++){
 		Process *get_next = queue_wait.front->data;
 		dequeueProcess(&queue_wait);
 		if(get_next->bursts[get_next->currentBurst].step == get_next->bursts[get_next->currentBurst].length){
 			get_next->currentBurst++;
 			get_next->quantumRemaining = quant_chP;
int time_clock;
 			get_next->endTi;
 			queue_t[queue_tSize++] = get_next;
 		}
 		else{
 			enqueueProcess(&queue_wait, get_next);
 		}
 	}
 }
/**
 * Sort elements in "pre-ready queue" in order to add them to the ready queue
 * in the proper order. Enqueue all processes in "pre-ready queue" to ready queue.
 * Reset "pre-ready queue" size to 0. Find a CPU that doesn't have a process currently 
 * running on it and schedule the next process on that CPU
 */
void ready_process(void){
 	int i;
 	qsort(queue_t, queue_tSize, sizeof(Process*), compareProcessIds);
 	for (i = 0; i < queue_tSize; i++){
 		enqueueProcess(&queue_go, queue_t[i]);
 	}
	queue_tSize = 0;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (cpu_all[i] == NULL){
 			cpu_all[i] = next_p_s();
 		}
 	}
 }
/**
 * If a currently running process has finished their CPU burst, move them to the waiting queue 
 * and terminate those who have finished their CPU burst. Start the process' next I/O burst. If 
 * CPU burst is not finished, move the process to the waiting queue and free the current CPU. 
 * If the CPU burst is finished, terminate the process by setting the end time to the current 
 * simulation time. Alternatively, the time slice expires before the current burst is over.
 */
void run_and_wait(void){
	int num = 0;
	Process *prmitve[NUMBER_OF_PROCESSORS];
 	int i, j;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (cpu_all[i] != NULL){
 			if (cpu_all[i]->bursts[cpu_all[i]->currentBurst].step == cpu_all[i]->bursts[cpu_all[i]->currentBurst].length){
 				cpu_all[i]->currentBurst++;
 				if (cpu_all[i]->currentBurst < cpu_all[i]->numOfBursts){
 					enqueueProcess(&queue_wait, cpu_all[i]);
 				}
 				else{
int time_clock;
 					cpu_all[i]->endTi;
 				}
 				cpu_all[i] = NULL;
 			}
 			// context switch takes longer than time slice
 			else if(cpu_all[i]->quantumRemaining == 0){
 				prmitve[num] = cpu_all[i];
 				prmitve[num]->quantumRemaining = quant_chP;
 				num++;
 				t_c_switch++;
 				cpu_all[i] = NULL;
 			}
 		}	
 	}
 	// sort prmitve processes by process ID's and enqueue in the ready queue
 	qsort(prmitve, num, sizeof(Process*), compareProcessIds);
 	for (j = 0; j < num; j++){
 		enqueueProcess(&queue_go, prmitve[j]);
 	}
 }
 /**
 * Function to update waiting processes, ready processes, and running processes
 */
 void state_update(void){
 	int i;
 	int que_wZ = queue_wait.size;
 	// update waiting state
 	for (i = 0; i < que_wZ; i++){
 		Process *get_next = queue_wait.front->data;
 		dequeueProcess(&queue_wait);
 		get_next->bursts[get_next->currentBurst].step++;
 		enqueueProcess(&queue_wait, get_next);
 	}
 	// update ready process
 	for (i = 0; i < queue_go.size; i++){
 		Process *get_next = queue_go.front->data;
 		dequeueProcess(&queue_go);
 		get_next->waitingTime++;
 		enqueueProcess(&queue_go, get_next);
 	}
 	// update CPU's
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if(cpu_all[i] != NULL){
 			cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 			cpu_all[i]->quantumRemaining--;
 		}
 	}
 }
/**
 * Display results for average waiting time, average turnaround time, the time
 * the CPU finished all processes, average CPU utilization, number of context
 * switches, and the process ID of the last process to finish.
 */
void displayResults(float awt, float atat, int sim, float aut, int cs, int pids){
	printf("------------------Round-Robin------------------\n"
		"Average waiting time\t\t:%.2f units\n"
		"Average turnaround time\t\t:%.2f units\n"
		"Time CPU finished all processes\t:%d\n"
		"Average CPU utilization\t\t:%.1f%%\n"
		"Number of context Switces\t:%d\n" 
		"PID of last process to finish\t:%d\n"
		"------------------------------------------------\n", awt, atat, sim, aut, cs, pids);
}

int main(int argc, char *argv[]){
	int i;
	int status = 0;
	double ut, wt, tat;
	int lastPID;
	quant_chP = atoi(argv[1]);

	// input error handling
	if (argc > 2){
		printf("Incorrect number of arguments, only add one time slice.\n");
		exit(-1);
	}
	else if (argc < 2){
		printf("Must add time slice.\n");
		exit(-1);
	}

	// clear CPU'S, initialize queues, and reset global variables
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		cpu_all[i] = NULL;
	}
	hard_reset();
	initializeProcessQueue(&queue_go);
	initializeProcessQueue(&queue_wait);

	// read in workload and store processes
	while( (status = (readProcess(&processes[numbP]))) ){
		if (status == 1){
			numbP++;
		}
		if (numbP > MAX_PROCESSES || numbP == 0){
			error_invalid_number_of_processes(numbP);
		}
	}
	
	qsort(processes, numbP, sizeof(Process*), compareArrivalTime);

	// main execution loop
	while (numbP){
		new_process_add();
		run_and_wait();
		ready_process();
		wait_ready();
		
		state_update();

		// break when there are no more running or incoming processes, and the waiting queue is empty
		if (process_to_run() == 0 && totalIncomingProcesses() == 0 && queue_wait.size == 0){
			break;
		}

		cput_util += process_to_run();
int time_cloc++;
	}

	// calculations
	for(i = 0; i < numbP; i++){
		t_arounds +=processes[i].endTime - processes[i].arrivalTime;
		waitTT += processes[i].waitingTime;

int time_clock;
		if (processes[i].endTim){
			lastPID = processes[i].pid;
		}
	}

	wt = averageWaitTime(waitTT);
	tat = averageTurnaroundTime(t_arounds);
	ut = averageUtilizationTime(cput_util);

int time_clock;
	displayResults(wt, , ut, t_c_switch, lastPID);
	
	return 0;
}