CC=gcc

csrc = $(wildcard src/*.c) \
       $(wildcard src/predictor/*.c) \
	   $(wildcard src/cliparser/*.c) \
	   $(wildcard src/encoder/*.c) \
	   $(wildcard src/utils/*.c)
obj = $(csrc:.c=.o)

#NOTE: Main folder for every include is set to src 
CFLAGS= -I./src/
LDFLAGS = -O3 -g -Wall -lm
main.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS) 
.PHONY: clean
clean:
	rm -f $(obj) main.out
