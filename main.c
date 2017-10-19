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
 * File Name: main.c
 * Program Purpose:
 *		This program takes an array(size 2^n with n up to 5) of distinct 
 *		integers and calls the program merge.c to concurrently sort the array
 */
 
 main ()
 {
	 //Declare initial variables and print initial output
	 char out[10000];
	 sprintf(out, "Merge Sort with Multiple Processes\n");
	 write (1, out, strlen(out));
	 char temp1[20];
	 scanf("%s", temp1);
	 int size = atoi(temp1);
	 int temp2;
	 int i;
	 int counter = 0;
	 int arguments [size];
	 
	 //Insert input array into arguments[]
	 while(counter < size)
	 {
		 scanf("%s", temp1);
		 if (*temp1 != -16)
		 {
			 temp2 = atoi(temp1);
			 arguments[counter] = temp2;
			 counter++;
		 }
	 }
	 
	 //Create Shared Memory
	 int ShmID;
	 key_t Key;
	 int *ShmPTR;
	 Key = ftok("./", 'n');
	 sprintf(out, "*** MAIN: shared memory key = %d\n", Key);
	 write (1, out, strlen(out));
	 ShmID = shmget (Key, sizeof(sizeof(int) * size), IPC_CREAT | 0666);
	 sprintf(out, "*** MAIN: shared memory created\n");
	 write (1, out, strlen(out));
	 ShmPTR = shmat(ShmID, NULL, 0);
	 sprintf(out, "*** MAIN: shared memory attached and is ready to use as main array\n");
	 write (1, out, strlen(out));
	 sprintf(out, "Input array for mergesort has %d elements:\n   ", size);
	 write (1, out, strlen(out));
	 char *outpointer = out;
	 char temp [10000];
	 
	 //Print out the initial array to output
	 for (i = 0; i < size; i++)
	 {
		 if ((i % 8) == 0 && i != 0)
		 {
			 sprintf(temp, "\n   ");
			 strcat(outpointer, temp);
		 }
		 
		 ShmPTR[i] = arguments[i];
		 sprintf(temp, "%6d", ShmPTR[i]);
		 strcat(outpointer, temp);
	 }
	 strcat(outpointer, "\n\n");
	 write (1, out, strlen(out));
	 
	 //Prep to execvp()
	 char * argv[3];
	 char file []= {"./merge"};
	 char high [12];
	 char low [12];
	 int lowindex = 0;
	 int highindex = size - 1;
	 sprintf(high, "%d", highindex);
	 sprintf(low, "%d", lowindex);
	 argv[0] = file;
	 argv[1] = high;
	 argv[2] = low;
	 argv[3] = '\0';
	 
	 //Fork for execvp()
	 pid_t pid = fork();
	 if (pid == 0)
	 {
		 sprintf(out, "*** MAIN: about to spawn the merge sort process\n");
		 write (1, out, strlen(out));
		 
		 //Print entry output
		 char output [10000];
		 sprintf(output, "   ### M-PROC(%d): entering with a[%d..%d]\n   ", getpid(), lowindex, highindex);
		 char *outpointer = output;
		 char temp4[1000];
		 
		 for (i = lowindex; i < (highindex + 1); i++)
		 {
		 if ((i % 8) == 0 && i != 0)
		 {
			 sprintf(temp4, "\n   ");
			 strcat(outpointer, temp4);
		 }
		 sprintf(temp4, "%6d", ShmPTR[i]);
		 strcat(outpointer, temp4);
		 }
		 sprintf(temp4, "\n");
		 strcat(outpointer, temp4);
		 write(1, output, strlen(output));
		 
		 //Execvp()
		 if (execvp(file, argv) < 0)
		 {
			 sprintf(out, "*** MAIN: execvp() failed\n");
			 write (1, out, strlen(out));
		 }
	 }
	 
	 //Wait for merge to finish
	 wait();
	 
	 //Print out the final array
	 char output1[10000];
	 char *outpointer1 = output1;
	 char temp4[1000];
	 sprintf(output1, "*** MAIN: merged array:\n   ");
	 for (i = 0; i < size; i++)
	 {
		 if ((i % 8) == 0 && i != 0)
		 {
			 sprintf(temp4, "\n   ");
			 strcat(outpointer1, temp4);
		 }
						 
		 sprintf(temp4, "%6d", ShmPTR[i]);
		 strcat(outpointer1, temp4);
	 }
	 sprintf(temp4, "\n");
	 strcat(outpointer1, temp4);
	 write (1, output1, strlen(output1));

	 //Detach and delete shared memory, print final output, and exit
	 shmdt(ShmPTR);
	 sprintf(out, "*** MAIN: shared memory successfully detached\n");
	 write (1, out, strlen(out));
	 shmctl(ShmID, IPC_RMID, NULL);
	 sprintf(out, "*** MAIN: shared memory successfully removed\n*** MAIN: exits\n");
	 write (1, out, strlen(out));
 }
 
 