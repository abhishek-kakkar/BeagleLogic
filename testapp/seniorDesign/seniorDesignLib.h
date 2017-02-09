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

inline void quadrature_counter(int buffer1, int buffer2)
{
	const int FORWARDCONSTANT = 0b10101010; //constant that contains 4 bit pairs going forward
	const int BACKWARDCONSTANT = 0b01010101; //constant that contains 4 bit pairs going backward

	static int past[2] = { 0x00, 0x00 }; //holds last run (static)
	int read[2]; //place both buffer values into int array
	int temp = 0x00; //variable to hold masked value

	int forwardcheck = 0b10000000; //compares temp to forward value "10"
	int backwardcheck = 0b01000000; //compares temp to backward value "01"
	int mask = 0b11000000; //masks two bits at a time

	int j = 0; //loop counters

	read[0] = buffer1;
	read[1] = buffer2;

	/*put this if outside of he function so we don't have to jump*/
	//present run = last run, do nothing
	//printf("Past[0]= %d Past[1]= %d \n", past[0], past[1]);
	//printf("Current[0]= %d Current[1]= %d \n", read[0], read[1]);

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

	/*If different check bit pairs individually.
	Loop 5 times to check the 4 bit pairs in the first byte and the first bit pair in the second byte.
	The next bit pair (bits 10-11) are the prover input so it will have no impact on counters.
	The next 2 bit pairs (12-15) will always be grounded as we cannot access them and thus will have no impact on counters.*/

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

	//set past = present for next run
	past[0] = read[0];
	past[1] = read[1];
}

#endif /* SENIORDESIGNLIB_H_ */
