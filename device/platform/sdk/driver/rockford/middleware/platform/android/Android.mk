# Copyright (C) 2008 The Android Open Source Project
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
REFSW_PATH :=vendor/broadcom/bcm${BCHP_CHIP}/brcm_nexus

LOCAL_PATH := $(call my-dir)

include $(REFSW_PATH)/bin/include/platform_app.inc

GLES_CFLAGS = 

# Should only be defined for enabling multi process openGL
#	-DCLIENT_MODE_IPC \
#	-DKHRONOS_CLIENT_LOGGING

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SHARED_LIBRARIES := liblog libcutils libGLES_nexus libnexus

LOCAL_C_INCLUDES += $(REFSW_PATH)/bin/include \
		    $(REFSW_PATH)/../libGLES_nexus/interface/khronos/include \
		    $(REFSW_PATH)/../brcm_nexus/bin/include \
		    frameworks/base/include/ui \
		    vendor/broadcom/bcm${BCHP_CHIP}/libgralloc \
		    system/core/include

LOCAL_SRC_FILES := 	\
	default_RSO_android.c \
	display_RSO_android.c

LOCAL_MODULE := libandroidpl
LOCAL_MODULE_TAGS := optional debug
LOCAL_CFLAGS:= $(GLES_CFLAGS) $(NEXUS_CFLAGS) -DANDROID
include $(BUILD_SHARED_LIBRARY)
