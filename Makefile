day := day8

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -O2 -g -funroll-loops
INCLUDES := -Iusflib2/include

OBJS := $(day).c $(wildcard usflib2/obj/*.o)

all: $(day)

$(day): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm

.PHONY: all
