CXX = g++
#CXXFLAGS = $(shell fltk-config --cxxflags)
#LDFLAGS = $(shell fltk-config --ldstaticflags --use-gl)

OBJ = SRGGraph.o OptNet.o Construction.o SRGAntColony.o batchTests.o

all: batch

SRGGraph.o: ../src/SRGGraph.cpp 
	$(CXX) -c $< 

OptNet.o: ../src/OptNet.cpp SRGGraph.o
	$(CXX) -c $< 

Construction.o: ../src/Construction.cpp SRGGraph.o
	$(CXX) -c $< 
	
SRGAntColony.o: ../src/SRGAntColony.cpp SRGGraph.o
	$(CXX) -c $< 
	
batchTests.o: batchTests.cpp 
	$(CXX) -c $< 

batch: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

clean:
	rm -rf *o batch
