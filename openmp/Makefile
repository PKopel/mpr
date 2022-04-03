CC ?= gcc
OPTIONS ?= -Wall -fopenmp
ARGS ?=

.PRECIOUS: %.o

%.o: %.c 
	@$(CC) $(OPTIONS) -o $@ $< 

run-%: %.o 
	@./$< $(ARGS)