# OSX Makefile

osx:
	cd platforms/osx && make clean && make
	platforms/osx/build/raptor_island assets/shaders/basic.vsh assets/shaders/basic.fsh
