CC=g++
TESTFLAGS=-lgtest -pthread
all: final clean

final: main.o all_graphs.o factory.o find_path.o
	$(CC) -o t main.o all_graphs.o factory.o find_path.o $(TESTFLAGS)

main.o: main.cpp all_graphs.h factory.h find_path.h
	$(CC) -c main.cpp $(TESTFLAGS)

find_path.o: find_path.cpp find_path.h
	$(CC) -c find_path.cpp

factory.o: factory.cpp factory.h
	$(CC) -c factory.cpp

all_graphs.o: all_graphs.cpp all_graphs.h
	$(CC) -c all_graphs.cpp

clean:
	rm -rf *.o
