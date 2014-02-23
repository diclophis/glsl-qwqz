LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := qwqz

TARGET_ARCH=arm
TARGET_ARCH_ABI=arm
LOCAL_ARM_MODE=arm

LOCAL_CFLAGS := -g -Wall -std=c99 -DDEBUGBUILD -I../../src -I../../src/contrib -DANDROID -DANDROID_NDK -DEV_STANDALONE=1 -DEV_USE_SELECT=1 -DEV_SELECT_USE_FD_SET

CG_SUBDIRS := \
. \
../../../$(IMPL)-src \
../../../src \
../../../src/contrib/chipmunk \
../../../src/contrib/spine \
../../../src/contrib/unzip \
../../../src/contrib/zlib

LOCAL_SRC_FILES := $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.cpp))))
LOCAL_SRC_FILES += $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))

LOCAL_LDLIBS := -lGLESv2 -ldl -llog -lc -lgcc -lm -ldl -lstdc++ -landroid

include $(BUILD_SHARED_LIBRARY)
