OBJS = beaglelogic_seniorDesignV2.o beaglelogic.o seniorDesign.o 
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c -pthread $(DEBUG)
LFLAGS = -Wall -pthread -lpaho-mqtt3c $(DEBUG)

seniorDesignAPP : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o seniorDesignAPP

beaglelogic_seniorDesignV2.o : seniorDesign/seniorDesignLib.h libbeaglelogic.h beaglelogic_seniorDesignV2.c 
	$(CC) $(CFLAGS) beaglelogic_seniorDesignV2.c

beaglelogic.o : libbeaglelogic.h beaglelogic.c
	$(CC) $(CFLAGS) beaglelogic.c 

seniorDesign.o : seniorDesign/seniorDesignLib.h seniorDesign/seniorDesign.c
	$(CC) -L/home/debian/paho.mqtt.c/build/output $(CFLAGS) -lpaho-mqtt3c seniorDesign/seniorDesign.c
clean:
	\rm *.o *~ seniorDesignAPP

