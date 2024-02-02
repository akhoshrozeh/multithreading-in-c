#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "SortedList.h"

#define BILLION 1000000000

// globals
int iterations_number = 1;		// default
int threads_number = 1;			// default
int threads_flag = 0;
int iterations_flag = 0;
int opt_yield = 0;  // shared with SortedList.h
int yield_flag = 0;
int mutex_flag = 0;
int spin_flag = 0;
int sync_flag = 0;
pthread_mutex_t mutex_lock;  
int spin_lock = 0; 
int elements_size; // threads_number * iterations_number

SortedList_t* list; 
SortedListElement_t* elements; 


// FUNCTIONS

void get_test_name(char* test_name) {
	
	if(yield_flag) { 
		if (opt_yield & INSERT_YIELD)
			strncat(test_name, "i", 1);
		if (opt_yield & DELETE_YIELD)
			strncat(test_name, "d", 1);
		if (opt_yield & LOOKUP_YIELD)
			strncat(test_name, "l", 1);
	}
	if(!yield_flag)
		strncat(test_name, "none", 4);

	strncat(test_name, "-", 1);

	if(!sync_flag)
		strncat(test_name, "none", 4);

	if(sync_flag) {
		if(spin_flag)
			strncat(test_name, "s", 1);
		if(mutex_flag)
			strncat(test_name, "m", 1);
	}
	
}


void signal_handler(int signal) {
	if(signal == SIGSEGV) {		
		fprintf(stderr, "Error: SIGSEGV signal caught. %s\n", strerror(errno) );
		exit(1);
	}
}


void get_random_key(char *s, const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int i;
    for (i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}


void* m_thread(void* id)
{
	int thread_id = *((int*) id);
	int i;

	if(mutex_flag)
			pthread_mutex_lock(&mutex_lock);

	else if(spin_flag)
		while(__sync_lock_test_and_set(&spin_lock, 1));
			
	
	for(i = thread_id; i < elements_size; i += threads_number)
		SortedList_insert(list, &elements[i]);
	


	if(SortedList_length(list) < 0) {
		fprintf(stderr, "Error: bad length. corrupted list. \n");
		exit(2);
	}

	if(mutex_flag)
		pthread_mutex_unlock(&mutex_lock);

	else if(spin_flag)
		__sync_lock_release(&spin_lock);

	
	for(i = thread_id; i < elements_size; i += threads_number) {

		if(mutex_flag)
			pthread_mutex_lock(&mutex_lock);

		else if(spin_flag)
			while(__sync_lock_test_and_set(&spin_lock, 1));

		SortedListElement_t* element_ptr = SortedList_lookup(list, elements[i].key);

		if(element_ptr == NULL) {
			fprintf(stderr, "Error: couldn't delete element; corrupted list. \n");
			exit(2);
		}

		if(SortedList_delete(element_ptr) == 1) {
			fprintf(stderr, "Error: couldn't delete element; corrupted list.\n");
			exit(2);
		}

		if(mutex_flag)
		pthread_mutex_unlock(&mutex_lock);

		else if(spin_flag)
			__sync_lock_release(&spin_lock);




	}

	return NULL;
}



int main(int argc, char* argv[])
{	
	long long time_elapsed;
	struct timespec start, end;
	const int NO_OF_LISTS = 1;
	const int KEY_LENGTH = 15;
	char* yield_args;
	char* sync_arg;
	int i;

	srand(time(NULL));
	

	static const struct option long_options[] = 
	{
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", required_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{0, 0, 0, 0}
	};

	char c;
	while (1)
	{
		int option_index = 0;  
		c = getopt_long(argc, argv, "", long_options, &option_index); 

		if (c == -1)
			break;

		switch(c)
		{
			case 't':      
				threads_flag = 1;
				threads_number = atoi(optarg);
				break;

			case 'i':
				iterations_flag = 1;
				iterations_number = atoi(optarg);
				break; 

			case 'y':
				yield_flag = 1;
				yield_args = optarg;
				break;  
			case 's':
				sync_flag = 1;
				sync_arg = optarg;
				break;

			default:  
				fprintf(stderr, "Correct Usage: lab2_list --threads=# --iterations=# [--yield=[idl]] [--sync=m,s]\n" );
				exit(1);
		}

	}

	if(!iterations_flag || !threads_flag) {
		fprintf(stderr, "Correct Usage: lab2_list --threads=# --iterations=# [--yield=[idl]] [--sync=m,s]\n" );
		exit(1);
	}

	if(sync_flag) {

		if(!strcmp(sync_arg,"m")) {
			mutex_flag = 1;
			pthread_mutex_init(&mutex_lock, NULL);
		}

		else if(!strcmp(sync_arg, "s")) 
			spin_flag = 1;

		else {
			fprintf(stderr, "Correct Usage: lab2 --threads=# --iterations=# [--yield=[idl]] [--sync=m,s]\n" );
			exit(1);
		}
	}



	if(yield_flag) {
		//printf("OPTIONS: %s\n", yield_options);
		if(strstr(yield_args, "i") != NULL) {
    		opt_yield |= INSERT_YIELD;
		}
		if(strstr(yield_args, "d") != NULL) {
    		opt_yield |= DELETE_YIELD;
		}
		if(strstr(yield_args, "l") != NULL) {
    		opt_yield |= LOOKUP_YIELD;
		}
		else if(strstr(yield_args, "abcefghjkmnoqrstuvqxyz") != NULL){
			fprintf(stderr, "Correct Usage: lab2 --threads=# --iterations=# [--yield=[idl]] [--sync=m,s]\n" );
			exit(1);
		}

	}

	signal(SIGSEGV, signal_handler);
	elements_size = threads_number * iterations_number;

	// the list used by threads
	list = (SortedList_t *) malloc(sizeof(SortedList_t)); 

	// initialization of the list
	list->key = NULL;
	list->prev = list; 
	list->next = list;

	// create and initialize the required number of elements
	elements = (SortedListElement_t*) malloc(elements_size * sizeof(SortedListElement_t));


	// initialize elements with random keys
    for(i = 0; i < elements_size; i++){
    	char * temp_key = (char *) malloc(15 * sizeof(char));
    	get_random_key(temp_key, KEY_LENGTH);
		elements[i].key = temp_key;
    }

	// create space for threads and thread ids to keep track
	pthread_t* threads = (pthread_t*) malloc(threads_number * sizeof(pthread_t));
	int* thread_ids = (int*) malloc(threads_number * sizeof(int));

	


	// start the clock
	if (clock_gettime(CLOCK_REALTIME, &start) < 0) {
		fprintf(stderr, "Error: calling clock_gettime (1). %s\n", strerror(errno));
		exit(1);
	}

	// create the threads
	for (i = 0; i < threads_number; ++i) {
		thread_ids[i] = i;
		if (pthread_create(threads + i, NULL, m_thread, (void*)(thread_ids + i)) != 0) {
			fprintf(stderr, "Error: creating thread %s\n", strerror(errno));
			exit(1);
		}
	}

	// wait to join
	for (i = 0; i < threads_number; ++i) {
		if (pthread_join(threads[i], NULL) != 0) {
		fprintf(stderr, "Error: joining thread. %s\n", strerror(errno));
		exit(1);
		}
	}

	if (clock_gettime(CLOCK_REALTIME, &end) < 0) {
		fprintf(stderr, "Error: calling clock_gettime (2). %s\n", strerror(errno));
		exit(1);
	}
	// clock ended

	time_elapsed = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);

	if(mutex_flag)
        pthread_mutex_destroy(&mutex_lock);
    

	if (SortedList_length(list) != 0) {
		fprintf(stderr, "Error: list not empty. corrupted list. \n");
		exit(2);
	}

	// free all allocated memory
	for (i = 0; i < elements_size; ++i)
		free((void*)elements[i].key);

	free(elements);
	free(threads);
	free(thread_ids);

	// create the test name
	char test_name[] = "list-";
	get_test_name(test_name);

	long long operations = 3 * threads_number * iterations_number;
	long long avg_op_time = time_elapsed / operations;

	printf("%s,%d,%d,%d,%lld,%lld,%lld\n", test_name, threads_number, iterations_number, NO_OF_LISTS, operations, time_elapsed, avg_op_time);
	exit(0);

}























