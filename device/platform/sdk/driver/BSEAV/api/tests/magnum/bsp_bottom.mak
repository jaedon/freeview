# This file sould be included on bottom of the application makefile
# this files uese:
# CFLAGS - compiler flags
# SRCS - list of source files
# LDFLAGS - linker options
# APP - name of application
# OBJ - list of target object files from magnum VOB (as defined in the rockford/modules/foo.inc)
# INC - list of include path for magnum VOB (as defined in the rockford/modules/foo.inc)
# it also expects vpath for all source directories
# 
# and defines: application and clean targets to build application
#
# ***** THIS FILE SHALL BE INCLUDED INTO THE CUSTOM PROJECT, NOT COPIED *****

INC += commonutils/sur/${BCHP_CHIP}
INC += commonutils/fmt
INC += commonutils/pxl

include $(ROCKFORD)/inc/compose.mak
CFLAGS += ${MAGNUM_INC} 
OBJS += ${MAGNUM_OBJS}


include ${BSEAV}/api/build/rules.mak

build-app: application
	@echo "Build completed: ${APP_IMAGE}"

