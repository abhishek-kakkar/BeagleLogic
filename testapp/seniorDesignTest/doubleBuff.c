/*
 * doubleBuff.c
 *
 *  Created on: Jan 30, 2017
 *      Author: michael
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_t writeThread;
pthread_t readThread;
pthread_mutex_t buffer_mutex;

char buff1[4];
char buff2[4];

struct mutex_shared{
	int stillReading, stillWriting,run_not_over;
	char *writeBuff, *readBuff;
}SHARED;

void *writeToBuff(void *idk){


	while(!SHARED.run_not_over){

		SHARED.stillWriting = 1;

		/*when on BBB*/
		//read()

		/*comment out whole for loop before using on BBB*/
		for(int i=0; i<4; i++){

			//do stuff here
		}

		SHARED.stillWriting = 0;
		while(SHARED.stillReading){};
	}

	printf("hello from write\n");
	return NULL;
}

void *readToBuff(void *idk){

	while(!SHARED.run_not_over){

		SHARED.stillReading = 1;

		for(int i=0; i<4; i++){
			// do stuff here
		}

		while(SHARED.stillWriting){};
		swap(writeThread,readThread);
	}

	printf("hello from read");
	return NULL;
}

void swap(char **a, char **b){

	pthread_mutex_lock(&buffer_mutex);

		printf("in swap\n");
		char *temp = *a;
		*a = *b;
		*b = temp;

		SHARED.stillReading = 0;
		//SHARED.stillWriting = 0;

	pthread_mutex_unlock(&buffer_mutex);
}

int main(){

	/* pass address of both buff */
	SHARED.writeBuff = buff1;
	SHARED.readBuff = buff2;

	/* print buffer address */
	printf("buff1 address %p\n", (void*) &buff1);
	printf("buff2 address %p\n", (void*) &buff2);

	/*what pointers point to before swap*/
	printf("writeBuff address its pointing to %p\n", SHARED.writeBuff);
	printf("readBuff address its pointing to %p\n", SHARED.readBuff);

	swap(&SHARED.writeBuff,&SHARED.readBuff);

	/* what pointers point to after swap*/
	printf("writeBuff address its pointing to %p\n", SHARED.writeBuff);
	printf("readBuff address its pointing to %p\n", SHARED.readBuff);

	/* make threads */
	pthread_mutex_init(&buffer_mutex,NULL);

	/*create write thread*/
	printf("Creating Write Thread\n");
	if(pthread_create(&writeThread, NULL, writeToBuff, NULL)){

		printf("failed to create thread\n");
		return 1;
	}
	printf("Thread created\n");

	/*create read thread */
	printf("Creating Read Thread\n");
	if(pthread_create(&readThread, NULL, readToBuff, NULL)){

			printf("failed to create thread\n");
			return 1;
	}
	printf("Thread created\n");

	pthread_join(writeThread, NULL);
	pthread_join(readThread, NULL);

	exit(0);
}
