# OSX Makefile

rocketeer:
	cd platforms/osx && IMPL=rocketeer make
	platforms/osx/build/raptor_island

rocketeer-assets:~/Desktop/RobotAttackVector/skeleton.* ~/Desktop/RobotAttackVectorBackground/skeleton.*
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.json assets/spine/bgs.json
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.atlas assets/spine/bgs.atlas
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.png assets/spine/bgs.png
	cp ~/Desktop/RobotAttackVector/skeleton.json assets/spine/robot.json
	cp ~/Desktop/RobotAttackVector/skeleton.atlas assets/spine/robot.atlas
	cp ~/Desktop/RobotAttackVector/skeleton.png assets/spine/robot.png

rocketeer-clean:
	cd platforms/osx && IMPL=rocketeer make clean

test:
	cd platforms/osx && IMPL=test make
	platforms/osx/build/raptor_island assets/shaders/basic.vsh assets/shaders/flower.fsh

test-clean:
	cd platforms/osx && IMPL=test make clean

emscripten-rocketeer:
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make clean
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make install

emscripten-test:
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make clean
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make install
