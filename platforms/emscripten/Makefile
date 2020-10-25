# OSX Makefile, needs to know where the main src root is

#TOTAL_ALLOWED_RAM=8388608

checkout=/home/app
build=build
impl=$(IMPL)

OPTL=-O3

CXXFLAGS=\
$(OPTL) \
-I$(checkout)/src \
-I$(checkout)/src/contrib \
-I$(checkout)/$(impl)-src \
-DEMSCRIPTEN \
-DDESKTOP \
-DNDEBUG \
-DHAVE_SETENV \
-DMODPLUG_BASIC_SUPPORT \
-DMODPLUG_FASTSOUNDLIB \
-DHAVE_STDINT_H \
-DHAVE_SINF \
-DHAVE_OPENAL

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
hash_dir=/var/www/html/$(HASH)

$(build)/raptor_island_$(impl).js: $(build)/raptor_island_$(impl).bc
	#EMCC_DEBUG=1 $(CXX) $(OPTL) -lopenal --closure 0 -s WASM=1 -s FORCE_ALIGNED_MEMORY=0 -s FORCE_FILESYSTEM=1 -s NO_EXIT_RUNTIME=1 -std=c++11 -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free']" $(build)/raptor_island_$(impl).bc -o $(build)/raptor_island_$(impl).js --js-library library_window.js
	$(CXX) $(OPTL) -lopenal -s NO_EXIT_RUNTIME=1 -std=c++11 -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free']" $(build)/raptor_island_$(impl).bc -o $(build)/raptor_island_$(impl).js --js-library library_window.js --preload-file ../../assets

$(build)/raptor_island_$(impl).bc: $(objects)
	$(CXX) $(OPTL) $(objects) -std=c++11 -o $(build)/raptor_island_$(impl).bc

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
	ruby cache_manifest.rb ../../assets/shaders ../../assets/sounds ../../assets/spine > $(hash_dir)/index.appcache
	cp -R ../../assets $(hash_dir)
	install -C index.html $(hash_dir)
	install -C $(build)/raptor_island_$(impl).js $(hash_dir)/raptor_island.js
	install -C $(build)/raptor_island_$(impl).wasm $(hash_dir)/raptor_island_$(impl).wasm
	install -C $(build)/raptor_island_$(impl).data $(hash_dir)/raptor_island_$(impl).data
	(test -f $(build)/raptor_island_$(impl).js.mem && install -C $(build)/raptor_island_$(impl).js.mem $(hash_dir)/) || true
	install -C memoryprofiler.js $(hash_dir)/
	ln -nsf $(hash_dir) /var/www/html/latest-qwqz
