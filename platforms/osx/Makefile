# OSX Makefile

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
-DNDEBUG

CFLAGS=$(CXXFLAGS)
LDFLAGS=-m64 -arch x86_64 -framework OpenGL -framework GLUT -framework Foundation -framework ApplicationServices -framework AppKit -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework Accelerate

objects   = $(patsubst %,build/%, $(patsubst %.m,%.o, $(wildcard *.m)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/zlib/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/contrib/chipmunk/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../src/*.c)))
objects   += $(patsubst ../../$(impl)-src/%,build/%, $(patsubst %.c,%.o, $(wildcard ../../$(impl)-src/*.c)))

raptor_island: $(build) $(objects)
	$(CC) $(LDFLAGS) -o $(build)/raptor_island $(objects)

$(build)/%.o: %.m
	$(CC) $(CXXFLAGS) -c $< -o $@

$(build)/%.o: ../../$(impl)-src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.o: ../../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)
	mkdir -p $(build)/contrib/zlib
	mkdir -p $(build)/contrib/chipmunk

clean:
	touch $(build) && rm -R $(build)
