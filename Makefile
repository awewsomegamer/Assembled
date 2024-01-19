PRODUCT := ./assembled.out
CFILES := $(shell find ./src/ -type f -name '*.c')
CFLAGS := -Isrc/include -lcurses -o $(PRODUCT)
GLIB_FLAGS = `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -DAS_GLIB_ENABLE

all: glib

standalone:
	gcc $(CFILES) $(CFLAGS)

glib:
	gcc $(CFILES) $(CFLAGS) $(GLIB_FLAGS)

documentation:
		doxygen ./Doxyfile

run: all
	$(PRODUCT) test/test.txt

clean:
	rm -rf docs/*

debug: CFLAGS += -DDEBUG_MODE
	debug: all
