# OSX Makefile

build=build

CXXFLAGS=\
-m64 \
-Wall \
-g \
-pg \
-I/usr/include \
-I../../src \
-DDESKTOP

CFLAGS=$(CXXFLAGS)
LDFLAGS=-m64 -arch x86_64 -framework OpenGL -framework GLUT -framework Foundation -framework ApplicationServices -framework AppKit -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework Accelerate

objects   = $(patsubst %,build/%, $(patsubst %.mm,%.o, $(wildcard *.mm)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.cpp,%.o, $(wildcard ../../src/*.cpp)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/*.c)))

raptor_island: $(build) $(objects)
	$(CXX) $(LDFLAGS) -o $(build)/raptor_island $(objects)

$(build)/%.o: %.mm
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build)/%.o: ../../src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build)/%.o: ../../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)

clean:
	touch $(build) && rm -R $(build)
