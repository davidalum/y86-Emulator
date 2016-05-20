CC  = gcc
CCFLAGS = -Wall -lm -Werror

all :	y86

y86 : y86emul.o
	$(CC) $(CCFLAGS) -o y86 y86emul.c

clean:
	rm -f y86emul.o
