csrc = $(wildcard src/*.c) \
       $(wildcard src/predictor/*.c) \
	   $(wildcard src/cliparser/*.c) \
	   $(wildcard src/encoder/*.c) \
	   $(wildcard src/utils/*.c) \



obj = $(csrc:.c=.o)

LDFLAGS = -O3 -g -Wall -lm

main.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) main.out
	
