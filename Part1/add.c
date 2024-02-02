#include <sys/types.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


// globals
int threads_flag = 0;
int iterations_flag = 0;
int opt_yield;  // flag for yield
int sync_flag = 0;
int mutex_flag = 0;
int spin_flag = 0;
int comp_swap_flag = 0;

int threads_number = 1;		// default
int iterations_number = 1;  // deafault
pthread_mutex_t mutex_lock;
int spin_lock = 0;

long long counter;


// functions

int get_test_name(void) {
	
	if(!opt_yield) {

		if(sync_flag) {

			if(mutex_flag)
				return 1;
			else if(spin_flag)
				return 2;
			else if(comp_swap_flag)
				return 3;
		}

		else if(!sync_flag)
			return 0;
	}

	else if(opt_yield) {

		if(sync_flag) {

			if(mutex_flag)
				return 5;
			else if(spin_flag)
				return 6;
			else if(comp_swap_flag)
				return 7;
		}

		else if(!sync_flag)
			return 4;
	}

	return -1; // should never be returned
	
		
}


void add(long long *pointer, long long value) { 

	if(mutex_flag) {
		pthread_mutex_lock(&mutex_lock); // entering critical section
		long long sum = *pointer + value; 
		if (opt_yield)
			sched_yield(); 
		*pointer = sum;
		pthread_mutex_unlock(&mutex_lock);
	}

	else if(spin_flag) {
		while (__sync_lock_test_and_set(&spin_lock, 1));
		long long sum = *pointer + value; 
		if (opt_yield)
			sched_yield(); 
		*pointer = sum;
 	 	__sync_lock_release(&spin_lock);
	}
	
	else if(comp_swap_flag) {
		long long old_val, sum;
		do {
			old_val = *pointer; 
			sum = old_val + value; 
			if (opt_yield)
				sched_yield();
		} while ( __sync_val_compare_and_swap(pointer, old_val, sum) != old_val); 
	}

	else {	// no --sync options
		long long sum = *pointer + value;
	    if (opt_yield)
	        sched_yield();
	    *pointer = sum;
	}

}

void *m_thread_add(void) {
	int i;
	for(i = 0; i < iterations_number; i++) {
		add(&counter, 1);
	}

	for(i = 0; i < iterations_number; i++) {
		add(&counter, -1);
	}

	return NULL;
}


int main(int argc, char* argv[]) {
	long long billion = 1000000000;
	char * test_names [8] = {
		"add-none",	// 0
		"add-m",	// 1
		"add-s",	// 2
		"add-c",	// 3
		"add-yield-none", // 4
		"add-yield-m",	  // 5
		"add-yield-s",	  // 6
		"add-yield-c" };  // 7	

	int test_name_index = 0;  // basic usage default
	int c; 
	char * sync_option;

	while(1) {
		static struct option long_options[] = 
		{
			{"threads",    required_argument, NULL, 't'},
			{"iterations",   required_argument, NULL, 'i'},
			{"yield",   no_argument, NULL, 'y'},
			{"sync",   required_argument, NULL, 's'},
			{0,0,0,0}	
		};

		int option_index = 0;
		c = getopt_long(argc, argv, "", long_options, &option_index);

		if(c == -1) { 
			break;
		}

		switch(c) {
			case 't':
				threads_flag = 1;
				threads_number = atoi(optarg);
				break;

			case 'i':
				iterations_flag = 1;
				iterations_number = atoi(optarg);
				break;

			case 'y':
				opt_yield = 1;
				break;

			case 's':
				sync_flag = 1;
				sync_option = optarg;
				break;


			default:
				fprintf(stderr, "Correct Usage: lab2_add --threads=# --iterations=# [--yield] [--sync=m,s,c]\n" );
				exit(1);
		}
 
	}

	counter = 0;
	struct timespec begin, end;
	// create space for threads
	pthread_t* threads = (pthread_t*) malloc(threads_number * sizeof(pthread_t));
	
	// checks the argument for --sync
	if(sync_flag) {
		if(!strcmp(sync_option,"m")) {
			mutex_flag = 1;
			pthread_mutex_init(&mutex_lock, NULL);
		}
		else if(!strcmp(sync_option, "s")) 
			spin_flag = 1;
		else if(!strcmp(sync_option, "c"))
			comp_swap_flag = 1;
		else {
			fprintf(stderr, "Correct Usage: lab2 --threads=# --iterations=# [--yield] [--sync=m,s,c]\n" );
			exit(1);
		}
	}

	

	if(clock_gettime(CLOCK_REALTIME, &begin) == -1) {
		fprintf(stderr, "Error: calling clock_gettime (1). %s\n", strerror(errno));
		exit(1);
	}

	// create 'threads_number' of threads
	int i; 
	for(i = 0; i < threads_number; i++) {
		if(pthread_create(&threads[i], NULL, (void*)&m_thread_add, NULL) < 0) {
			fprintf(stderr, "Error: creating thread %s\n", strerror(errno));
			exit(1);
		}
	}

	// waiting for the them to join
	for(i = 0; i < threads_number; i++) {
		if(pthread_join(threads[i], NULL) < 0) {
			fprintf(stderr, "Error: joining thread %s\n", strerror(errno));
			exit(1);
		}
	}


	if(clock_gettime(CLOCK_REALTIME, &end) == -1) {
		fprintf(stderr, "Error: calling clock_gettime (2). %s\n", strerror(errno));
		exit(1);
	}

	free(threads);
	test_name_index = get_test_name();

	if(test_name_index == -1) {
		fprintf(stderr, "Error: getting test name. \n");
		exit(2);
	}


	long long time_elapsed =  billion * (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);  // get seconds, convert to ns and add ns
	long long operations = threads_number * iterations_number * 2;
	long long avg_op_time = time_elapsed / operations;

	printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", test_names[test_name_index], threads_number, iterations_number, 
								 	operations, time_elapsed, avg_op_time, counter);


	exit(0);
}























