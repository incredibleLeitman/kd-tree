CXX = g++
#INCLUDES = -I./includes
#LIBRARIES = -L./libs

CXXFLAGS = -Wall -O3 $(INCLUDES)
#LDFLAGS = $(LIBRARIES) -lglfw3 -lopengl32 -lglu32 -lgdi32

kdtree: main.o kdtree.o
	$(CXX) $(CXXFLAGS) -o kdtree main.o kdtree.o $(LDFLAGS)

main.o: main.cpp kdtree.h
	$(CXX) $(CXXFLAGS) -c main.cpp $(LDFLAGS)

kdtree.o: kdtree.cpp

clean :
	-rm *.o kdtree
