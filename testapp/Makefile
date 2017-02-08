OBJS = beaglelogic_seniorDesign.o seniorDesign.o beaglelogic.o 
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c -pthread $(DEBUG)
LFLAGS = -Wall $(DEBUG)
SUBDIR = seniorDesign/seniorDesign.c

seniorDesignAPP : $(objs)
	$(CC) $(LFLAGS) $(OBJS) -o seniorDesign

beaglelogic_seniorDesign.o : lfq.h seniorDesignLib.h 
	$(CC) $(CFLAGS) beaglelogic_seniorDesign.c
