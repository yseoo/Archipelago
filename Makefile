OUT = projet
CXX = g++
CXXFLAGS = -Wall -std=c++11 
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = projet.o error.o tools.o ville.o noeud.o gui.o graphic.o

all: $(OUT)

projet.o: projet.cc ville.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

ville.o: ville.cc ville.h noeud.h tools.h error.h
	$(CXX) $(CXXFLAGS) -c ville.cc -o ville.o

tools.o: tools.cc tools.h graphic_partial.h
	$(CXX) $(CXXFLAGS) -c tools.cc -o tools.o
	
error.o: error.cc error.h constantes.h
	$(CXX) $(CXXFLAGS) -c error.cc -o error.o

noeud.o: noeud.cc noeud.h tools.h constantes.h error.h
	$(CXX) $(CXXFLAGS) -c noeud.cc -o noeud.o

gui.o: gui.cc gui.h ville.h graphic_complete.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
graphic.o: graphic.cc graphic_partial.h graphic_complete.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

clean:
	@echo "Cleaning compilation files"
	@rm $(OFILES) $(OUT) 
