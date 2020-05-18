OBJ = main.o state.o action.o playsound.o process.o capture.o bmp.o private/mutex.o private/private_impl.o private/threadcondition.o private/yuv_conversion.o camera.o graphics.o cameracontrol.o

# raspicam.o
INC =   -I/opt/vc/include -I/opt/vc/userland-master/interface/vcos -I/opt/vc/userland-master -I/opt/vc/userland-master/interface/vcos/pthreads -I/opt/vc/userland-master/interface/vmcs_host/linux -I/opt/vc/userland-master/host_applications/linux/libs/bcm_host/include


DEPS =

LDFLAGS = 	-L/opt/vc/lib
#INC = 	 -Iprivate -I. 
LIBS = -lm -lbcm2835 -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lrt -lGLESv2 -lEGL
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
	rm -f greenlight *.o private/*.o mmal/*.o *~ core *~
