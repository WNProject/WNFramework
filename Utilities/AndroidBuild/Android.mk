# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


APP_LOCAL_PATH := $(call my-dir)
include ##WN_PROJECT_PATH##/public/Libraries/jni/Android.mk
##WN_ADDITIONAL_ANDROID_LIBS##
LOCAL_PATH := $(APP_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp
LOCAL_MODULE    := ##LIB_NAME## 
LOCAL_SRC_FILES := ##WN_SOURCES##
LOCAL_CFLAGS  := -std=c++11 -I##WN_PROJECT_PATH##/public/Libraries ##WN_ADDITIONAL_CFLAGS##
LOCAL_LDLIBS    := -lm -llog -landroid -lEGL -lGLESv2
LOCAL_WHOLE_STATIC_LIBRARIES := ##WN_FORCED_LIBRARIES##
LOCAL_STATIC_LIBRARIES := android_native_app_glue ##WN_LIBRARIES##
ADDITIONAL_LIBS := $(if $(filter WNScripting,$(LOCAL_STATIC_LIBRARIES)),$(shell ##ANDROID_LLVM_CONFIG## --libs)) 
ANDROID_LIB_LOC := $(if $(filter WNScripting,$(LOCAL_STATIC_LIBRARIES)),$(shell ##ANDROID_LLVM_CONFIG## --libdir))
LOCAL_LDFLAGS := -L$(ANDROID_LIB_LOC) -L$(APP_LOCAL_PATH) $(ADDITIONAL_LIBS) -Lobj/local/armeabi/ 


include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
