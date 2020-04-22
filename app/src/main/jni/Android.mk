JNI_PATH := $(call my-dir)
LOCAL_PATH := $(JNI_PATH)

DLIB_PATH := $(LOCAL_PATH)/dlib
TRAFFICSIGN_PATH := $(LOCAL_PATH)/TrafficSign

# Just build the Android.mk files in the subdirs
include $(call all-subdir-makefiles)

LOCAL_PATH := $(JNI_PATH)
include $(CLEAR_VARS)


USER_LOCAL_CFLAGS := -NDEBUG

# To compile the OpenCV
OPENCV_ROOT:=/home/badricio/Downloads/opencv-4.3.0/OpenCV-android-sdk
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCV_ROOT}/sdk/native/jni/OpenCV.mk

LOCAL_MODULE     := adas

LOCAL_SRC_FILES  += \
    adas.cpp \
    TrafficSign/Detector_dlib.cpp
    #TrafficSign/Monitor.cpp \
    #TrafficSign/Processor.cpp

LOCAL_C_INCLUDES += \
   $(LOCAL_PATH) \
   $(TRAFFICSIGN_PATH) \
   $(DLIB_PATH)


LOCAL_LDLIBS    += -landroid -llog -ldl

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
