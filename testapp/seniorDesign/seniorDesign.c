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

/* MQTT defined values */
#define ADDRESS		"tcp://localhost:1883"
#define CLIENTID	"FMCFlow"
#define TOPIC		  "MQTTTest"
#define QOS			  1
#define TIMEOUT		10000L

State presentState[5]={INIT};
State previousState=INIT;//for use with stateINIT only
u32int risingEdgeCounts[10]={0};
u32int channelTimes[10]={0};

/* Quadrature state machine */
void changeState(int current1, int current2){
  int read = current1;
  int temp = 0x00; //clear temp every run
  int mask = dataHH;
  stateData.LH = dataLH;
  stateData.HL = dataHL;
  stateData.HH = dataHH;

  for(i=0; i<5; i++){

    if(i==4){
      stateData.LH = dataLH;
      stateData.HL = dataHL;
      stateData.HH = dataHH;
      mask = dataHH;
      read = current2;
    }

    temp = read & mask; //access bits step 1

    switch(presentState[i]){
      case LL:
        stateLL(temp);
        break;
      case LH:
        stateLH(temp);
        break;
      case HL:
        stateHL(temp)
        break;
      case HH:
        stateHH(temp);
        break;
      default:
        stateINIT(temp,previousState);
        break;
    }

    stateData.LH = stateData.LH >>2;
    stateData.HL = stateData.HL >>2;
    stateData.HH = stateData.HH >>2;
    mask = mask >> 2;
  }
}

/* state functions */
void stateLL(int temp){

  switch(temp){
    case stateData.LH:
      risingEdgeCounts[i*2+1]++;
      countbackward++;
      presentState[i] = LH;
      break;
    case stateData.HL:
      risingEdgeCounts[i*2]++;
      countforward++;
      presentState[i] = HL;
      break;
    case 0:
      break;
    case stateData.HH:
      risingEdgeCounts[i*2]++;
      risingEdgeCounts[i*2+1]++;
      counterror++;
      presentState[i] = INIT;
      previousState = LL;
      break;
    default:
      printf("Error\n");
      presentState[i] = INIT;
      previousState = LL;
      break;
  }
}

void stateLH(int temp){

  switch(temp){
    case stateData.LH:
      break;
    case stateData.HL:
      risingEdgeCounts[i*2]++;
      counterror++;
      presentState[i] = INIT;
      previousState = LH;
      break;
    case 0:
      countforward++;
      presentState[i] = LL;
      break;
    case stateData.HH:
      risingEdgeCounts[i*2]++;
      countbackward++;
      presentState[i] = HH;
      break;
    default:
      printf("Error\n");
      presentState[i] = INIT;
      previousState = LH;
      break;
  }
}

void stateHL(int temp){

  switch(temp){
    case stateData.LH:
      risingEdgeCounts[i*2+1]++;
      counterror++;
      presentState[i] = INIT;
      previousState = HL;
      break;
    case stateData.HL:
      break;
    case 0:
      countbackward++;
      presentState[i] = LL;
      break;
    case stateData.HH:
      risingEdgeCounts[i*2+1]++;
      countforward++;
      presentState[i] = HH;
      break;
    default:
      printf("Error\n");
      presentState[i] = INIT;
      previousState = HL;
      break;
  }
}

void stateHH(int temp){

  switch(temp){
    case stateData.LH:
      countforward++;
      presentState[i] = LH;
      break;
    case stateData.HL:
      countbackward++;
      presentState[i] = HL;
      break;
    case 0:
      counterror++;
      presentState[i] = INIT;
      previousState = HH;
      break;
    case stateData.HH:
      break;
    default:
      printf("Error\n");
      presentState[i] = INIT;
      previousState = HH;
      break;
  }
}

void stateINIT(int temp, State previous){

  switch(temp){
    case stateData.LH:
      if(previous == LL || previous == HL)
        risingEdgeCounts[i*2+1]++;
      presentState[i] = LH;
      break;
    case stateData.HL:
      if(previous == LL || previous == LH)
        risingEdgeCounts[i*2]++;
      presentState[i] = HL;
      break;
    case 0:
      presentState[i] = LL;
      break;
    case stateData.HH:
      if(previousState == HL || previousState == LL){
        risingEdgeCounts[i*2+1]++;
      }
      if(previousState == LH || previousState == LL){
        risingEdgeCounts[i*2]++;
      }
      presentState[i] = HH;
      break;
    default:
      printf("Error\n");
      presentState[i] = INIT;
      previousState = INIT;
      break;
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
