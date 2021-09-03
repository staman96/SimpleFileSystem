CC = g++

CFSshell: CFSshell.o objects/CFS.o objects/MDS.o
	$(CC) -lm -g CFSshell.o objects/CFS.o objects/MDS.o -o CFSshell

CFS.o: objects/CFS.cpp objects/CFS.h MDS.h
	$(CC) -c -g objects/CFS.cpp /objects

MDS.o: objects/MDS.cpp objects/MDS.h objects/CFS.h
	$(CC) -c -g objects/MDS.cpp /objects

CFSshell.o: CFSshell.cpp objects/CFS.h objects/MDS.h
	$(CC) -c -g CFSshell.cpp

clean:
	rm -rvf *.o CFSshell objects/*.o