# OSX Makefile

rocketeer:
	#cd platforms/osx && IMPL=rocketeer make clean
	cd platforms/osx && IMPL=rocketeer make
	platforms/osx/build/raptor_island

test:
	#cd platforms/osx && IMPL=test make clean
	cd platforms/osx && IMPL=test make
	platforms/osx/build/raptor_island assets/shaders/basic.vsh assets/shaders/flower.fsh

emscripten-rocketeer:
	#cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make clean
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make install
