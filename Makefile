CXXFLAGS += --std=c++11 -DGLM_FORCE_RADIANS
LDLIBS += -lGLEW -lGL -lSDL2 -lassimp

default: ptep

ptep: ptep.o glUtil.o modelLoader.o readFile.o shaderLoader.o
	$(CXX) -g $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm *.o
	rm ptep

$PHONY: default clean
