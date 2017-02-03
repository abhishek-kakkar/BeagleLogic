/*
 * seniorDesignLib.h
 *
 *  Created on: Feb 2, 2017
 *      Author: michael
 */

#ifndef SENIORDESIGNLIB_H_
#define SENIORDESIGNLIB_H_

//Bit processing 
int Rand_Int(int a, int b);
void quadrature_counter(int buffer1, int buffer2);

//thread functions 
void start_process_t();
void start_MQTT_t();
void *process_thread(void *ptr_q);
void *MQTT_thread(void *ptr_q); 

#endif /* SENIORDESIGNLIB_H_ */
