# Build burst
#

EXE=burst
#objects:
OBJS=burst.o

#Compiler + options
CC=gcc
CC_OPTS=-Wall

all : ${EXE}

burst: burst.o
	${CC} $^ -o $@

#Not quite sure what $^ and $@ and $< are...

burst.o: burst.c
	${CC} ${CC_OPTS} -c $<

clean :
	rm -f ${EXE} ${OBJS} ${LIBS}
