# Linux Makefile

rootsrc=/home/ubuntu/glsl-qwqz/src
rootroot=/home/ubuntu/glsl-qwqz

build=build
impl=$(IMPL)

CXXFLAGS=\
-m64 \
-Wall \
-g \
-pg \
-O0 \
-I/usr/include \
-I$(rootsrc) \
-I$(rootsrc)/contrib \
-I$(rootroot)/$(impl)-src \
-DDESKTOP \
-DNDEBUG \
-DHAVE_SETENV \
-DMODPLUG_BASIC_SUPPORT \
-DMODPLUG_FASTSOUNDLIB \
-DHAVE_STDINT_H \
-DHAVE_SINF \
-DHAVE_OPENAL

CFLAGS=$(CXXFLAGS)
LDFLAGS=-lpthread -lresolv -lGL -lglut -lGLU -lm -lstdc++ -lopenal

objects   = $(patsubst %,build/%, $(patsubst %.c,%.o, $(wildcard *.c)))

objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootsrc)/contrib/zlib/*.c)))
objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootsrc)/contrib/chipmunk/*.c)))
objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootsrc)/contrib/spine/*.c)))
objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootsrc)/contrib/pt/*.c)))
objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.cpp,%.o, $(wildcard $(rootsrc)/contrib/libmodplug/*.cpp)))

objects   += $(patsubst $(rootsrc)/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootsrc)/*.c)))

objects   += $(patsubst $(rootroot)/$(impl)-src/%,build/%, $(patsubst %.c,%.o, $(wildcard $(rootroot)/$(impl)-src/*.c)))

raptor_island: $(build) $(objects)
	$(CXX) -o $(build)/raptor_island $(objects) $(LDFLAGS)

$(build)/%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

$(build)/%.o: $(rootroot)/$(impl)-src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.o: $(rootsrc)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.o: $(rootsrc)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)
	mkdir -p $(build)/contrib/zlib
	mkdir -p $(build)/contrib/chipmunk
	mkdir -p $(build)/contrib/spine
	mkdir -p $(build)/contrib/libmodplug

clean:
	touch $(build) && rm -R $(build)
