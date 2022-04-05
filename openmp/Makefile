CC ?= gcc
OPTIONS ?= -Wall -fopenmp
BUCKET ?= bucket.h bucket.c
ARGS ?=

.PRECIOUS: %.o

%.o: %.c 
	@$(CC) $(OPTIONS) -o $@ $(BUCKET) $< 

run-%: %.o 
	@./$< $(ARGS)