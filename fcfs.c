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

//Here are the global variables necessary for the program to use 
Process processes[MAX_PROCESSES+1];
int number_of_processes =0 ;
int next_process = 0 ;
int total_wait_time = 0 ;
int context_switchs = 0 ;
int cpu_utilliz = 0 ;
int clock_t = 0 ;
int turn_arounds = 0 ;

// Ready and wait process queue , very important to instantiate these 
Process_queue queue_go;
Process_queue queue_wait;

// points to all the CPU's
Process *cpu_alls[NUMBER_OF_PROCESSORS];

// pointing towards the temporary  queue
Process *queue_t[MAX_PROCESSES+1];
int queue_tSize;

/**
 * initilizes a  single process node with pointer to data and next
 */
Process_node *node_create(Process *d){
	Process_node *node = (Process_node*)malloc(sizeof(Process_node));
	if (node == NULL){
		error(" HAAA you're out of memory");
	}
	node->data = d;
	node->next = NULL;
	return node;
}
/**
 * Resets all global variables back to  0 , otherwise processes will not run as expected.
 * with out this output is off
 */
void hard_reset(void){
	number_of_processes = 0;
	next_process = 0;
	total_wait_time = 0;
	context_switchs = 0;
	cpu_utilliz = 0;
	clock_t = 0;
	turn_arounds = 0;
	queue_tSize = 0;
}
/**
 *  This function to initialize the process queue. 
 */
void process_que_init(Process_queue *s){
	s = (Process_queue*)malloc(sizeof(Process_queue));
	s->front = s->back = NULL;
	s->size = 0;
}
/**
 *  This Function to start queing the process
 */
void que_start(Process_queue *s, Process *d){
	Process_node *node = node_create(d);
	if (s->front == NULL){
		assert(s->back == NULL);
		s->front = s->back = node;
	}
	else{
		assert(s->back != NULL);
		s->back->next = node;
		s->back = node;
	}
	q->size++;
}
/**
 * Function to stop queing the process
 */
void que_stop(Process_queue *s) {
    Process_node *deleted = q->front;
    assert(s->size > 0);
    if (s->size == 1) {
        s->front = NULL;
        s->back = NULL;
    } else {
        assert(s->front->next != NULL);
        s->front = s->front->next;
    }
    free(deleted);
    s->size--;  
}
/**
 * function to calculate the  average wait time 
 */
double avg_waittime(int waaiit){
	double result = waiit / (double) number_of_processes;
	return result;
}
/**
 * function to calculate the average turnaround time
 */
double avg_turnaround(int turn_around){
	double result = turn_around / (double) number_of_processes;
	return result;
}
 /**
  * function to calculate the  average CPU utilization
  */
double avg_utilliz(int util_z){
 	double result = (util_z * 100.0) / clock_t;
 	return result;
 }
/**
 * Function to return the total number of incoming processes
 */
int total_process_inc(void){
	return number_of_processes - next_process;
}
/**
 * function to compare the  arrival time of the two processes
 */
int compareArrivalTime(const void *s, const void *d){
	Process *uno = (Process *) s;
	Process *dos = (Process *) d;
	return uno->arrivalTime - dos->arrivalTime;
}
/**
 *  function to compare the  process ID's of the two processes
 */
int compareProcessIds(const void *s const void *d){
	Process *uno = (Process *) s;
	Process *dos = (Process *) d;
	if (uno->pid == dos->pid){
		error_duplicate_pid(uno->pid);
	}
	return uno->pid - dos->pid;
}
/**
 * function to iterate through all CPU's and to find and return the total number of 
 *  running processes
 */
int process_to_run(void){
	int process_to_run = 0;
	int i;
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		if (cpu_alls[i] != NULL){
			process_to_run++;
		}
	}
	return process_to_run;
}
/**
 * Function to grabs the next scheduled process in the queue
 * Increments the waiting time in order to update
 * the ready state then returns the next process to be run
 */
Process *next_p_s(void){
	if (queue_go.size == 0){
		return NULL;
	}
	Process *next_go = queue_go.front->data;
	que_stop(&queue_go);
	return next_go;
}
/**
 * function to add any new incoming processes to a the temporary queue so it may be sorted then later added
 * to the ready queue. These incoming processes are put in a "pre-ready queue"
 */
void new_process_add(void){
	while(next_process < number_of_processes && processes[next_process].arrivalTime <= clock_t){
		queue_t[queue_tSize++] = &processes[next_process++];
	}
}
/**
 *  Function that gets the first process in the waiting queue, checks if their I/O burst is completed.
 * then if  the current I/O burst is completed move on to the  next I/O burst and add the process
 * to the que temp place holder. Dequeueing the waiting state and update the  waiting state by 
 * incrementing the current burst's step
 */
void wait_ready(void){
 	int i;
 	int queue_waitSize = queue_wait.size;
 	for(i = 0; i < queue_waitSize; i++){
 		Process *next_go = queue_wait.front->data;
 		que_stop(&queue_wait);
 		if(next_go->bursts[next_go->io_burst].step == next_go->bursts[next_go->io_burst].length){
 			next_go->io_burst++;
 			queue_t[queue_tSize++] = next_go;
 		}
 		else{
 			que_start(&queue_wait, next_go);
 		}
 	}
 }
/**
 * Sort elements so that they are added to the ready queue in the correct order. All processes should be queued in the ready queue. 
 *  Find a CPU where no processes are currently running and schedule the next process to run on that CPU.
 */
void ready_process(void){
 	int i;
 	qsort(queue_t, queue_tSize, sizeof(Process*), compareProcessIds);
 	for (i = 0; i < queue_tSize; i++){
 		que_start(&queue_go, queue_t[i]);
 	}
	queue_tSize = 0;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (cpu_alls[i] == NULL){
 			cpu_alls[i] = next_p_s();
 		}
 	}
 }
/**
 *  Function to move a presently operating process to the waiting queue if it has completed its CPU burst, and terminate those who have completed their CPU burst.
 *  Begin the process's next I/O burst. Move the process to the waiting queue and liberate the current CPU if the CPU burst has not been completed.
 *  End the process by setting the end time to the current simulation time if the CPU burst is complete.
 */
void run_and_wait(void){
 	int i;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (cpu_alls[i] != NULL){
 			if (cpu_alls[i]->bursts[cpu_alls[i]->io_burst].step == cpu_alls[i]->bursts[cpu_alls[i]->io_burst].length){
 				cpu_alls[i]->io_burst++;
 				if (cpu_alls[i]->io_burst < cpu_alls[i]->numOfBursts){
 					que_start(&queue_wait, cpu_alls[i]);
 				}
 				else{
 					cpu_alls[i]->endTime = clock_t;
 				}
 				cpu_alls[i] = NULL;
 			}
 		}	
 	}
 }
 /**
 * Function to update:
 *  waiting processes
 *  ready processes
 *   running processes
 */
 void state_update(void){
 	int i;
 	int queue_waitSize = queue_wait.size;
 	// this updates the  waiting state as needed for the function
 	for (i = 0; i < queue_waitSize; i++){
 		Process *next_go = queue_wait.front->data;
 		que_stop(&queue_wait);
 		next_go->bursts[next_go->io_burst].step++;
 		que_start(&queue_wait, next_go);
 	}
 	// this updates the  ready process as needed for the function
 	for (i = 0; i < queue_go.size; i++){
 		Process *next_go = queue_go.front->data;
 		que_stop(&queue_go);
 		next_go->waitingTime++;
 		que_start(&queue_go, next_go);
 	}
 	// this updates the CPU's as needed for the function
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if(cpu_alls[i] != NULL){
 			cpu_alls[i]->bursts[cpu_alls[i]->io_burst].step++;
 		}
 	}
 }
/**
 * Display the average wait time, average turnaround time, time it has taken the CPU to complete all these processes, 
 * average CPU utilization, number of context switches, and the process ID of the most recently concluded process.
 */
void result_display(float awt, float atat, int sim, float aut, int cs, int pids){
	printf("First-Come-First-Serve\n"
		"Average waiting time\t\t:%.2f units\n"
		"Average turnaround time\t\t:%.2f units\n"
		"Time CPU finished all these  processes\t:%d\n"
		"Average CPU utilization\t\t:%.1f%%\n"
		"Number of context Switces\t:%d\n" 
		"Process ID of most recently concluded process\t:%d\n"
		"\n", awt, atat, sim, aut, cs, pids);
}

int main(){
	int i;
	int status = 0;
	float ut, wt, tat;
	int last_pid;

	// Reset global variables, clean CPUs, and initialise the queues
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		cpu_alls[i] = NULL;
	}
	hard_reset();
	process_que_init(&queue_go);
	process_que_init(&queue_wait);

	// Processes are read in and stored after the workload has been read in.
	while( (status = (readProcess(&processes[number_of_processes]))) ){
		if (status == 1){
			number_of_processes++;
		}
		if (number_of_processes > MAX_PROCESSES || number_of_processes == 0){
			error_invalid_number_of_processes(number_of_processes);
		}
	}

	qsort(processes, number_of_processes, sizeof(Process*), compareArrivalTime);

	// main execution loop
	while (number_of_processes){
		new_process_add();
		run_and_wait();
		ready_process();
		wait_ready();
		
		state_update();
		
		cpu_utilliz += process_to_run();

		//When there are no more running or incoming processes and the waiting queue is empty, the programme will terminate.
		if (process_to_run() == 0 && total_process_inc() == 0 && queue_wait.size == 0){
			break;
		}
		clock_t++;
	}

	// calculations
	for(i = 0; i < number_of_processes; i++){
		turn_arounds +=processes[i].endTime - processes[i].arrivalTime;
		total_wait_time += processes[i].waitingTime;

		if (processes[i].endTime == clock_t){
			last_pid = processes[i].pid;
		}
	}

	wt = avg_waittime(total_wait_time);
	tat = avg_turnaround(turn_arounds);
	ut = avg_utilliz(cpu_utilliz);

	result_display(wt, tat, clock_t, ut, context_switchs, last_pid);
	
	return 0;
}