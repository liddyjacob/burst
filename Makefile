# Build burst
#

EXE=burst
#objects:
OBJS=burst.o makeargv.o

#Compiler + options
CC=gcc
CC_OPTS=-Wall

all: ${EXE}

#For debug mode, add debug to cc_opts
debug: CC_OPTS += -DDEBUG
debug: all

burst: burst.o makeargv.o
	${CC} $^ -o $@

#Not quite sure what $^ and $@ and $< are...

burst.o: burst.c
	${CC} ${CC_OPTS} -c $<

makeargv.o: makeargv.c makeargv.h
	${CC} ${CC_OPTS} -c $<  

clean :
	rm -f ${EXE} ${OBJS} ${LIBS}
