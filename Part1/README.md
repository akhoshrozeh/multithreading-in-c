NAME: Anthony Khoshrozeh
EMAIL: akhoshrozeh@ucla.edu
ID: 305366728


### File Descriptions ###

	lab2_add.c:
		a C program that implements and tests a shared variable add function, that takes options for threads, iterations, sync, and yield

	lab2_list.c:
		a C program that allows for multiple threads to do specificied operations concurrently with other threads. takes same options as lab2_add
		with different arguments

	SortedList.h:
		a C header file that protypes the linked list elements and their operations: insert, delete, length and lookup

	SortedList.c:
		implements the header file with certain yield options

	Makefile:
		build: 
			compiles and builds both lab2_add.c and lab2_list.c with gcc

		tests:
			runs tests.sh which runs many tests for both add and list programs and outputs them to lab2_add.csv and lab2_list.csv that will be used to make graphs

		graphs:
			runs gnuplot command with supplied data reduction scripts (.gp) and creates .png files of the .csv data

		dist:
			creates the tarball for submission

		clean:
			deletes all contents created by the Makefile


	lab2_add.csv:
		contains data about many different runs of lab2_add executable

	lab2_list.csv:
		contains data about many different runs of lab2_list executable


	lab2_add-1.png: threads and iterations required to generate a failure (with and without yields)
	lab2_add-2.png: average time per operation with and without yields.
	lab2_add-3.png: average time per (single threaded) operation vs. the number of iterations.
	lab2_add-4.png: threads and iterations that can run successfully with yields under each of the synchronization options. 
	lab2_add-5.png: average time per (protected) operation vs. the number of threads.
	lab2_list-1.png: average time per (single threaded) unprotected operation vs. number of iterations (illustrating the correction of the       					per-operation cost for the list length).
	lab2_list-2.png: threads and iterations required to generate a failure (with and without yields).
	lab2_list-3.png: iterations that can run (protected) without failure.
	lab2_list-4.png: (length-adjusted) cost per operation vs the number of threads for the various synchronization options.

	tests.sh:
		runs lab2_add and lab2_list with all different options such as iterations, threads, synchronization and yield options and outputs
		the result to lab2_add.csv and lab2_list.csv, resepectively.

	README:
		contains file descriptions and also analysis of the project (aka questions w/ answers from specs)




### Analysis ###

QUESTION 2.1.1 - causing conflicts:

Why does it take many iterations before errors are seen?
	-> Once there is more than one thread and as the iterations become greater and greater, so does the
	   probabilitity of a thread's CPU time being preempted during in the critical section. This means it's more
	   likely that the other thread(s) will be scheduled to run before the first thread could finish computation 
	   in the critical section. This results in non-deterministic values of the shared counter,
	   which we consider errors. 

Why does a significantly smaller number of iterations so seldom fail?
	-> When there's a smaller number of iterations there is a greater chance that the thread will complete the 
	   critical section in one time slice before being preempted during the critical section where it's doing
	   computation to the shared variable.



QUESTION 2.1.2 - cost of yielding:

Why are the --yield runs so much slower?
	-> The --yield option runs slower because the threads get changed from running states to ready states, which means
	   that there is context switch which creates more overhead due to all data like registers that has to saved and restored
	   when the thread gets put back into a running state.

Where is the additional time going?
	-> The time goes into the context switch, which must save the registers, the PC, memory maps, stack pointer, and other various
	   data structures, and then restore previous saves for other processes/threads.

Is it possible to get valid per-operation timings if we are using the --yield option? If so, explain how. If not, explain why not.
	-> It is not possible in this context to get valid per-operation timings with the --yield option because we are unaware of 
	   the amount of time that passes when OS performs a context switch between threads.



QUESTION 2.1.3 - measurement errors:

Why does the average cost per operation drop with increasing iterations?
	-> As there are increasing iterations, the cost per operation drops because the greater iterations account for the overhead
	   of thread creation. So as there's more iterations, which is quick work for the CPU, the cost of operation goes down because
	   more gets done with the same amount of overhead from thread creation.

If the cost per iteration is a function of the number of iterations, how do we know how many iterations to run (or what the "correct" cost is)?
	-> The "correct" cost could be the number of iterations it takes for the slope of a graph of "cost-per-iterations vs. iterations" is linear, 
	   not logarithmic as in lab2_add-3.png.



QUESTION 2.1.4 - costs of serialization:

Why do all of the options perform similarly for low numbers of threads?
	-> When there are a lower number of threads, each thread is more likely to complete before it's preempted and another tries to enter the
	   critical section. This means that the threads generally don't have to wait on other threads for critical section completion because there
	   is less competition for computing on the shared variable. There is also less overhead when there are less threads.

Why do the three protected operations slow down as the number of threads rises?
	-> They slow down as the number of threads rises because there are more occasions where multiple threads are trying to access the critical
	   sections. So more threads spend more time waiting to run, which essentially slowing down protected operations. 



QUESTION 2.2.1 - scalability of Mutex

Compare the variation in time per mutex-protected operation vs the number of threads in Part-1 (adds) and Part-2 (sorted lists). 
	-> In the the add program, the time per operation increases as the number of threads increase, because more threads are trying to access
	   the critical section. In the list program, the cost per operation also increases greatly and possibly more than add, which is what I would expect as the list has more operations.

Comment on the general shapes of the curves, and explain why they have this shape.
	-> Generally, the time per operation increases as the threads increase because there are more threads trying to access the critical section.
	   Which means more threads are going to waiting as only 1 thread can access at one instance, so the time per operation naturally increases.

Comment on the relative rates of increase and differences in the shapes of the curves, and offer an explanation for these differences.
	-> There seems to be a greater increase with the list program, which can be explained by the fact the doing operations on the linked list
	   takes more instructions and therefore more cycles to do than the add program. Which means threads are going to waiting longer for one thread to finish its critical section. 


QUESTION 2.2.2 - scalability of spin locks

Compare the variation in time per protected operation vs the number of threads for list operations protected by Mutex vs Spin locks. Comment on the general shapes of the curves, and explain why they have this shape.
	->	The spin locks clearly have a greater time per operation as the threads increase in the add program, but for the list its not as clear   	 from the graphs. I would definitely expect that the spin locks would also be greater for the list as well than mutex. Spins waste more 
		CPU cycles so as the number of threads grow, the cost in time will get greater and greater.

Comment on the relative rates of increase and differences in the shapes of the curves, and offer an explanation for these differences.
	-> The spin locks seem to increase much faster than the mutex, which is because the spin locks waste CPU cycles and increase the time per 	   operation faster than mutex locks.









