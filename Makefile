csrc = $(wildcard src/*.c) \
       $(wildcard src/predictor/*.c) \
       $(wildcard src/utils/*.c)

headers = $(wildcard src/predictor/include/*.h) \
	  $(wildcard src/utils/include/*.h)

obj = $(csrc:.c=.o)

LDFLAGS = -g -Wall -lm

main: $(obj)
	$(CXX) $(headers) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) myprog





