/*
 * SeniorDesign.c
 *
 *  Created on: Feb 2, 2017
 *      Author: Michael
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "lfq.h"
#include "seniorDesignLib.h"
#include "../libbeaglelogic.h"

//counters to keep track of how many times forward and backward has been seen
int countforward = 0;
int countbackward = 0;
int counterror = 0;
int i;

int Rand_Int(int a, int b)
{
	return (rand() % (a + b + 1) + a);
}

void quadrature_counter(int buffer1, int buffer2)
{
	const int FORWARDCONSTANT = 0b10101010; //constant that contains 4 bit pairs going forward
	const int BACKWARDCONSTANT = 0b01010101; //constant that contains 4 bit pairs going backward

	static int past[2] = { 0x00, 0x00 }; //holds last run (static)
	int read[2]; //place both buffer values into int array
	int temp = 0x00; //variable to hold masked value

	int forwardcheck = 0b10000000; //compares temp to forward value "10"
	int backwardcheck = 0b01000000; //compares temp to backward value "01"
	int mask = 0b11000000; //masks two bits at a time in a byte

	int j = 0; //loop counters

	read[0] = buffer1;
	read[1] = buffer2;

	//present run = last run, do nothing
	if (read[0] == past[0] && read[1] == past[1])
	{
		//printf("Past = Present. Did nothing. \n");
	}

	else
	{
		for (i = 0; i<2; i++)
		{
			// if all bits are going forward, avoid shifting just add 8 to forward count
			if (read[i] == FORWARDCONSTANT)
			{
				countforward = countforward + 4;
			}

			// if all bits are going backward, avoid shifting just add 8 to backward count
			else if (read[i] == BACKWARDCONSTANT)
			{
				countbackward = countbackward + 4;
			}

			else
			{
				for (j = 0; j< 4; j++)
				{
					temp = 0x00;
					temp = read[i] & mask; //access first two bits in a byte

					//check for errors - bit pairs "11" or "00"
					if ((temp == mask) || (temp == 0))
					{
						counterror++;
						//printf("count error\n");
					}

					//check for forward flow - bit pair "10"
					else if ((temp & forwardcheck) == forwardcheck)
					{
						countforward++;
						//printf("count fwd\n");
					}

					//check for backward flow - bit pair "01"
					else if ((temp & backwardcheck) == backwardcheck) //else if later
					{
						countbackward++;
						//printf("count back\n");
					}

					//catch all - error in value read
					else
					{
						printf("ERROR SHOULD NOT HAVE ENTERED HERE");
					}

					//shift values right by two to check next two bits
					forwardcheck = forwardcheck >> 2;
					backwardcheck = backwardcheck >> 2;
					mask = mask >> 2;
				}

				//restore the checks and mask before checking the second byte
				forwardcheck = 0b10000000;
				backwardcheck = 0b01000000;
				mask = 0b11000000;
			}
		}

		//set past = present for next run
		past[0] = read[0];
		past[1] = read[1];
	}
}

void *process_thread(void *ptr_package) {

	size_t sz;
	seniorDesignPackage *package = (seniorDesignPackage*)ptr_package;

	printf("hellos from process thread\n");

	while (1) {

		/*when on BBB*/
		/*Cread 4MB buffer*/
		char buffer[4 * 1000 * 1000];

		/*start reading*/
		poll(&package->pollfd, 1, 500);
		for (i=0; i < 4 * 1000 * 1000; i+=2) {

			sz = read(package->bfd_cpy, buffer, 4 * 1000 * 1000);

			printf("%2x %2x\n", buffer[i], buffer[i+1]);

			/*Quadrature_counter*/
			quadrature_counter((int) buffer[i], (int) buffer[i + 1]);

			/*store in circular buffer*/
			lfq_queue(package->ptr_lfq, (void*)&buffer[i]);
			lfq_queue(package->ptr_lfq, (void*)&buffer[i + 1]);

			if (sz == 0) {

				printf("I am breaking things\n");
				break;
			}
			else if (sz == -1) {
				poll(&package->pollfd, 1, 500);
				continue;
			}
		}
	}

	printf("hello from process thread\n");
	return NULL;

}

void *MQTT_thread(void *ptr_package){

	seniorDesignPackage *package = (seniorDesignPackage*) ptr_package;
	for(i=0; i< 4*1000*1000; i++){

		char *ptr_lfq_element = (char*) lfq_dequeue(package->ptr_lfq);
		char lfq_element = *ptr_lfq_element;

		// do stuff here
		printf("value dequeue %2x\n",lfq_element );
	}

	printf("hello from MQTT thread");
	return NULL;
}

/*Helper function to start process thread*/
int start_process_t(void *ptr_package, pthread_t process_t){

	printf("Creating Process Thread\n");
	if (pthread_create(&process_t, NULL, process_thread, ptr_package)) {

		printf("failed to create thread\n");
		return 1;
	}
	printf("Thread created\n");

	pthread_join(process_t, NULL);
	return 0;
}

/*Helper function to start MQTT thread*/
int start_MQTT_t(void *ptr_package, pthread_t MQTT_t) {

	printf("Creating MQTT Thread\n");
	if (pthread_create(&MQTT_t, NULL, MQTT_thread, ptr_package)) {

		printf("failed to create thread\n");
		return 1;
	}
	printf("Thread created\n");

//	pthread_join(MQTT_t,NULL);
	return 0;
}
