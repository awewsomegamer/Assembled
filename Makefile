PRODUCT := ./assembled.out
CFILES := $(shell find ./src/ -type f -name '*.c')
CFLAGS := -Isrc/include -lcurses -o $(PRODUCT)

all:
	gcc $(CFILES) $(CFLAGS)

run: all
	$(PRODUCT) test.txt