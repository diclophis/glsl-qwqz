# OSX Makefile

rocketeer: ~/Desktop/RobotAttackVector/skeleton.* ~/Desktop/RobotAttackVectorBackground/skeleton.*
	#cd platforms/osx && IMPL=rocketeer make clean
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.json assets/spine/bgs.json
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.atlas assets/spine/bgs.atlas
	cp ~/Desktop/RobotAttackVectorBackground/skeleton.png assets/spine/bgs.png
	cp ~/Desktop/RobotAttackVector/skeleton.json assets/spine/robot.json
	cp ~/Desktop/RobotAttackVector/skeleton.atlas assets/spine/robot.atlas
	cp ~/Desktop/RobotAttackVector/skeleton.png assets/spine/robot.png
	cd platforms/osx && IMPL=rocketeer make
	platforms/osx/build/raptor_island

test:
	#cd platforms/osx && IMPL=test make clean
	cd platforms/osx && IMPL=test make
	platforms/osx/build/raptor_island assets/shaders/basic.vsh assets/shaders/flower.fsh

emscripten-rocketeer:
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make clean
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make install

emscripten-test:
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make clean
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make install
