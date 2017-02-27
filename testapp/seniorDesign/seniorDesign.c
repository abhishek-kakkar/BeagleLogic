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

/* State Data values */
#define dataLH 0b01000000
#define dataHL 0b10000000
#define dataHH 0b11000000
#define dataLL 0

/* MQTT defined values */
#define ADDRESS		"tcp://localhost:1883"
#define CLIENTID	"FMCFlow"
#define TOPIC		  "MQTTTest"
#define QOS			  1
#define TIMEOUT		10000L

State presentState[5]={INIT};
State previousState=INIT;//for use with stateINIT only
stateData data;

/* Quadrature state machine */
void changeState(int current1, int current2){
  int read = current1;
  int temp = 0x00;
  int mask = dataHH;

  data.LH = dataLH;
  data.HL = dataHL;
  data.HH = dataHH;
  data.LL = dataLL;

  for(i=0; i<5; i++){

    if(i==4){

      /* reset for the 2nd byte */
      data.LH = dataLH;
      data.HL = dataHL;
      data.HH = dataHH;
      mask = dataHH;
      read = current2;

    }

    /* access bits step 1 */
    temp = read & mask;

    switch(presentState[i]){
      case LL:
        stateLL(temp);
        break;
      case LH:
        stateLH(temp);
        break;
      case HL:
        stateHL(temp);
        break;
      case HH:
        stateHH(temp);
        break;
      default:
        stateINIT(temp,previousState);
        break;
    }

    /* shift all bytes to look at next bit pair */
    data.LH = data.LH >>2;
    data.HL = data.HL >>2;
    data.HH = data.HH >>2;
    mask = mask >> 2;
  }
}

/* state functions */
void stateLL(int temp){

  if(temp == data.LH){
    risingEdgeCounts[i*2+1]++;
    countbackward++;
    presentState[i] = LH;
  }
  else if(temp == data.HL){
    risingEdgeCounts[i*2]++;
    countforward++;
    presentState[i] = HL;
  }
  else if( temp == data.HH){
    risingEdgeCounts[i*2]++;
    risingEdgeCounts[i*2+1]++;
    counterror++;
    presentState[i] = INIT;
    previousState = LL;
  }
  else if(temp != data.LL){
    printf("Error StateLL\n");
    presentState[i] = INIT;
    previousState = LL;
  }
}

void stateLH(int temp){

    if(temp == data.HL){
      risingEdgeCounts[i*2]++;
      counterror++;
      presentState[i] = INIT;
      previousState = LH;
    }
    else if(temp == data.LL){
      countforward++;
      presentState[i] = LL;
    }
    else if(temp == data.HH){
      risingEdgeCounts[i*2]++;
      countbackward++;
      presentState[i] = HH;
    }
    else if(temp != data.LH){
      printf("Error StateLH\n");
      presentState[i] = INIT;
      previousState = LH;
    }
}

void stateHL(int temp){

    if(temp == data.LH){
      risingEdgeCounts[i*2+1]++;
      counterror++;
      presentState[i] = INIT;
      previousState = HL;
    }
    else if(temp == data.LL){
      countbackward++;
      presentState[i] = LL;
    }
    else if(temp == data.HH){
      risingEdgeCounts[i*2+1]++;
      countforward++;
      presentState[i] = HH;
    }
    else if(temp != data.HL){
      printf("Error StateHL\n");
      presentState[i] = INIT;
      previousState = HL;
    }
}

void stateHH(int temp){

    if(temp == data.LH){
      countforward++;
      presentState[i] = LH;
    }
    else if(temp == data.HL){
      countbackward++;
      presentState[i] = HL;
    }
    else if(temp == data.LL){
      counterror++;
      presentState[i] = INIT;
      previousState = HH;
    }
    else if(temp != data.HH){
      printf("Error StateHH\n");
      presentState[i] = INIT;
      previousState = HH;
    }
}

void stateINIT(int temp, State previous){

    if(temp == data.LH && (previous == LL || previous == HL)){
      risingEdgeCounts[i*2+1]++;
      presentState[i] = LH;
    }
    else if(temp == data.HL && (previous == LL || previous == LH)){
      risingEdgeCounts[i*2]++;
      presentState[i] = HL;
    }
    else if(temp == data.LL){
      presentState[i] = LL;
    }
    else if (temp == data.HH && (previousState == HL || previousState == LL)){
        risingEdgeCounts[i*2+1]++;
    }
    else if(temp == data.HH && (previousState == LH || previousState == LL)){
        risingEdgeCounts[i*2]++;
        presentState[i] = HH;
    }
    else{
      printf("Error Init\n");
      presentState[i] = INIT;
      previousState = INIT;
  }
}

/* Queue data for publishing over MQTT */
inline void MQTT_queueData(void *MQTT_package) {
	/* Update semaphore */
	int semVal;
	printf("hello handler\n");
	sem_getvalue(&MQTT_mutex, &semVal);
	sem_post(&MQTT_mutex);
	printf("semVal after post is %d\n", semVal);

	/* Set Flag to 0*/
	pub_signal = 0;
}

/* Thread handler*/
void *MQTT_thread(void *MQTT_package){

	MQTT_Package *package = (MQTT_Package*)MQTT_package;
	int rc, semVal;
	char PAYLOAD[100] = "";

	/* Init MQTT*/
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	/* create connection */
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

		/* CHANGE LATER */
		//PAYLOAD = "HELLO";

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
int start_MQTT_t(void *MQTT_package, pthread_t MQTT_t) {

	printf("Creating MQTT Thread\n");
	if (pthread_create(&MQTT_t, NULL, MQTT_thread, MQTT_package)) {

		printf("failed to create thread\n");
		return 1;
	}
	printf("Thread created\n");

//	pthread_join(MQTT_t,NULL);
	return 0;
}
