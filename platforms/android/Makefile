CRYSTAX=~/android-ndk-r8-crystax-1

debug:
	IMPL=$(IMPL) $(CRYSTAX)/ndk-build
	IMPL=$(IMPL) ant debug

install:
	ant uninstall
	ant installd

clean:
	IMPL=$(IMPL) $(CRYSTAX)/ndk-build clean
	ant clean
