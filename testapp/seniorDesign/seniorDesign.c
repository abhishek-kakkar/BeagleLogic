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
#define dataLL 0b00000000

/* MQTT defined values */
#define ADDRESS		"tcp://localhost:1883"
#define CLIENTID	"FMCFlow"
#define QOS		1
#define TIMEOUT		10000L
#define TOPIC 		"test"

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
    //printf("mask = %2x \n", mask);
    //printf("temp = %d i = %d\n",temp,j);
    //printf("Present State = %d \n",presentState[j]);
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
    //printf("present State after = %d\n", presentState[j]);

    /* shift all bytes to look at next bit pair */
    /* Debug */
    //printf("LH %d\n", data.LH);
    //printf("HL %d\n", data.HL);
    //printf("HH %d\n", data.HH);
    //printf("LL %d\n", data.LL);
    data.LH = data.LH >>2;
    data.HL = data.HL >>2;
    data.HH = data.HH >>2;
    mask = mask >> 2;

  }

  /* Debug */
  //printf("forward counts %d\n", forwardCount[j]);
  //printf("backward counts %d\n", backwardCount[j]);
  //printf("error counts %d\n", errorCount[j]);

}

/* state functions */
void stateLL(int temp){

  if(temp == data.LH){
    risingEdgeCounts[j*2+1]++;
    backwardCount[j]++;
    presentState[j] = LH;
  }
  else if(temp == data.HL){
    risingEdgeCounts[j*2]++;
    forwardCount[j]++;
    presentState[j] = HL;
  }
  else if( temp == data.HH){
    risingEdgeCounts[j*2]++;
    risingEdgeCounts[j*2+1]++;
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
    }
    else if(temp == data.HL){
		presentState[j] = HL;
		if(previous == LL || previous == LH)
			risingEdgeCounts[j*2]++;
    }
    else if(temp == data.LL){
		presentState[j] = LL;
    }
    else if (temp == data.HH){
		presentState[j] = HH;
		if(previous == HL || previous == LL)
			risingEdgeCounts[j*2+1]++;
		else if(previous == LH || previous == LL)
			risingEdgeCounts[j*2]++;
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
  memcpy(package->MQTT_channelTimes, channelTimes, sizeof(channelTimes));
  package->MQTT_time = clockValue;
  package->MQTT_time = event;

  /* Signal to publish */
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
	char PAYLOAD[100] = "hello";

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

      pubmsg.payload = PAYLOAD;
      pubmsg.payloadlen = strlen(PAYLOAD);
      pubmsg.qos = QOS;
      pubmsg.retained = 0;
      MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
/*
      for(j=0; j<10; j++){

    		/* Create Payload to send
        /* need to evaluate this
        if(j<5){

		printf("publishing \n");
            pubmsg.payload = &package->MQTT_countforward[j];
            pubmsg.payloadlen = strlen(PAYLOAD);
            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
            pubmsg.payload = &package->MQTT_countbackward[j];
            pubmsg.payloadlen = strlen(PAYLOAD);
            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
            pubmsg.payload = &package->MQTT_counterror[j];
            pubmsg.payloadlen = strlen(PAYLOAD);
            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        }

        pubmsg.payload = &package->MQTT_risingEdgeTime;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        pubmsg.payload = &package->MQTT_channelTimes;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

        /* Debug
        printf("Waiting for up to %d seconds for publication of %s\n"
        "on topic %s for client with ClientID: %s\n",
        (int)(TIMEOUT / 1000), PAYLOAD, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        printf("Message with delivery token %d delivered\n", token);

        /* Clear PAYLOAD
        memset(PAYLOAD,0,sizeof(PAYLOAD));
      }

      sprintf(PAYLOAD, "time = %u, trigger event = %u", package->MQTT_time,
        package->MQTT_event);
      pubmsg.payload = PAYLOAD;
      MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
  }

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);

	printf("hello from MQTT thread");
	//return rc;
}
*/
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
