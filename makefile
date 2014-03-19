CXX = $(shell fltk-config --cxx)
CXXFLAGS = $(shell fltk-config --cxxflags)
LDFLAGS = $(shell fltk-config --ldstaticflags --use-gl)

OBJ = SRGGraph.o OptNet.o PavanGenerator.o Construction.o SRGAnnealing.o SRGAntColony.o GraphWindow.o MainWindow.o main.o

all: optnets 

SRGGraph.o: src/SRGGraph.cpp 
	$(CXX) -c $< $(CXXFLAGS)

OptNet.o: src/OptNet.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

PavanGenerator.o: src/PavanGenerator.cpp OptNet.o
	$(CXX) -c $< $(CXXFLAGS)

Construction.o: src/Construction.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

SRGAnnealing.o: src/SRGAnnealing.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

SRGAntColony.o: src/SRGAntColony.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

GraphWindow.o: src/GraphWindow.cpp SRGGraph.o OptNet.o PavanGenerator.o
	$(CXX) -c $< $(CXXFLAGS)

MainWindow.o: src/MainWindow.cpp GraphWindow.o Construction.o SRGAnnealing.o SRGAntColony.o
	$(CXX) -c $< $(CXXFLAGS)

main.o: main.cpp MainWindow.o
	$(CXX) -c $< $(CXXFLAGS)

optnets: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) 

clean:
	rm -rf *o optnets
