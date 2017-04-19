# Build burst
#

EXE=burst
#objects:
OBJS=burst.o makeargv.o read_process.o

#Compiler + options
CC=gcc
CC_OPTS=-Wall -std=c99

all: executable

#For debug mode, add debug to cc_opts
debug: CC_OPTS += -DDEBUG
debug: executable

executable: ${EXE}

burst: burst.o makeargv.o read_process.o
	${CC} $^ -o $@

#Not quite sure what $^ and $@ and $< are...

burst.o: burst.c
	${CC} ${CC_OPTS} -c $<

makeargv.o: makeargv.c makeargv.h
	${CC} ${CC_OPTS} -c $<  

read_process.o: read_process.c
	${CC} ${CC_OPTS} -c $<

clean :
	rm -f ${EXE} ${OBJS} ${LIBS}
