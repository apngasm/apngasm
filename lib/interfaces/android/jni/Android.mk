# File: Android.mk
LOCAL_PATH := $(call my-dir)
OR_LIB_PATH := LOCAL_PATH
OR_INCLUDE_PATH := LOCAL_PATH
OR_CPP_SOURCES :=

include $(CLEAR_VARS)

LOCAL_MODULE    := apngasm-android
LOCAL_SRC_FILES := apng_wrap.cpp $(OR_CPP_SOURCES)
LOCAL_CPP_FEATURES += exceptions rtti
#LOCAL_CFLAGS    := -frtti -fexceptions
LOCAL_C_INCLUDES += $(OR_INCLUDE_PATH)/$(TARGET_ARCH)

#$(call import-add-path, $(OR_LIB_PATH))

#LOCAL_PATH += $(OR_LIB_PATH)/$(TARGET_ARCH)
LOCAL_LDLIBS 	+= -L$(OR_LIB_PATH)/$(TARGET_ARCH) -lpng -lz
LOCAL_SHARED_LIBRARIES += png
LOCAL_STATIC_LIBRARIES += boost_system_static boost_filesystem_static boost_program_options_static boost_regex_static z

include $(BUILD_SHARED_LIBRARY)

$(call import-module,boost/1.59.0)
