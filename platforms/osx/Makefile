# OSX Makefile

build=build

CXXFLAGS=\
-m64 \
-Wall \
-g \
-pg \
-DDESKTOP \
-DHAVE_SETENV \
-DMODPLUG_BASIC_SUPPORT \
-DMODPLUG_FASTSOUNDLIB \
-D__MACOSX_CORE__ \
-D_iPhoneVersion \
-I/usr/include \
-I../../src \
-I../../src/include \
-I../../src/include/libmodplug \
-I../../src/octree \
-DUSE_GLES2 \
-DHAVE_STDINT_H \
-DHAVE_SINF \


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
