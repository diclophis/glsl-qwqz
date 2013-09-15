# OSX Makefile

IMPL=test

osx:
	cd platforms/osx && IMPL=$(IMPL) make clean
	cd platforms/osx && IMPL=$(IMPL) make
	platforms/osx/build/raptor_island assets/shaders/basic.vsh assets/shaders/flower.fsh
