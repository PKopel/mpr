CC ?= gcc
OPEN_MP := -Wall -fopenmp
OPTIONS ?= 
BUCKET ?= buckets/bucket.h buckets/bucket.c
ARGS ?=

.PRECIOUS: %.o

%.o: %.c 
	@$(CC) $(OPTIONS) $(OPEN_MP) -o $@ $(BUCKET) $< 

run-%: %.o 
	@./$< $(ARGS)