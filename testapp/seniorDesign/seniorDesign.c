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
		//printf("value dequeue %2x\n",lfq_element );
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
