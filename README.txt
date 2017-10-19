/*
 * Author: Nicholas Christensen					User ID: nwchrist
 * Due Date: 10/7/2017
 * Class: CS3331
 * Program Assignment #2 ReadMe
 */
 
 
 1.
	The idea behind this program is to split up the merge sort
	process into several different sections in order to utilize
	a multicore CPU to perform the sort faster. 
 
 2. 
	This program has two files, main.c and merge.c. Main.c takes 
	in an array of distinct integers and saves it into a shared
	memory segment. Main.c then calls merge.c and passes it the
	high and low indexes of the array to search. Merge.c then
	forks itself into two halfs of the array recursively until
	it reaches the base case of an array of size 2, then merge
	will swap the array to sort it at the base case and exit,
	at which point the next upper process will have two sorted
	arrays which it can merge using a modified binary search,
	and will then exit to the next process untill all of the
	arrays have been sorted and a final array results.

 3.
	A shared memory segment is created at the beginning of the
	process in order to store the array that is passed into the
	program. This array is used to store the values of the array
	at each checkpoint along the sorting process. Additionally,
	each non-basecase instance of the merge process will created
	a temporary shared memory array in order to temporarily store
	the results of its binarysearch children, but this shared memory
	is deleted immediatley after usage to avoid issues with memory.
	
 4.
	There are no race conditions in my program. This is because in
	conditions where a race condition could be present, the parent
	is instructed to wait for its children to finish, creating a type
	of checkpoint that stops race conditions.
	
 5.
	You cannot save the assigned array entry back into the array
	immediatley because other binarysearch children are still
	running and updating the array immediatley would introduce
	race conditions that would likely corrupt the output.

 6.
	I use a short lived shared memory array with a key assigned by the OS
	in order to hold the temporary array of binarysearch results. 
	This arrayis deleted just after creation to avoid hogging memory.

 7.
	Yes, using busy waiting and shared memory we could create shared memory
	"flags" that tell child processes when all other processes are done
	and then those child processes can take new arguments from shared memory
	and run the next section without having to be terminated and reinstantiated.