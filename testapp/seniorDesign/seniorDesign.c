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
#include <string.h>
#include "semaphore.h"
#include "lfq.h"
#include "MQTTClient.h"
#include "seniorDesignLib.h"
#include "../libbeaglelogic.h"

#define ADDRESS		"tcp://localhost:1883"
#define CLIENTID	"FMCFlow"
#define TOPIC		"MQTTTest"
#define QOS			1
#define TIMEOUT		10000L


int Rand_Int(int a, int b)
{
	return (rand() % (a + b + 1) + a);
}

/* Quadrature counting */
inline void quadrature_counter(int buffer1, int buffer2)
{
	const int FORWARDCONSTANT = 0b10101010; //constant that contains 4 bit pairs going forward
	const int BACKWARDCONSTANT = 0b01010101; //constant that contains 4 bit pairs going backward
	int read[2]; //place both buffer values into int array
	int temp = 0x00; //variable to hold masked value
	int forwardcheck = 0b10000000; //compares temp to forward value "10"
	int backwardcheck = 0b01000000; //compares temp to backward value "01"
	int mask = 0b11000000; //masks two bits at a time
	int j = 0; //loop counters

	read[0] = buffer1;
	read[1] = buffer2;

	// if all bit pairs in the first byte are going forward, avoid shifting just add 4 to forward count
	if (read[0] == FORWARDCONSTANT)
	{
		countforward = countforward + 4;
	}
	// if all bit pairs in the first byte are going backward, avoid shifting just add 4 to backward count
	else if (read[0] == BACKWARDCONSTANT)
	{
		countbackward = countbackward + 4;
	}
	/* If different check bit pairs individually */
	else
	{
		i = 0;
		for (j = 0; j< 5; j++)
		{
			if (j == 4)
			{
				i = 1; //if the 4 bit pairs in the first byte have already been checked, increase i to check second byte.
					   //Also restore the checks and mask before checking the second byte
				forwardcheck = 0b10000000;
				backwardcheck = 0b01000000;
				mask = 0b11000000;
			}

			temp = 0x00; //clear temp every run
			temp = read[i] & mask; //access bits

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
	}
}

/* Thread handler*/
void *MQTT_thread(void *ptr_package){

	seniorDesignPackage *package = (seniorDesignPackage*)ptr_package;
	int rc, semVal;
	char  PAYLOAD[32] = "hi";
	/* Init MQTT*/
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);
	}

	while(1){

		/* Wait on signal */
		sem_getvalue(package->MQTT_mutex, &semVal);
		printf("semVal = %d\n", semVal); 
		sem_wait(package->MQTT_mutex);

		/* Send message */
		pubmsg.payload = PAYLOAD;
		pubmsg.payloadlen = strlen(PAYLOAD);
		pubmsg.qos = QOS;
		pubmsg.retained = 0;
		MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
		printf("Waiting for up to %d seconds for publication of %s\n"
			"on topic %s for client with ClientID: %s\n",
			(int)(TIMEOUT / 1000), PAYLOAD, TOPIC, CLIENTID);
		rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		printf("Message with delivery token %d delivered\n", token);

	}

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);

	printf("hello from MQTT thread");
	//return rc;
}

/* Helper function to start MQTT thread */
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
