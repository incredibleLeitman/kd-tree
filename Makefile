CXX = g++
CXXFLAGS = -Wall -O3

kdtree: main.o kdtree.o
	$(CXX) $(CXXFLAGS) -o kdtree main.o kdtree.o

main.o: main.cpp kdtree.h
	$(CXX) $(CXXFLAGS) -c main.cpp

kdtree.o: kdtree.cpp

clean :
	-rm *.o kdtree
