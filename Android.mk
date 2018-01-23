# Copyright 2015 ikeGPS(NZ) Ltd

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := libOpenCL.so
LOCAL_MODULE := libOpenCL
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := libCLC.so
LOCAL_MODULE := libCLC
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := libVSC.so
LOCAL_MODULE := libVSC
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main.c

LOCAL_SHARED_LIBRARIES := \
	libGAL \
	libOpenCL \
    libCLC \
#	libVSC \

LOCAL_C_INCLUDES += external/mesa3d/include

LOCAL_LDLIBS = -lm
LOCAL_CFLAGS += -std=c99 -DLINUX -DUSE_SOC_MX6 -Wall -O2 -fsigned-char -march=armv7-a -mfpu=neon -mthumb-interwork -mtune=cortex-a9 -mfpu=vfp \
			  -DEGL_API_FB -DGPU_TYPE_VIV -DGL_GLEXT_PROTOTYPES -DENABLE_GPU_RENDER_20
LOCAL_MODULE:= opencl 
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	cl_helloworld.cpp

LOCAL_SHARED_LIBRARIES := \
	libGAL \
	libVSC \
	libOpenCL \
#    libCLC \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \

LOCAL_LDLIBS = -lm
LOCAL_CFLAGS += -DLINUX -DUSE_SOC_MX6 -Wall -O2 -fsigned-char -march=armv7-a -mfpu=neon -mthumb-interwork -mtune=cortex-a9 -mfpu=vfp \
			  -DEGL_API_FB -DGPU_TYPE_VIV -DGL_GLEXT_PROTOTYPES -DENABLE_GPU_RENDER_20
LOCAL_MODULE:= opencl2 
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)


