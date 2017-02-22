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
extern int countforward;
extern int countbackward;
extern int counterror;
extern int pub_signal;
extern sem_t MQTT_mutex;
typedef struct {

	struct lfq *ptr_lfq;
	struct pollfd pollfd;
	int bfd_cpy;
	sem_t *MQTT_mutex;

	/* Counters to Publish*/
	int MQTT_countforward;
	int MQTT_countbackward;
	int MQTT_counterror;
	int MQTT_risingEdgeTime[10];


} MQTT_Package;

//Bit processing
int Rand_Int(int a, int b);

//thread functions
int  start_process_t();
int  start_MQTT_t();
void *process_thread(void *ptr_package);
void *MQTT_thread(void *ptr_package);
void MQTT_queueData(void *MQTT_package);
void quadrature_counter(int buffer1, int buffer2);

#endif /* SENIORDESIGNLIB_H_ */
