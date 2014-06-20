# Main Makefile

sidescroll:
	cd platforms/osx && IMPL=sidescroll make
	platforms/osx/build/raptor_island

#sidescroll-assets:~/Desktop/RobotAttackVector/skeleton.* ~/Desktop/RobotAttackVectorBackground/skeleton.*
#	cp ~/Desktop/RobotAttackVectorBackground/bgs.json assets/spine/bgs.json
#	cp ~/Desktop/RobotAttackVectorBackground/bgs.atlas assets/spine/bgs.atlas
#	cp ~/Desktop/RobotAttackVectorBackground/bgs.png assets/spine/bgs.png
#	cp ~/Desktop/RobotAttackVector/robot.json assets/spine/robot.json
#	cp ~/Desktop/RobotAttackVector/robot.atlas assets/spine/robot.atlas
#	cp ~/Desktop/RobotAttackVector/robot.png assets/spine/robot.png

sidescroll-clean:
	cd platforms/osx && IMPL=sidescroll make clean

rocketeer:
	cd platforms/osx && IMPL=rocketeer make
	platforms/osx/build/raptor_island

rocketeer-assets:~/Desktop/RobotAttackVector/skeleton.* ~/Desktop/RobotAttackVectorBackground/skeleton.*
	cp ~/Desktop/RobotAttackVectorBackground/bgs.json assets/spine/bgs.json
	cp ~/Desktop/RobotAttackVectorBackground/bgs.atlas assets/spine/bgs.atlas
	cp ~/Desktop/RobotAttackVectorBackground/bgs.png assets/spine/bgs.png
	cp ~/Desktop/RobotAttackVector/robot.json assets/spine/robot.json
	cp ~/Desktop/RobotAttackVector/robot.atlas assets/spine/robot.atlas
	cp ~/Desktop/RobotAttackVector/robot.png assets/spine/robot.png

rocketeer-clean:
	cd platforms/osx && IMPL=rocketeer make clean

test:
	cd platforms/osx && IMPL=test make
	platforms/osx/build/raptor_island

test-clean:
	cd platforms/osx && IMPL=test make clean

emscripten-sidescroll:
	cd platforms/emscripten && IMPL=sidescroll ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=sidescroll ~/emscripten/emmake make install

emscripten-sidescroll-clean:
	cd platforms/emscripten && IMPL=sidescroll ~/emscripten/emmake make clean

emscripten-rocketeer-clean:
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make clean

emscripten-rocketeer:
	#cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=rocketeer ~/emscripten/emmake make install

emscripten-test:
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make
	cd platforms/emscripten && IMPL=test ~/emscripten/emmake make install

android: android-clean
	cd platforms/android && make
	cd platforms/android && make install

android-install:
	cd platforms/android && make install

android-clean:
	cd platforms/android && make clean

clean: sidescroll-clean rocketeer-clean test-clean android-clean
