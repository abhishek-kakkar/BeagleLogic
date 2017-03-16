/*
 * seniorDesignLib.h
 *
 *  Created on: Feb 2, 2017
 *      Author: michael
 */

#include <sys/poll.h>
#ifndef SENIORDESIGNLIB_H_
#define SENIORDESIGNLIB_H_

//throw i back in here if you get an error
//counters to keep track of how many times forward and backward has been seen
extern int pub_signal, transmit;
extern uint32_t forwardCount[5];
extern uint32_t backwardCount[5];
extern uint32_t errorCount[5];
extern uint32_t risingEdgeCounts[10];
extern uint32_t LastRisingEdgeTime[10];
extern uint32_t clockValue;
extern uint32_t event;

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

	struct pollfd pollfd;
	int bfd_cpy;
	sem_t *MQTT_mutex;

	/* Counters to Publish*/
	uint32_t MQTT_countforward[5];
	uint32_t MQTT_countbackward[5];
	uint32_t MQTT_counterror[5];
	uint32_t MQTT_risingEdgeTime[10];
  uint32_t MQTT_LastRisingEdgeTime[10];
  uint32_t MQTT_time;
  uint32_t MQTT_event;
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
