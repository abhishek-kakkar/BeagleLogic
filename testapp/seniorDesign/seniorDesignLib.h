/*
 * seniorDesignLib.h
 *
 *  Created on: Feb 2, 2017
 *      Author: michael
 */

#include <sys/poll.h>
#ifndef SENIORDESIGNLIB_H_
#define SENIORDESIGNLIB_H_



typedef struct {

	struct lfq *ptr_lfq;
	struct pollfd pollfd;
	int bfd_cpy;

} seniorDesignPackage;

//Bit processing
int Rand_Int(int a, int b);

//thread functions
int  start_process_t();
int  start_MQTT_t();
void *process_thread(void *ptr_package);
void *MQTT_thread(void *ptr_package);



#endif /* SENIORDESIGNLIB_H_ */
