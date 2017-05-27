OBJ = main.o state.o action.o playsound.o process.o capture.o bmp.o raspicam.o private/mutex.o private/private_impl.o private/threadcondition.o private/yuv_conversion.o

DEPS =

LDFLAGS = 	-L/opt/vc/lib
INC = 	 -Iprivate -I. 
LIBS = -lbcm2835 -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lrt -lGLESv2 -lEGL
CFLAGS =  -O0 -g -std=c++0x  -lrt

CC = g++

EXTENSION = .cpp

%.o:	%$(EXTENSION) $(DEPS)
	$(CC)  $(INC) -c -o $@ $< $(CFLAGS)
	
all:	greenlight

greenlight:	$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDFLAGS)
	
rungreenlight:
	sudo ./greenlight -da -hf -w 512 -h 512

.PHONY:	clean

clean:
	rm -f *.o private/*.o mmal/*.o *~ core *~
