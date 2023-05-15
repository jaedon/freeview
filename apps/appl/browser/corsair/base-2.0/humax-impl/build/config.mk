
##################################
# Include Main configuration
###################################
-include ../../config.mk

###################################
# Included from Makefile
###################################


###################################
# Path to Linux SDK package and OOIF
##
GOGI_SDK_PATH ?= ../..
OOIF_PATH = $(GOGI_SDK_PATH)/jsplugins/ooif


###################################
# Selected components
##
SELECTED_COMPONENTS := corsair

.PHONY: default
default: corsair

USE_VEGA_DFB	:= YES
OIPF_BUILD		:= YES
OIPF_WIDGETS_BUILD := YES
USE_DIRECTFBGL	:= NO



ifeq ($(CONFIG_DEBUG),y)
CORSAIR_SEGV_HANDLER_WITH_TRACEBACK	:= YES
endif

ifneq ($(CORSAIR_DSMCC_EXTERNAL),YES)
	CORSAIR_DSMCC_DUMMY_IMPLEMENTATION := YES
endif
CORSAIR_INPUT_BROADCOM := YES
#CORSAIR_DSMCC_EXTERNAL := YES
#CORSAIR_AIT_DUMMY_IMPLEMENTATION := YES
CORSAIR_EXTRA_LIBPATH 		 := ../$(PLATFORM)/$(GCC_LIBC_STRING)/lib

###################################
# Default installation PREFIX
##
ifeq ($(PREFIX),)
 PREFIX := ${PWD}/..
endif


###################################
# Default installation subfolders
##
INSTALL_DIR_SHARED_LIBRARY	:= $(PREFIX)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib
INSTALL_DIR_STATIC_LIBRARY	:= $(PREFIX)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib
INSTALL_DIR_EXECUTABLE		:= $(PREFIX)/$(PLATFORM)/$(GCC_LIBC_STRING)/bin
INSTALL_DIR_DOCUMENTATION	:= $(PREFIX)/doc


###################################
# Component directory
##
COMPONENT_DIR 			:= ../components
