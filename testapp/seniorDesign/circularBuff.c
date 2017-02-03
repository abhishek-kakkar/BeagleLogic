/*
 * circularBuff.c
 *
 *  Created on: Feb 1, 2017
 *      Author: michael
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lfq.h"
#include "quadCount.h"

pthread_t process_t;
pthread_t MQTT_t;
pthread_mutex_t buffer_mutex;

char buffer_MQTT[32*1000*1000];
void *buff_ptr;
struct lfq q;
struct lfq *ptr_q;

struct mutex_shared{
	int stillReading, stillWriting,run_not_over;
	char *writeBuff, *readBuff;
}SHARED;

void *process_thread(void *ptr_q){

	/*when on BBB*/
	//read()

	/*comment out whole for loop before using on BBB*/
	char buffer_read = 1;
	void *PTR ;

	PTR = &buffer_read;

	for(int i=0; i<4*1000*1000; i++){

		//printf("here i guees\n");
		lfq_queue(ptr_q, PTR);
	}

	printf("hello from process thread\n");
	return NULL;
}

void *MQTT_thread(void *ptr_q){

	while(1){

		// do stuff here
		printf("value dequeue %p\n", lfq_dequeue(ptr_q));
	}

	printf("hello from MQTT thread");
	return NULL;
}

int main(){

		/* create buffer */
		buff_ptr = (void *) malloc( 32*1000*1000 * sizeof(void) );
		void ** PTR1 = buff_ptr;

		ptr_q = &q;

		/* Initialize struct for circular buffer*/
		lfq_init(ptr_q, 32*1000*1000, PTR1);

		/* make threads */
		/*create Process thread*/
		printf("Creating Process Thread\n");
		if(pthread_create(&process_t, NULL, process_thread, ptr_q)){

			printf("failed to create thread\n");
			return 1;
		}
		printf("Thread created\n");

		/*create MQTT thread*/
		printf("Creating MQTT Thread\n");
		if(pthread_create(&MQTT_t, NULL, MQTT_thread, ptr_q)){

				printf("failed to create thread\n");
				return 1;
		}
		printf("Thread created\n");

		pthread_join(process_t, NULL);
		pthread_join(MQTT_t, NULL);

		exit(0);
}
