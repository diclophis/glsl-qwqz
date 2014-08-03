# OSX Makefile, needs to know where the main src root is

#TOTAL_ALLOWED_RAM=8388608

build=build
impl=$(IMPL)

OPTL=O0

CXXFLAGS=\
-$(OPTL) \
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

CFLAGS=$(CXXFLAGS)

objects   = $(patsubst %,build/%, $(patsubst %.c,%.bc, $(wildcard *.c)))

objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/zlib/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/chipmunk/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/spine/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/pt/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.cpp,%.bc, $(wildcard ../../src/contrib/libmodplug/*.cpp)))

objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/*.c)))

objects   += $(patsubst ../../$(impl)-src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../$(impl)-src/*.c)))

HASH=qwqz-$(shell test -e build/raptor_island_$(impl).js && md5sum build/raptor_island_$(impl).js | cut -d" " -f 1 || echo 0)
hash_dir=~/emscripten.risingcode.com/public/$(HASH)

$(build)/raptor_island_$(impl).js: $(build)/raptor_island_$(impl).bc
	$(CXX) -$(OPTL) -s TOTAL_MEMORY=32000000 -std=c++11 -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free']" $(build)/raptor_island_$(impl).bc -o $(build)/raptor_island_$(impl).js --js-library library_window.js

$(build)/raptor_island_$(impl).bc: $(objects)
	$(CXX) -$(OPTL) $(objects) -std=c++11 -o $(build)/raptor_island_$(impl).bc

$(build)/%.bc: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

$(build)/%.bc: ../../$(impl)-src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.bc: ../../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.bc: ../../src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	touch /tmp/emscripten_temp && rm -R /tmp/emscripten_temp
	touch $(build) && rm -Rf $(build)
	mkdir -p $(build)
	mkdir -p $(build)/contrib/zlib
	mkdir -p $(build)/contrib/chipmunk
	mkdir -p $(build)/contrib/spine
	mkdir -p $(build)/contrib/pt
	mkdir -p $(build)/contrib/libmodplug

install: $(build)/raptor_island_$(impl).js
	mkdir -p $(hash_dir)
	ruby cache_manifest.rb ../../assets/spine ../../assets/shaders ../../assets/sounds > $(hash_dir)/index.appcache
	cp -R ../../assets $(hash_dir)
	ln -nsf ~/favicon.ico $(hash_dir)/favicon.ico
	install -C index.html $(hash_dir)
	install -C $(build)/raptor_island_$(impl).js $(hash_dir)/raptor_island.js
	ln -nsf ~/favicon.ico ~/emscripten.risingcode.com/public/favicon.ico
	ln -nsf $(hash_dir) ~/emscripten.risingcode.com/public/latest-qwqz
	touch ~/emscripten.risingcode.com/public/latest-qwqz
