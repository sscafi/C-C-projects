/*
 *  Scafi 
 *  Saher 
 *  216691396
 *  sscafi 
 *
*/

//included classes
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <semaphore.h>

#define TEN_MILLIS_IN_NANOS 10000000

// circular buffer item structure
typedef  struct {
     char  data;
     off_t offset ;
     char string[7];
} BufferItem ;

BufferItem *buffer = NULL;

//global variables
FILE *file = NULL;
FILE *copy = NULL;
FILE *record_L = NULL;


int i = 0; 
int buff_size = 0;
int n_IN = 0;
int n_OUT = 0;

int out_helper = 0; 

int *cart_in_join = NULL;
int *cart_out_join = NULL;

//mutex 
pthread_mutex_t ops_lock; 
pthread_mutex_t done_lock;  
pthread_mutex_t innncrease_lock; 
pthread_mutex_t outtincrease_luck; 
pthread_mutex_t inndecreaser_lock; 

void sleep_wait();
int filled_item_buffer();
int empty_item_buffer();
void *in(void *arg);
void *out(void *arg);
void close_release();


int main(int argc, char *argv[]){



  if (argc != 7) {
	fprintf(stderr, "not enough arguments given\n");
	exit(-1);
  }


  if (atoi(argv[1]) < 1){
	fprintf(stderr, "invalid number for IN threads\n");
	exit(-1);
  }


  if (atoi(argv[2]) < 1){
	fprintf(stderr, "invalid number for OUT threads");
	exit(-1);
  }


if (argv[4] == NULL){
	fprintf(stderr, "invalid name for copy file");
	exit(-1);
}


if(atoi(argv[5]) < 1){
	fprintf(stderr, "invalid buffer size");
	exit(-1);
}

if (argv[6] == NULL){
	fprintf(stderr, "invalid name ");
	exit(-1);
}

//intialize locks
pthread_mutex_init(&ops_lock, NULL);
pthread_mutex_init(&done_lock, NULL);
pthread_mutex_init(&innncrease_lock, NULL);
pthread_mutex_init(&outtincrease_luck, NULL);
pthread_mutex_init(&inndecreaser_lock, NULL); 

//open source file, copy file, and log file

    file = fopen(argv[3], "r");          
       copy = fopen(argv[4], "w");
       record_L = fopen(argv[6], "w");

	// check for error in fopens
   if (file == NULL) {
	fprintf(stderr, "File does not exists \n");
	exit(-1);
	}

   if (copy == NULL) {
	fprintf(stderr, "File does not exists \n");
	exit(-1);
	}

       if (record_L == NULL) {
	fprintf(stderr, "File does not exists \n");
	exit(-1);
	}  





buff_size = atoi(argv[5]);
n_IN = atoi(argv[1]);
out_helper = n_IN; //OUT function helper
n_OUT = atoi(argv[2]);

cart_in_join = malloc(n_IN * sizeof(int));
cart_out_join = malloc(n_OUT * sizeof(int));

if(cart_in_join == NULL || cart_out_join == NULL){
	fprintf(stderr, "cannot assign memory to threads, please try again\n");
	exit(-1);
}

    pthread_t t_IN[n_IN];
    pthread_t t_OUT[n_OUT];
    pthread_attr_t  t_attribute; 



	

   buffer = (BufferItem*) malloc(buff_size * sizeof(BufferItem));

   for(i = 0; i < buff_size; i++){
	strcpy(buffer[i].string, "empty"); 
	}

  





   for (i = 0; i < n_IN; i++) {

	pthread_attr_init(&t_attribute);        
	pthread_create(&t_IN[i], NULL, (void *) in, file);

    }



  for (i = 0; i < n_OUT; i++) {

	pthread_create(&t_OUT[i], NULL, (void *) out, copy);

    }





for(i = 0; i < n_IN; i++){
   while(cart_in_join[i] == 0);  

}

	

for(i = 0; i < n_OUT; i++){
   while(cart_out_join[i] == 0);
}

	



    for (i = 0; i < n_IN; i++) {


	pthread_join(t_IN[i], NULL);

    }

   for (i = 0; i < n_OUT; i++) {



	pthread_join(t_OUT[i], NULL);

    }

	



fclose(file);
fclose(copy);
fclose(record_L);



exit(0); 
}

void sleep_wait(){
struct timespec t;

t.tsec = 0;
t.tnsec = rand()%(TEN_MILLIS_IN_NANOS+1);
nanosleep(&t, NULL);
}


int empty_buffer_item(){

    for(i = 0; i < buff_size; i++){
	if(strcmp(buffer[i].string, "empty") == 0)
		return i;
    }
    
		return -1; 

}


int filled_buffer_item(){

    for(i = 0; i < buff_size; i++){
	if(strcmp(buffer[i].string, "filled") == 0){
		return i;
	}

    }
    
		return -1; 

}


void *in(void *arg){

sleep_wait(); 

//local variables
int index = 0;
int in_number = 0;
int offset_in = 0;
char byte_in = 0;


pthread_mutex_lock(&innncrease_lock);
in_number = n_IN++;
pthread_mutex_unlock(&innncrease_lock);

do{

    
    pthread_mutex_lock(&ops_lock);
    
    index = empty_buffer_item();

	while(index != -1){
	  

		
		offset_in = ftell(file);		
		byte_in = fgetc(file);

		
		if(fprintf(record_L, "read_byte PT%d O%d B%d I-1\n", in_number, offset_in, byte_in) < 0) {
			fprintf(stderr, "log file not valid\n");
			exit(-1);
		}

		if(byte_in == EOF)
			break;

		
		else{
			buffer[index].offset = offset_in;
			buffer[index].data = byte_in;
			strcpy(buffer[index].string, "filled");
		}

		
		if(fprintf(record_L, "produce PT%d O%d B%d I%d\n", in_number, offset_in, byte_in, index) < 0) {
			fprintf(stderr, " The log file is not  valid, please try again\n");
			exit(-1);
		}

		index = empty_buffer_item();
		sleep_wait();

	   }
		
     
     pthread_mutex_unlock(&ops_lock);
   } while (!feof(file)); 

   
pthread_mutex_lock(&inndecreaser_lock);
out_helper = out_helper - 1;
pthread_mutex_unlock(&inndecreaser_lock);


pthread_mutex_lock(&done_lock);
cart_in_join[in_number] = 1;
pthread_mutex_unlock(&done_lock);

sleep_wait();
pthread_exit(0);

}


void *out(void *arg){

sleep_wait();


int threat_numb = 0;
int off_offset = 0;
char byte_out = 0;
int index = 0;

int set_offset = 0; 
int threads_working = 0; 

pthread_mutex_lock(&outtincrease_luck);
threat_numb = n_OUT++;
pthread_mutex_unlock(&outtincrease_luck);

do {

   
   index = filled_buffer_item();

   if(index != -1){

	
	pthread_mutex_lock(&ops_lock);
	index = filled_buffer_item();

	
	off_offset = buffer[index].offset;
	byte_out = buffer[index].data;

	
	if(fprintf(record_L, "consume CT%d O%d B%d I%d\n", threat_numb, off_offset, byte_out, index) < 0) {
			fprintf(stderr, "The log file is not valid, please try again\n");
			exit(-1);
	}

	strcpy(buffer[index].string, "this buffer is empty");
	buffer[index].offset = 0;

	
	pthread_mutex_unlock(&ops_lock);

	
	pthread_mutex_lock(&ops_lock);

	
	set_offset = fseek(copy, off_offset, SEEK_SET); 
	   if(set_offset < 0){
		pthread_mutex_unlock(&ops_lock);
		fprintf(stderr, "something wrong with what you seek\n");
			exit(-1);
	   }

	   if(fputc(byte_out, copy) == EOF){
		pthread_mutex_unlock(&ops_lock);
		fprintf(stderr, " The File that needs to be written is not valid, try again\n");
			exit(-1);
	   }
	   
	   
	if(fprintf(record_L, "write_byte CT%d O%d B%d I-1\n", threat_numb, off_offset, byte_out) < 0) {
		fprintf(stderr, " The log file is not valid, try again\n");
		exit(-1);
	}

	
	pthread_mutex_unlock(&ops_lock);
	

   }

   sleep_wait();
   
  
   threads_working = out_helper;

}while(threads_working > 0 || index != -1); 



pthread_mutex_lock(&done_lock);
cart_out_join[threat_numb] = 1;
pthread_mutex_unlock(&done_lock);

  pthread_exit(0);
}
