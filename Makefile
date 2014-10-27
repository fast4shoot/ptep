CXXFLAGS += --std=c++11
LDLIBS += -lGL -lGLEW -lSDL2

default: ptep

ptep: ptep.o
	$(CXX) $(LDLIBS) $(CXXFLAGS) $^ -o $@

$PHONY: default
