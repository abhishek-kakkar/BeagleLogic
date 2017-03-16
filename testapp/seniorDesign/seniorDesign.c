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
#include "MQTTClient.h"
#include "seniorDesignLib.h"
#include "../libbeaglelogic.h"

/* State Data values */
#define dataLH 0b01000000
#define dataHL 0b10000000
#define dataHH 0b11000000
#define dataLL 0b00000000

/* MQTT defined values */
#define ADDRESS		"tcp://localhost:1883"
#define CLIENTID	"FMCFlow"
#define QOS		1
#define TIMEOUT		10000L
#define TOPIC 		"test"
#define PAYLOADSIZE 10000

int j;

state presentState[5]={INIT};
state previousState=INIT;//for use with stateINIT only
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

  for(j=0; j<5; j++){

    if(j==4){

      /* reset for the 2nd byte */
      data.LH = dataLH;
      data.HL = dataHL;
      data.HH = dataHH;
      mask = dataHH;
      read = current2;

    }

    /* access bits step 1 */
    temp = read & mask;

    switch(presentState[j]){
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

    data.LH = data.LH >>2;
    data.HL = data.HL >>2;
    data.HH = data.HH >>2;
    mask = mask >> 2;

  }
}

/* state functions */
void stateLL(int temp){

  if(temp == data.LH){
    risingEdgeCounts[j*2+1]++;
    LastRisingEdgeTime[j*2+1] = clockValue;
    backwardCount[j]++;
    presentState[j] = LH;
  }
  else if(temp == data.HL){
    risingEdgeCounts[j*2]++;
    LastRisingEdgeTime[j*2] = clockValue;
    forwardCount[j]++;
    presentState[j] = HL;
  }
  else if( temp == data.HH){
    risingEdgeCounts[j*2]++;
    LastRisingEdgeTime[j*2] = clockValue;
    risingEdgeCounts[j*2+1]++;
    LastRisingEdgeTime[j*2+1] = clockValue;
    errorCount[j]++;
    presentState[j] = INIT;
    previousState = LL;
  }
  else if(temp != data.LL){
    printf("Error StateLL\n");
    presentState[j] = INIT;
    previousState = LL;
  }
}

void stateLH(int temp){

    if(temp == data.HL){
      risingEdgeCounts[j*2]++;
      LastRisingEdgeTime[j*2] = clockValue;
      errorCount[j]++;
      presentState[j] = INIT;
      previousState = LH;
    }
    else if(temp == data.LL){
      forwardCount[j]++;
      presentState[j] = LL;
    }
    else if(temp == data.HH){
      risingEdgeCounts[j*2]++;
      LastRisingEdgeTime[j*2] = clockValue;
      backwardCount[j]++;
      presentState[j] = HH;
    }
    else if(temp != data.LH){
      printf("Error StateLH\n");
      presentState[j] = INIT;
      previousState = LH;
    }
}

void stateHL(int temp){

    if(temp == data.LH){
      risingEdgeCounts[j*2+1]++;
      LastRisingEdgeTime[j*2+1] = clockValue;
      errorCount[j]++;
      presentState[j] = INIT;
      previousState = HL;
    }
    else if(temp == data.LL){
      backwardCount[j]++;
      presentState[j] = LL;
    }
    else if(temp == data.HH){
      risingEdgeCounts[j*2+1]++;
      LastRisingEdgeTime[j*2+1] = clockValue;
      forwardCount[j]++;
      presentState[j] = HH;
    }
    else if(temp != data.HL){
      printf("Error StateHL\n");
      presentState[j] = INIT;
      previousState = HL;
    }
}

void stateHH(int temp){

    if(temp == data.LH){
      forwardCount[j]++;
      presentState[j] = LH;
    }
    else if(temp == data.HL){
      presentState[j] = HL;
    }
    else if(temp == data.LL){
      errorCount[j]++;
      presentState[j] = INIT;
      previousState = HH;
    }
    else if(temp != data.HH){
      printf("Error StateHH\n");
      presentState[j] = INIT;
      previousState = HH;
    }
}

void stateINIT(int temp, state previous){

    if(previous == INIT){

        if(temp == data.LH){
          presentState[j] = LH;
        }
        else if(temp == data.HL){
          presentState[j] = HL;
        }
        else if (temp == data.LL){
          presentState[j] = LL;
        }
        else if(temp == data.HH){
          presentState[j] = HH;
        }
        else{
          printf("Error at start of INIT \n");
        }
    }
    else if(temp == data.LH){
		presentState[j] = LH;
		if(previous == LL || previous == HL)
			risingEdgeCounts[j*2+1]++;
      LastRisingEdgeTime[j*2+1] = clockValue;
    }
    else if(temp == data.HL){
		presentState[j] = HL;
		if(previous == LL || previous == LH)
			risingEdgeCounts[j*2]++;
      LastRisingEdgeTime[j*2] = clockValue;
    }
    else if(temp == data.LL){
		    presentState[j] = LL;
    }
    else if (temp == data.HH){
      presentState[j] = HH;
		  if(previous == HL || previous == LL){
			     risingEdgeCounts[j*2+1]++;
           LastRisingEdgeTime[j*2+1] = clockValue;
      }
		  else if(previous == LH || previous == LL){
			     risingEdgeCounts[j*2]++;
           LastRisingEdgeTime[j*2] = clockValue;
      }
    }
    else{
		    printf("Error Init\n");
		    presentState[j] = INIT;
        previous = INIT;
  }
}

/* Queue data for publishing over MQTT */
inline void MQTT_queueData(void *MQTT_package) {
	/* Update semaphore */
	int semVal;
  MQTT_Package *package = (MQTT_Package*)MQTT_package;

  /* package data */
  memcpy(package->MQTT_countforward, forwardCount, sizeof(forwardCount));
  memcpy(package->MQTT_countbackward, backwardCount, sizeof(backwardCount));
  memcpy(package->MQTT_counterror, errorCount, sizeof(errorCount));
  memcpy(package->MQTT_risingEdgeTime, risingEdgeCounts, sizeof(risingEdgeCounts));
  memcpy(package->MQTT_LastRisingEdgeTime, LastRisingEdgeTime, sizeof(LastRisingEdgeTime));
  package->MQTT_time = clockValue;
  package->MQTT_event = event;

  /* Signal to publish */
	sem_getvalue(&MQTT_mutex, &semVal);
	sem_post(&MQTT_mutex);
	printf("semVal after post is %d\n", semVal);

	/* Set Flag to 0*/
	pub_signal = 0;
}

/* Thread handler*/
void *MQTT_thread(void *MQTT_package){

  int y;

	MQTT_Package *package = (MQTT_Package*)MQTT_package;
	int rc, semVal;
	char *PAYLOAD = (char*) malloc(PAYLOADSIZE);
  strcpy(PAYLOAD,"Hello");

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

  while(transmit){

  		/* Wait on signal */
  		sem_getvalue(package->MQTT_mutex, &semVal);
  		printf("semVal = %d\n", semVal);
  		sem_wait(package->MQTT_mutex);

      /* Send Hello */
      pubmsg.payload = PAYLOAD;
      pubmsg.payloadlen = strlen(PAYLOAD);
      pubmsg.qos = QOS;
      pubmsg.retained = 0;
      MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
      strcpy(PAYLOAD,"");

      /* Create Payload to send */
      for(y=0; y<10; y++){

        if(y<5){

          sprintf(PAYLOAD, "Counts for Byte Pair %lu\n"
          "Forward Counts = %lu\n"
          "Backward Counts = %lu\n"
          "Error Counts = %lu\n",
          y, package->MQTT_countforward[y], package->MQTT_countbackward[y],
          package->MQTT_counterror[y]);

          pubmsg.payload = PAYLOAD;
          pubmsg.payloadlen = strlen(PAYLOAD);
          MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        }
        sprintf(PAYLOAD, "channel %d Rising Edge Counts = %lu\n Last Rising Edge Time = %lu\n",
          y,package->MQTT_risingEdgeTime[y], package->MQTT_LastRisingEdgeTime[y]);

          pubmsg.payload = PAYLOAD;
          pubmsg.payloadlen = strlen(PAYLOAD);
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
      }

      /* Add Tigger event */
      sprintf(PAYLOAD, "time = %lu, trigger event = %lu \n-----------------------------------------------------------\n" ,
        package->MQTT_time, package->MQTT_event);

      pubmsg.payload = PAYLOAD;
      pubmsg.payloadlen = strlen(PAYLOAD);
      MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

      /* Clear PAYLOAD */
      memset(PAYLOAD,0,sizeof(PAYLOAD));
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

	return 0;
}
