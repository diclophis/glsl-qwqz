# OSX Makefile, needs to know where the main src root is

TOTAL_ALLOWED_RAM=65536

build=build

CXXFLAGS=\
-O3 \
-I../../src \
-DDESKTOP \

CFLAGS=$(CXXFLAGS)

objects = $(patsubst %,build/%, $(patsubst %.cpp,%.bc, $(wildcard *.cpp)))
objects += $(patsubst ../../src/%,build/%, $(patsubst %.cpp,%.bc, $(wildcard ../../src/*.cpp)))
objects += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/*.c)))

HASH=qwqz-$(shell md5sum build/raptor_island.js | cut -d" " -f 1)
hash_dir=~/emscripten.risingcode.com/public/$(HASH)

raptor_island: $(build) $(objects)
	EMCC_DEBUG=1 $(CXX) -O3 $(objects) -o $(build)/raptor_island.bc
	EMCC_DEBUG=1 $(CXX) -O3 -s ASM_JS=1 -s PRECISE_I64_MATH=1 -s TOTAL_MEMORY=$(TOTAL_ALLOWED_RAM) -s TOTAL_STACK=$(TOTAL_ALLOWED_RAM) -s FAST_MEMORY=$(TOTAL_ALLOWED_RAM) -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free', '_start_game', '_command']" $(build)/raptor_island.bc -o $(build)/raptor_island.js

$(build)/%.bc: %.cpp
	EMCC_DEBUG=1 $(CXX) $(CXXFLAGS) -c $< -o $@

$(build)/%.bc: ../../src/%.cpp
	EMCC_DEBUG=1 $(CXX) $(CXXFLAGS) -c $< -o $@

$(build)/%.bc: ../../src/%.c
	EMCC_DEBUG=1 $(CC) $(CFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)

clean:
	touch /tmp/emscripten_temp && rm -R /tmp/emscripten_temp
	touch $(build) && rm -Rf $(build)

$(hash_dir):
	mkdir $(hash_dir)

install: $(hash_dir) build/raptor_island.js
	ln -nsf $(hash_dir) ~/emscripten.risingcode.com/public/latest-qwqz
	ruby cache_manifest.rb ../../assets > ~/emscripten.risingcode.com/public/latest-qwqz/index.appcache
	cp -R ../../assets ~/emscripten.risingcode.com/public/latest-qwqz
	touch ~/emscripten.risingcode.com/public/latest-qwqz/favicon.ico
	install -C index.html ~/emscripten.risingcode.com/public/latest-qwqz
	install -C $(build)/raptor_island.js ~/emscripten.risingcode.com/public/latest-qwqz
