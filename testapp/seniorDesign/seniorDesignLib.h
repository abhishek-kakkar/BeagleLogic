/*
 * seniorDesignLib.h
 *
 *  Created on: Feb 2, 2017
 *      Author: michael
 */

#include <sys/poll.h>
#ifndef SENIORDESIGNLIB_H_
#define SENIORDESIGNLIB_H_

int i;

//counters to keep track of how many times forward and backward has been seen
extern int pub_signal;
extern uint32_t forwardCount[5];
extern uint32_t backwardCount[5];
extern uint32_t errorCount[5];
extern uint32_t risingEdgeCounts[10];
extern uint32_t channelTimes[10];

extern sem_t MQTT_mutex;

enum State {INIT, LL, LH, HL, HH};
typedef enum State state;

typedef struct{
  int LH;
  int HL;
  int HH;
  int LL;
} stateData;

typedef struct {

	struct lfq *ptr_lfq;
	struct pollfd pollfd;
	int bfd_cpy;
	sem_t *MQTT_mutex;

	/* Counters to Publish*/
	int MQTT_forwardCount[5];
	int MQTT_backwardCount[5];
	int MQTT_errorCount[5];
	int MQTT_risingEdgeTime[10];
} MQTT_Package;

//Bit processing
int Rand_Int(int a, int b);

/* Quadrature State Machine functions */
void changeState(int current1, int current2);
void stateLL(int temp);
void stateLH(int temp);
void stateHL(int temp);
void stateHH(int temp);
void stateINIT(int temp, state previous);

/* MQTT functions */
int  start_MQTT_t();
void *MQTT_thread(void *ptr_package);
void MQTT_queueData(void *MQTT_package);


#endif /* SENIORDESIGNLIB_H_ */
