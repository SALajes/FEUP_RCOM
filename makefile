CC = gcc 

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

dep := packet_factory.c state_machine.c interface.c applicationLayer.c progressbar.c


all: clean sender receiver

sender:
	@echo "Building sender..."
	$(CC) sender.c $(dep) -o sender -lncurses

receiver: 
	@echo "Building receiver..."
	$(CC) receiver.c $(dep) -o receiver -lncurses

debug:
	@echo "Building sender and receiver with extra debugger information..."
	$(CC) sender.c $(dep) -o sender -g
	$(CC) receiver.c $(dep) -o receiver -g


clean:
	@echo "Cleaning up..."
	rm -rf *.o ${BINS} 