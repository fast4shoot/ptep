CXXFLAGS += --std=c++11 -DGLM_FORCE_RADIANS
LDLIBS += -lGL -lGLEW -lSDL2 -lassimp

default: ptep

ptep: ptep.o
	$(CXX) $(LDLIBS) $(CXXFLAGS) $^ -o $@

$PHONY: default
