CC ?= gcc
OPTIONS ?= -Wall -fopenmp
ARGS ?=

%.o: %.c 
	@$(CC) $(OPTIONS) -o $@ $< 

run-%: %.o 
	@./$< $(ARGS)