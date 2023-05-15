# This file should be included on top of the application makefile
# on it's output it provides:
# CFLAGS - compiler flags
# SRCS - list of source files
# LDFLAGS - linker options
# MAGNUM_OBJS - list of target object files from magnum VOB
# MAGNUM_INC - list of include path for magnum VOB
# ROCKFORD  - points to the root of ROCKFORD VOB
# also it sets vpath for all source directories
#
# BSEAV shall be defined prior to inclusion of this file 
# ***** THIS FILE SHALL BE INCLUDED INTO THE CUSTOM PROJECT, NOT COPIED *****

all: build-app

B_REFSW_DEBUG ?= y
SHAREDLIB=no
include ${BSEAV}/api/build/tools.mak
include ${BSEAV}/api/build/magnum/bsp.inc
ODIR = ${SYSTEM}${PLATFORM}$(BCHP_VER)_${B_REFSW_ARCH}
ifdef R_OBJ
SRCS +=	$(addsuffix .c, ${notdir ${R_OBJ}})
vpath %.c $(sort $(dir ${R_OBJ}))
endif
CFLAGS += $(addprefix -I, $(sort ${R_INC}))

LDFLAGS += ${BSETTOP_LIB} -lpthread

