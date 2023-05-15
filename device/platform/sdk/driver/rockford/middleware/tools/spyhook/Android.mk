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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

ifeq ($(BOARD_USES_BROADCOM_VIDEOCORE),true)
VC_TOP := $(TOP)/$(BROADCOM_PROPRIETARY_VIDEOCORE_ROOT)
include $(VC_TOP)/vcfw/platform/broadcom/bcm911160-tablet/shared-config.mk
endif

##LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_SHARED_LIBRARIES := liblog libcutils libstlport

LOCAL_C_INCLUDES += external/stlport/stlport bionic \
                    bionic/libstdc++/include \
                    $(LOCAL_PATH) \
                    vendor/broadcom/bcm${BCHP_CHIP}/libGLES_nexus \
                    vendor/broadcom/bcm${BCHP_CHIP}/libGLES_nexus/interface/khronos/include

ifeq ($(BOARD_USES_BROADCOM_VIDEOCORE),true)
LOCAL_C_INCLUDES += $(VC_TOP) \
                    $(VC_TOP)/interface/$(VCHIQ_FLAVOUR) \
                    $(VC_TOP)/interface/vmcs_host \
                    $(VC_TOP)/interface/vmcs_host/linux \
                    $(VC_TOP)/interface/vcos/pthreads
endif

LOCAL_SRC_FILES := \
   spyhook.cpp	\
   remote.cpp \
   packet.cpp \
	archive.cpp \
   platform.cpp

LOCAL_MODULE := libgpumonitor
LOCAL_MODULE_TAGS:= optional
LOCAL_CFLAGS:= -DANDROID
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

include $(BUILD_SHARED_LIBRARY)
