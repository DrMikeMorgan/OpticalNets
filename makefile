FLTK = $(shell fltk-config --cxx)
CXX = g++
CXXFLAGS = -Wall
FLTKFLAGS = $(shell fltk-config --cxxflags)
LDFLAGS = $(shell fltk-config --ldstaticflags --use-gl)

OBJ = SRGGraph.o OptNet.o PavanGenerator.o Construction.o SRGAnnealing.o SRGAntColony.o GraphWindow.o MainWindow.o main.o

SRGOBJ = SRGGraph.o OptNet.o PavanGenerator.o test1.o

BIOBJ = test2.o SRGGraph.o OptNet.o PavanGenerator.o Construction.o SRGAntColony.o

all: gui srgs 

gui: optnets 

srgs: srgtest

bicon: bitest

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
	$(FLTK) -c $< $(FLTKFLAGS)

MainWindow.o: src/MainWindow.cpp GraphWindow.o Construction.o SRGAnnealing.o SRGAntColony.o
	$(FLTK) -c $< $(FLTKFLAGS)

main.o: main.cpp MainWindow.o
	$(FLTK) -c $< $(FLTKFLAGS)

optnets: $(OBJ)
	$(FLTK) -o $@ $(OBJ) $(LDFLAGS) 

test1.o: test1.cpp SRGGraph.o OptNet.o PavanGenerator.o 
	$(CXX) -c $< $(CXXFLAGS)

test2.o: test2.cpp SRGGraph.o OptNet.o PavanGenerator.o Construction.o SRGAntColony.o
	$(CXX) -c $< $(CXXFLAGS)

srgtest: $(SRGOBJ)
	$(CXX) -o $@ $(SRGOBJ)

bitest: $(BIOBJ)
	$(CXX) -o $@ $(BIOBJ)

clean:
	rm -rf *o optnets
