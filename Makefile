CXXFLAGS += --std=c++11 -DGLM_FORCE_RADIANS
LDLIBS += -lGLEW -lGL -lSDL2 -lassimp

default: ptep

ptep: ptep.o
	$(CXX) -g $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$PHONY: default
