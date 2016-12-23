# Linux Makefile

build=build
impl=$(IMPL)

CXXFLAGS=\
-m64 \
-Wall \
-g \
-pg \
-I/usr/include \
-I../../src \
-I../../src/contrib \
-I../../$(impl)-src \
-DDESKTOP \
-DNDEBUG \
-DHAVE_SETENV \
-DMODPLUG_BASIC_SUPPORT \
-DMODPLUG_FASTSOUNDLIB \
-DHAVE_STDINT_H \
-DHAVE_SINF
#-DEMSCRIPTEN \

CFLAGS=$(CXXFLAGS)
LDFLAGS=-lpthread -lresolv -lGL -lglut -lGLU -lm -lstdc++ -lopenal

objects   = $(patsubst %,build/%, $(patsubst %.c,%.o, $(wildcard *.c)))

objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/zlib/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/chipmunk/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/spine/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/pt/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.cpp,%.o, $(wildcard ../../src/contrib/libmodplug/*.cpp)))

objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/*.c)))

objects   += $(patsubst ../../$(impl)-src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../$(impl)-src/*.c)))

raptor_island: $(build) $(objects)
	$(CXX) -o $(build)/raptor_island $(objects) $(LDFLAGS)

$(build)/%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

$(build)/%.o: ../../$(impl)-src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.o: ../../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.o: ../../src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)
	mkdir -p $(build)/contrib/zlib
	mkdir -p $(build)/contrib/chipmunk
	mkdir -p $(build)/contrib/spine
	mkdir -p $(build)/contrib/libmodplug

clean:
	touch $(build) && rm -R $(build)
