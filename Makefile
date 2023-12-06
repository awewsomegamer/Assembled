PRODUCT := ./assembled.out
CFILES := $(shell find ./src/ -type f -name '*.c')
CFLAGS := -Isrc/include -lcurses -o $(PRODUCT) -g

all:
	gcc $(CFILES) $(CFLAGS)

run: all
	$(PRODUCT) test/test.txt

debug: CFLAGS += -DDEBUG_MODE
debug: all
