OBJS = beaglelogic_seniorDesignV2.o beaglelogic.o seniorDesign.o 
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c -pthread $(DEBUG)
LFLAGS = -Wall -pthread $(DEBUG)

seniorDesignAPP : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o seniorDesignAPP

beaglelogic_seniorDesignV2.o : seniorDesign/lfq.h seniorDesign/seniorDesignLib.h libbeaglelogic.h 
	$(CC) $(CFLAGS) beaglelogic_seniorDesignV2.c

beaglelogic.o : libbeaglelogic.h 
	$(CC) $(CFLAGS) beaglelogic.c 

seniorDesign.o : seniorDesign/seniorDesignLib.h
	$(CC) $(CFLAGS) $(SUBDIR) seniorDesign/seniorDesign.c
clean:
	\rm *.o *~ seniorDesignAPP

