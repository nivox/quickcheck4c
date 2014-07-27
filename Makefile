CC=gcc
LD=gcc

CFLAGS=-g -Wall

EXE=example

all: $(EXE)

example: example.o quickcheck4c.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%: %.o
	$(LD) $(LDFLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f $(EXE)
	rm -f *.o test
