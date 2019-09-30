csrc = $(wildcard src/*.c) \
       $(wildcard src/predictor/*.c) \
	   $(wildcard src/cliparser/*.c) \
       $(wildcard src/utils/*.c)

headers = $(wildcard src/predictor/include/*.h) \
	  $(wildcard src/utils/include/*.h)

obj = $(csrc:.c=.o)

LDFLAGS = -g -Wall -lm

main: $(obj)
	$(CC) $(headers) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) myprog





