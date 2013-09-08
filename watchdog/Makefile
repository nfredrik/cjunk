VPATH = ../appl : ../include
CFLAGS =  -g -Wall -I ./ -I ../appl -I ../include -L/usr/lib  -DCS_TYPES -L/usr/local/lib 
LDFLAGS=  -lnsl -llog4c -lconfig -lpthread   -Wl,-rpath,/usr/local/lib   

OBJ_C  =  ../appl/communication.o system_monitor.o

sys_mon: $(OBJ_C)
	$(CC)  $(LDFLAGS) -o sys_mon $(OBJ_C)


cs_cmd.o : ../appl/communication.h cs_commands.h


clean:	
	rm *.o

