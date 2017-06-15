# Main Makefile

ntm:
	cd platforms/osx && IMPL=networked-tilemap-maze make
	platforms/osx/build/raptor_island

linux-ntm:
	cd platforms/linux && IMPL=networked-tilemap-maze make
	platforms/linux/build/raptor_island

ntm-clean:
	cd platforms/osx && IMPL=networked-tilemap-mze make clean


sidescroll:
	cd platforms/osx && IMPL=sidescroll make
	platforms/osx/build/raptor_island

sidescroll-clean:
	cd platforms/osx && IMPL=sidescroll make clean

linux-sidescroll:
	cd platforms/linux && IMPL=sidescroll make
	platforms/linux/build/raptor_island

linux-sidescroll-clean:
	cd platforms/linux && IMPL=sidescroll make clean

rocketeer:
	cd platforms/osx && IMPL=rocketeer make
	platforms/osx/build/raptor_island

rocketeer-clean:
	cd platforms/osx && IMPL=rocketeer make clean

linux-rocketeer:
	cd platforms/linux && IMPL=rocketeer make
	platforms/linux/build/raptor_island

linux-rocketeer-clean:
	cd platforms/linux && IMPL=rocketeer make clean

test:
	cd platforms/osx && IMPL=test make
	platforms/osx/build/raptor_island

test-clean:
	cd platforms/osx && IMPL=test make clean

linux-test:
	cd platforms/linux && IMPL=test make
	platforms/linux/build/raptor_island

linux-test-clean:
	cd platforms/linux && IMPL=test make clean

emscripten-ntm:
	cd platforms/emscripten && IMPL=networked-tilemap-maze emmake make
	cd platforms/emscripten && IMPL=networked-tilemap-maze emmake make install

emscripten-ntm-clean:
	cd platforms/emscripten && IMPL=networked-tilemap-maze emmake make clean

emscripten-sidescroll:
	cd platforms/emscripten && PATH=${PATH}:~/emscripten IMPL=sidescroll emmake make
	cd platforms/emscripten && PATH=${PATH}:~/emscripten IMPL=sidescroll emmake make install

emscripten-sidescroll-clean:
	cd platforms/emscripten && IMPL=sidescroll emmake make clean

emscripten-rocketeer:
	cd platforms/emscripten && IMPL=rocketeer emmake make
	cd platforms/emscripten && IMPL=rocketeer emmake make install

emscripten-rocketeer-clean:
	cd platforms/emscripten && IMPL=rocketeer emmake make clean

emscripten-test:
	cd platforms/emscripten && PATH=${PATH}:~/emscripten IMPL=test emmake make
	cd platforms/emscripten && PATH=${PATH}:~/emscripten IMPL=test emmake make install

emscripten-test-clean:
	cd platforms/emscripten && PATH=${PATH}:~/emscripten IMPL=test emmake make clean

android: android-clean
	cd platforms/android && make
	cd platforms/android && make install

android-install:
	cd platforms/android && make install

android-clean:
	cd platforms/android && make clean

clean: sidescroll-clean rocketeer-clean test-clean android-clean emscripten-test-clean emscripten-rocketeer-clean emscripten-sidescroll-clean emscripten-ntm-clean
