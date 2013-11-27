CXX = $(shell fltk-config --cxx)
CXXFLAGS = $(shell fltk-config --cxxflags)
LDFLAGS = $(shell fltk-config --ldstaticflags --use-gl)

OBJ = SRGGraph.o OptNet.o SRGDisplay.o OptNetDisplay.o GraphWindow.o main.o

all: optnets 

SRGGraph.o: src/SRGGraph.cpp 
	$(CXX) -c $< $(CXXFLAGS)

OptNet.o: src/OptNet.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

SRGDisplay.o: src/SRGDisplay.cpp SRGGraph.o
	$(CXX) -c $< $(CXXFLAGS)

OptNetDisplay.o: src/OptNetDisplay.cpp OptNet.o
	$(CXX) -c $< $(CXXFLAGS)

GraphWindow.o: src/GraphWindow.cpp SRGDisplay.o OptNetDisplay.o
	$(CXX) -c $< $(CXXFLAGS)

main.o: main.cpp GraphWindow.o
	$(CXX) -c $< $(CXXFLAGS)

optnets: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) 

clean:
	rm -rf *o optnets
