#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/*
 * Author: Nicholas Christensen					User ID: nwchrist
 * Due Date: 10/7/2017
 * Class: CS3331
 * Program Assignment #2
 * File Name: merge.c
 * Program Purpose:
 *		This program takes a high and low index, and sorts the array recursively and concurrently
 */
 
 main (int argc, char *argv[])
 { 
 	 int i;
	 //Parse arguments
	 int low = atoi(argv[2]);
	 int high = atoi(argv[1]);
	 
	 //Attach to shared memory
	 int ShmID1;
	 key_t Key;
	 int *array;
	 Key = ftok("./", 'n');
	 ShmID1 = shmget(Key, sizeof(sizeof(int) * (high + 1)), 0666);
	 array = shmat (ShmID1, NULL, 0);
	 
	 //Check for base case
	 if ((high - low) == 1)
	 {
		 if (array[low] > array[high])
		 {
			 int temp = array[low];
			 array[low] = array[high];
			 array[high] = temp;
		 }
		 
		 //Print entry output
		 char output [10000];
		 sprintf(output, "   ### M-PROC(%d) created by M-PROC(%d): entering with a[%d..%d] -- sorted\n   ", getpid(), getppid(), low, high);
		 char *outpointer = output;
		 char temp4[1000];
		 int k = 0;
		 
		 for (i = low; i < (high + 1); i++)
		 {
		 if ((k % 8) == 0 && k != 0)
		 {
			 sprintf(temp4, "\n   ");
			 strcat(outpointer, temp4);
		 }
		 sprintf(temp4, "%6d", array[i]);
		 strcat(outpointer, temp4);
		 k++;;
		 }
		 sprintf(temp4, "\n");
		 strcat(outpointer, temp4);
		 write(1, output, strlen(output));
		 
		 exit(0);
	 }
	 
	 //If we are not at the base case, find the middle and call two new merges using execvp()
	 int middle = (high + low) / 2;
	 
	 char * argv2[3];
	 char file []= {"./merge"};
	 char high1 [12];
	 char low1 [12];
	 char out [1000];
	 
	 //Fork for the lower array
	 pid_t pid = fork();
	 if (pid == 0)
	 {
		 int lowindex = low;
		 int highindex = middle;
		 sprintf(high1, "%d", highindex);
		 sprintf(low1, "%d", lowindex);
		 argv2[0] = file;
		 argv2[1] = high1;
		 argv2[2] = low1;
		 argv2[3] = '\0';
		 
		 //Print entry output
		 char output [10000];
		 sprintf(output, "   ### M-PROC(%d) created by M-PROC(%d): entering with a[%d..%d]\n   ", getpid(), getppid(), lowindex, highindex);
		 char *outpointer = output;
		 char temp4[1000];
		 int k = 0;
		 
		 for (i = low; i < (high + 1); i++)
		 {
		 if ((k % 8) == 0 && k != 0)
		 {
			 sprintf(temp4, "\n   ");
			 strcat(outpointer, temp4);
		 }
		 sprintf(temp4, "%6d", array[i]);
		 strcat(outpointer, temp4);
		 k++;
		 }
		 
		 sprintf(temp4, "\n");
		 strcat(outpointer, temp4);
		 write (1, output, strlen(output));
			
		 //Then execvp
		 if (execvp(file, argv2) < 0)
		 {
			 sprintf(out, "*** MAIN: execvp() failed\n");
			 write (1, out, strlen(out));
		 }
	 }
	 
	 //Fork for the upper array
	 pid = fork();
	 if (pid == 0)
	 {
		 int lowindex = middle + 1;
		 int highindex = high;
		 sprintf(high1, "%d", highindex);
		 sprintf(low1, "%d", lowindex);
		 argv2[0] = file;
		 argv2[1] = high1;
		 argv2[2] = low1;
		 argv2[3] = '\0';
		 
		 //Print entry output
		 char output [10000];
		 sprintf(output, "   ### M-PROC(%d) created by M-PROC(%d): entering with a[%d..%d]\n   ", getpid(), getppid(), lowindex, highindex);
		 char *outpointer = output;
		 char temp4[1000];
		 int k = 0;
		 
		 for (i = low; i < (high + 1); i++)
		 {
		 if ((k % 8) == 0 && k != 0)
		 {
			 sprintf(temp4, "\n   ");
			 strcat(outpointer, temp4);
		 }
		 sprintf(temp4, "%6d", array[i]);
		 strcat(outpointer, temp4);
		 k++;;
		 }
		 sprintf(temp4, "\n");
		 strcat(outpointer, temp4);
		 write(1, output, strlen(output));
			
		 //Then execvp
		 if (execvp(file, argv2) < 0)
		 {
			 sprintf(out, "*** MAIN: execvp() failed\n");
			 write (1, out, strlen(out));
		 }
	 }
	 
	 //Then wait for both child processes to finish
	 wait();
	 wait();
	 
	 //Then merge our two sorted arrays array1[low, middle] and array2[middle + 1, high]
	 
	 //We will need shared memory of the size of the finished array
	 int ShmID;
	 int * temparray;
	 ShmID = shmget (IPC_PRIVATE, sizeof(sizeof(int) * (high + 1)), IPC_CREAT | 0666);
	 sprintf(out, "   *** MERGE: shared memory created\n");
	 write (1, out, strlen(out));
	 temparray = shmat(ShmID, NULL, 0);
	 sprintf(out, "   *** MERGE: shared memory attached and is ready to use as temporary array\n");
	 write (1, out, strlen(out));
	 
	 //To do this, first we must create a process for each index of both arrays
	 int lowa = low;
	 int higha = middle;
	 int low2 = middle + 1;
	 int high2 = high;
	 int currentArraySize = (high - low + 1) / 2;
	 for (i = low; i < (high + 1); i++)
	 {
		 pid = fork();
		 if (pid == 0)
		 {
			 //Print output
			 char output [10000];
			 sprintf(output, "      $$$ B-PROC(%d): created by M-PROC(%d) for a[%d] = %d is created\n", getpid(), getppid(), i, array[i]);
			 write(1, output, strlen(output));
		 
			 //Then each process will search for its values position in the final array and store it in the temp array
			 int homeRank = i % currentArraySize;
			 int awayRank;
			 if ((i - lowa) < currentArraySize)
			 {
				 awayRank = mybsearch(array[i], high2, low2, array) - low2;
			 } else {
				 awayRank = mybsearch(array[i], higha, lowa, array) - lowa;
			 }
			 
			 int finalRank = homeRank + awayRank;
			 temparray[finalRank] = array[i];
			 
			 //Print output
			 sprintf(output, "      $$$ B-PROC(%d): a[%d] = %d is written to temp[%d]\n      $$$ B-PROC(%d): created by M-PROC(%d) for a[%d] = %d is terminated\n", 
				getpid(), i, array[i], finalRank, getpid(), getppid(), i, array[i]);
			 write(1, output, strlen(output));
			 
			 exit(0);
		 }
	 }
	 
	 //Wait for all merge children to be finished
	 for (i = 0; i < high + 1; i++)
	 {
		 wait();
	 }
	 
	 //After merging our two arrays, we will then set the array to be the temp array, detach and delete our shared memory, and then exit
	 int j = 0;
	 for (i = low; i < high + 1; i++)
	 {
		 array[i] = temparray[j];
		 j++;
	 }
	 
	 //Print entry output
	 char output [10000];
	 sprintf(output, "   ### M-PROC(%d), created by M-PROC(%d): merge sort a[%d..%d] completed:\n   ", getpid(), getppid(), low, high);
	 char *outpointer = output;
	 char temp4[1000];
	 int k = 0;
	 
	 for (i = low; i < (high + 1); i++)
	 {
	 if ((k % 8) == 0 && k != 0)
	 {
		 sprintf(temp4, "\n   ");
		 strcat(outpointer, temp4);
	 }
	 sprintf(temp4, "%6d", array[i]);
	 strcat(outpointer, temp4);
	 k++;;
	 }
	 sprintf(temp4, "\n");
	 strcat(outpointer, temp4);
	 write(1, output, strlen(output));
	 
	 shmdt(temparray);
	 shmdt(array);
	 shmctl(ShmID, IPC_RMID, NULL);
	 exit(0);
 }
 
/* ----------------------------------------------------------- */
/* FUNCTION  mybsearch:                                        */
/*      This function returns the position of a value in the   */
/*		shared memory array given a high and low index         */
/* PARAMETER USAGE :                                           */
/*		int value: the value to find the index for             */
/*		int high: the high index of the array to search        */
/*		int low: the low value of the array to search          */
/*		int * array: the array to search					   */	
/* FUNCTION CALLED :                                           */
/*    a list of functions that are called by this one          */
/* ----------------------------------------------------------- */
 int mybsearch(int value, int high, int low, int * array)
 {
	 if ((high - low) == 1)
	 {
		 if (value > array[high])
		 {
			 return high + 1;
		 }
		 if (value < array[high] && value > array[low])
		 {
			 return high;
		 }
		 return low;
	 }
	 
	 if (high == low)
	 {
		 if (value > array[high])
		 {
			 return high + 1;
		 }
		 if (value < array[high] && value > array[low])
		 {
			 return high;
		 }
		 return low;
	 }
	 
	 int middle = (high + low) / 2;
	 if (value < array[middle])
	 {
		 return mybsearch(value, middle, low, array);
	 }
	 return mybsearch(value, high, middle, array);
 }