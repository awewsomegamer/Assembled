PRODUCT := ./assembled.out
CFILES := $(shell find ./src/ -type f -name '*.c')
CFLAGS := -Isrc/include -o $(PRODUCT)

all:
	gcc $(CFILES) $(CFLAGS)

run:
	$(PRODUCT) test.txt