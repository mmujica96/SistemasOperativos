all: init time timex

init: 
	mkdir -p build

time: 
	gcc -o build/time timeshmm.c
	
timex: 
	gcc -o build/time timepipe.c
	
clean:
	 rm build/*
