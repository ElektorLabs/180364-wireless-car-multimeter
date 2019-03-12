CC		= gcc
CXX		= g++
CFLAGS		= -Wall -ggdb -I../../lib -I/usr/lib/arm-linux-gnueabihf/ -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\"
LDFLAGS		= -L../../lib /usr/lib/arm-linux-gnueabihf/libmosquitto.so.1 /usr/lib/arm-linux-gnueabihf/libmosquittopp.so.1
LIBS          	= -lbcm2835 
RADIOHEADBASE 	= ./RadioHead
INCLUDE 	= -I$(RADIOHEADBASE) 


.PHONY: all clean

all : main

main.o : main.cpp
	${CXX} -c $^ -o $@ ${CFLAGS} -c $(INCLUDE)

mqtt.o : mqtt.cpp
	${CXX} -c $^ -o $@ ${CFLAGS} 

RasPi.o: $(RADIOHEADBASE)/RHutil/RasPi.cpp
				$(CC) $(CFLAGS) -c $(RADIOHEADBASE)/RHutil/RasPi.cpp $(INCLUDE)


RH_RF95.o: $(RADIOHEADBASE)/RH_RF95.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHDatagram.o: $(RADIOHEADBASE)/RHDatagram.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHHardwareSPI.o: $(RADIOHEADBASE)/RHHardwareSPI.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHSPIDriver.o: $(RADIOHEADBASE)/RHSPIDriver.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHGenericDriver.o: $(RADIOHEADBASE)/RHGenericDriver.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHGenericSPI.o: $(RADIOHEADBASE)/RHGenericSPI.cpp
				$(CC) $(CFLAGS) -c $(INCLUDE) $<

main : main.o mqtt.o RasPi.o RH_RF95.o RHHardwareSPI.o RHGenericDriver.o RHGenericSPI.o RHSPIDriver.o
	${CXX} $^ $(LIBS) -o $@ ${LDFLAGS} 



clean :
	-rm -f *.o main
