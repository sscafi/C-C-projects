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
int present_level = 0;
int quant_chP[NUMBER_OF_LEVELS-1];

// Ready process queues and waiting process queues
Process_queue queue_go[NUMBER_OF_LEVELS];
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
	time_clock = 0;
	t_arounds = 0;
	queue_tSize = 0;
	present_level= 0;
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
 	double result = (util_z * 100.0) / time_clock;
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
	Process *uno = (Process *) a;
	Process *dos = (Process *) b;
	return uno->arrivalTime - dos->arrivalTime;
}
/**
 * Compare process ID of two processes
 */
int compareProcessIds(const void *a, const void *b){
	Process *uno = (Process *) a;
	Process *dos = (Process *) b;
	if (uno->pid == dos->pid){
		error_duplicate_pid(uno->pid);
	}
	return uno->pid - dos->pid;
}
/**
 * Compare priorities of two processes
 */
int comparePriority(const void *a, const void *b){
	Process *uno = (Process *) a;
	Process *dos = (Process *) b;
	if (uno->priority == dos->priority){
		return compareProcessIds(uno, dos);
	}
	return uno->priority - dos->priority;
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
 * Dispatcher. Will grab the next appropriate process to be run
 * based on the availability in their ready queues.
 */
Process *next_p_s(void){
	int queue_goSize1 = queue_go[0].size;
	int queue_goSize2 = queue_go[1].size;
	int queue_goSize3 = queue_go[2].size;
	Process *get_next;
	
	if (queue_goSize1 != 0){
		get_next = queue_go[0].front->data;
		dequeueProcess(&queue_go[0]);
	}
	else if (queue_goSize2 != 0){
		get_next = queue_go[1].front->data;
		dequeueProcess(&queue_go[1]);
	} 
	else if (queue_goSize3 != 0){
		get_next = queue_go[2].front->data;
		dequeueProcess(&queue_go[2]);
	}
	return get_next; 
}
/**
 * Add any new incoming processes to a temporary queue to be sorted and later added
 * to the ready queue. These incoming processes are put in a "pre-ready queue"
 */
void new_process_add(void){
	while(nextP < numbP && processes[nextP].arrivalTime <= time_clock){
		queue_t[queue_tSize] = &processes[nextP];
		queue_tSize++;
		nextP++;
	}
}
/**
 * Get the uno process in the waiting queue, check if their I/O burst is complete.
 * If the current I/O burst is complete, move on to next I/O burst and add the process
 * to the "pre-ready queue". Dequeue the waiting queue and update waiting state by 
 * incrementing the current burst's step. Assign priority to 0, and time quantum to 0.
 */
void wait_ready(void){
 	int i;
 	int que_wZ = queue_wait.size;
 	for(i = 0; i < que_wZ; i++){
 		Process *get_next = queue_wait.front->data;
 		get_next->priority = 0;
 		get_next->quantumRemaining = 0;
 		dequeueProcess(&queue_wait);
 		if(get_next->bursts[get_next->currentBurst].step == get_next->bursts[get_next->currentBurst].length){
 			get_next->currentBurst++;
 			get_next->endTime = time_clock;
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
 		enqueueProcess(&queue_go[0], queue_t[i]);
 	}
	queue_tSize = 0;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		printf("");
 		if (cpu_all[i] == NULL){
 			cpu_all[i] = next_p_s();
 		}
 	}
 }
/**
 * Check all cases; uno that the uno time slice hasn't expired, if it has then move
 * process to the dos level. Run the process on that level, if the time slice expires
 * then move it to the fcfs part of the algorithm. There it will operate as a regular fcfs
 * algorithm, processing each process in a uno come uno serve manner.
 */
void run_and_wait(void){
	int queue_goSize1 = queue_go[0].size;
	int queue_goSize2 = queue_go[1].size;
	int queue_goSize3 = queue_go[2].size;
 	int i;
 	
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (cpu_all[i] != NULL){
 			if (cpu_all[i]->bursts[cpu_all[i]->currentBurst].step != cpu_all[i]->bursts[cpu_all[i]->currentBurst].length 
 				&& cpu_all[i]->quantumRemaining != quant_chP[0] && cpu_all[i]->priority == 0){
 				cpu_all[i]->quantumRemaining++;
 				cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 			}
 			else if(cpu_all[i]->bursts[cpu_all[i]->currentBurst].step != cpu_all[i]->bursts[cpu_all[i]->currentBurst].length 
 				&& cpu_all[i]->quantumRemaining == quant_chP[0] && cpu_all[i]->priority == 0){
 				cpu_all[i]->quantumRemaining = 0;
 				cpu_all[i]->priority = 1;
 				t_c_switch++;
 				enqueueProcess(&queue_go[1], cpu_all[i]);
 				cpu_all[i] = NULL;
 			}
 			else if(cpu_all[i]->bursts[cpu_all[i]->currentBurst].step != cpu_all[i]->bursts[cpu_all[i]->currentBurst].length 
 				&& cpu_all[i]->quantumRemaining != quant_chP[1] && cpu_all[i]->priority == 1){
 				if (queue_goSize1 != 0){
 					cpu_all[i]->quantumRemaining = 0;
 					t_c_switch++;
 					enqueueProcess(&queue_go[1], cpu_all[i]);
 					cpu_all[i] = NULL;
 				}
 				else{
 					cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 					cpu_all[i]->quantumRemaining++;
 				}
 			}
 			else if(cpu_all[i]->bursts[cpu_all[i]->currentBurst].step != cpu_all[i]->bursts[cpu_all[i]->currentBurst].length 
 				&& cpu_all[i]->quantumRemaining == quant_chP[1] && cpu_all[i]->priority == 1){
 				cpu_all[i]->quantumRemaining = 0;
 				cpu_all[i]->priority = 2;
 				t_c_switch++;
 				enqueueProcess(&queue_go[2], cpu_all[i]);
 				cpu_all[i] = NULL;
 			}
 			else if(cpu_all[i]->bursts[cpu_all[i]->currentBurst].step != cpu_all[i]->bursts[cpu_all[i]->currentBurst].length 
 				&& cpu_all[i]->priority == 2){
 				if (queue_goSize1 != 0 || queue_goSize2 != 0){
 					cpu_all[i]->quantumRemaining = 0;
 					t_c_switch++;
 					enqueueProcess(&queue_go[2], cpu_all[i]);
 					cpu_all[i] = NULL;
 				}
 				else{
 					cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 				}
 			}
 			// fcfs part of the algorithm
 			else if (cpu_all[i]->bursts[cpu_all[i]->currentBurst].step == cpu_all[i]->bursts[cpu_all[i]->currentBurst].length){
 				cpu_all[i]->currentBurst++;
 				cpu_all[i]->quantumRemaining = 0;
 				cpu_all[i]->priority = 0;
 				if(cpu_all[i]->currentBurst < cpu_all[i]->numOfBursts){
 					enqueueProcess(&queue_wait, cpu_all[i]);
 				}
 				else{
 					cpu_all[i]->endTime = time_clock;
 				}
 				cpu_all[i] = NULL;
 			}
 		}
 		// if the CPU is free, assign it work
 		else if (cpu_all[i] == NULL){
 			if(queue_goSize1 != 0){
 				Process *get_next = queue_go[0].front->data;
 				dequeueProcess(&queue_go[0]);
 				cpu_all[i] = get_next;
 				cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 				cpu_all[i]->quantumRemaining++;
			}
			else if(queue_goSize2 != 0){
 				Process *get_next = queue_go[1].front->data;
 				dequeueProcess(&queue_go[1]);
 				cpu_all[i] = get_next;
 				cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 				cpu_all[i]->quantumRemaining++;
 			}
 			else if(queue_goSize3 != 0){
 				Process *get_next = queue_go[2].front->data;
 				dequeueProcess(&queue_go[2]);
 				cpu_all[i] = get_next;
 				cpu_all[i]->bursts[cpu_all[i]->currentBurst].step++;
 				cpu_all[i]->quantumRemaining = 0;
 			}
 		}	
 	}
 }
 /**
 * Function to update waiting processes, ready processes, and running processes
 */
 void state_update(void){
 	int i,j;
 	int que_wZ = queue_wait.size;
 	// update waiting state
 	for (i = 0; i < que_wZ; i++){
 		Process *get_next = queue_wait.front->data;
 		dequeueProcess(&queue_wait);
 		get_next->bursts[get_next->currentBurst].step++;
 		enqueueProcess(&queue_wait, get_next);
 	}
 	// update ready processes
 	for (i = 0; i < NUMBER_OF_LEVELS; i++){
 		for (j = 0; j < queue_go[i].size; i++){
 			Process *get_next = queue_go[i].front->data;
 			dequeueProcess(&queue_go[i]);
 			get_next->waitingTime++;
 			enqueueProcess(&queue_go[i], get_next);
 		}
 	}
 }
/**
 * Display results for average waiting time, average turnaround time, the time
 * the CPU finished all processes, average CPU utilization, number of context
 * switches, and the process ID of the last process to finish.
 */
void displayResults(float awt, float atat, int sim, float aut, int cs, int pids){
	printf("--------Three-Level-Feedback-Queue---------\n"
		"Average waiting time\t\t:%.2f units\n"
		"Average turnaround time\t\t:%.2f units\n"
		"Time CPU finished all processes\t:%d\n"
		"Average CPU utilization\t\t:%.1f%%\n"
		"Number of context Switces\t:%d\n" 
		"PID of last process to finish\t:%d\n"
		"----------------------------------------------\n", awt, atat, sim, aut, cs, pids);
}

int main(int argc, char *argv[]){
	int i;
	int status = 0;
	double ut, wt, tat;
	int lastPID;
	quant_chP[0] = atoi(argv[1]);
	quant_chP[1] = atoi(argv[2]);

	// input error handling
	if (argc > 3){
		printf("Incorrect number of arguments, only add two time slices.\n");
		exit(-1);
	}
	else if (argc < 3){
		printf("Must add two time slices.\n");
		exit(-1);
	}

	// clear CPU'S, initialize queues, and reset global variables
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		cpu_all[i] = NULL;
	}
	for (i = 0; i < NUMBER_OF_LEVELS; ++i){
		initializeProcessQueue(&queue_go[i]);
	}
	initializeProcessQueue(&queue_wait);
	hard_reset();

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
		
		ready_process();
		run_and_wait();
		wait_ready();
		
		state_update();
		// break when there are no more running or incoming processes, and the waiting queue is empty
		if (process_to_run() == 0 && totalIncomingProcesses() == 0 && queue_wait.size == 0){
			break;
		}

		cput_util += process_to_run();
		time_clock++;
	}
	// calculations
	for(i = 0; i < numbP; i++){
		t_arounds +=processes[i].endTime - processes[i].arrivalTime;
		waitTT += processes[i].waitingTime;

		if (processes[i].endTime == time_clock){
			lastPID = processes[i].pid;
		}
	}

	wt = averageWaitTime(waitTT);
	tat = averageTurnaroundTime(t_arounds);
	ut = averageUtilizationTime(cput_util);

	displayResults(wt, tat, time_clock, ut, t_c_switch, lastPID);
	
	return 0;
}