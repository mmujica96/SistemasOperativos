#compilacion del programa principal
programa: main.o
	gcc -g -Wall main.o -o programa
	
main.o: main.c
	gcc -g -Wall -c main.c
	
clean: 
	rm *.o
