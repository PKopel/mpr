CC ?= gcc
OPTIONS ?= -Wall -fopenmp
BUCKETS ?= bucket.h bucket.c
ARGS ?=

.PRECIOUS: %.o

%.o: %.c 
	@$(CC) $(OPTIONS) -o $@ $< 

run-%: %.o 
	@./$< $(ARGS)