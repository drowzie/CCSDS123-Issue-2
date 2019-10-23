csrc = $(wildcard src/*.c) \
       $(wildcard src/predictor/*.c) \
	   $(wildcard src/cliparser/*.c) \
	   $(wildcard src/predictor/localsum/*.c) \
	   $(wildcard src/predictor/weights/*.c) \
	   $(wildcard src/predictor/utils/*.c) \
       $(wildcard src/predictor/localdiffrences/*.c)



obj = $(csrc:.c=.o)

LDFLAGS = -g -Wall -lm -Ofast

main: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) myprog





