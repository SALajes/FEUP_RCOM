CC = gcc

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

dep := packet_factory.c state_machine.c interface.c


all: clean sender receiver

sender:
			@echo "Building sender..."
			$(CC) sender.c $(dep) -o sender.o

receiver: 
			@echo "Building receiver..."
			$(CC) receiver.c $(dep) -o receiver.o


clean:
				@echo "Cleaning up..."
				rm -rvf *.o ${BINS}