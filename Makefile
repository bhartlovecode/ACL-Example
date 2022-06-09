CC := g++

all: acl 

acl: acl.o
	$(CC) -o acl acl.o 

acl.o: acl.cpp
	$(CC) -c acl.cpp

clean:
	rm -f acl acl.o
