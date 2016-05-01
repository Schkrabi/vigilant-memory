objects = main.o simpleGC.o
ccargs = -g

simpleGC : $(objects)
	cc -o simpleGC $(objects) $(ccargs)
	rm $(objects)
	mv simpleGC ../bin/simpleGC
	
main.o : main.c simpleGC.h
	cc -c main.c $(ccargs)
	
simpleGC.o : simpleGC.c simpleGC.h
	cc -c simpleGC.c $(ccargs)
	
clean : 
	rm simpleGC simpleGC.o main.o
