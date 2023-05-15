################################################################################
# common.mk
################################################################################

################################################################################
# 64-bit Machine Test
################################################################################
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
M32=-m32
else
M32=
endif


################################################################################
# 플랫폼이 정해지지 않으면 컴파일 하지 않음.
# env파일을 실행해줘야합니다. ( # source TARGET.env )
ifndef MXLIB_TARGET
$(error export MXLIB_TARGET=BASE/8634/HUMAX/HUMAX2/TOPFIELD/SHEEVA/STLINUX)
endif
################################################################################

################################################################################
# 컴파일러 설정.
# MIPS = 1, ARM = 2
ELSE_TARGET = 1


ifeq "$(MXLIB_TOOLCHAIN)" "arm-linux-uclibcgnueabi"
  ELSE_TARGET = 2
endif

ifeq "$(MXLIB_TOOLCHAIN)" "arm-linux-gnueabihf"
  ELSE_TARGET = 3
endif

ifeq "$(MXLIB_TOOLCHAIN)" "mips-linux-uclibc"
  ELSE_TARGET = 4
endif


ifeq "$(MXLIB_TARGET)" "BASE"
CC			=	gcc $(M32)
CXX      	=	g++ $(M32)
LD			=	gcc $(M32)
AR			=	ar rc
RANLIB		=   ranlib
ELSE_TARGET = 0
endif

ifeq "$(MXLIB_TARGET)" "SHEEVA"
CC			=	arm-none-linux-gnueabi-gcc
LD			=	arm-none-linux-gnueabi-gcc
AR			=	arm-none-linux-gnueabi-ar rc
RANLIB		=	arm-none-linux-gnueabi-ranlib
ELSE_TARGET = 0
endif

ifeq "$(MXLIB_TARGET)" "STLINUX"
CC			=	sh4-linux-gcc
LD			=	sh4-linux-gcc
AR			=	sh4-linux-ar rc
RANLIB		=	sh4-linux-ranlib
ELSE_TARGET = 0
endif

ifeq ($(ELSE_TARGET) , 1) 
CC			=	mipsel-linux-gcc
CXX			=	mipsel-linux-g++
LD			=	mipsel-linux-gcc
AR			=	mipsel-linux-ar rc
RANLIB		=	mipsel-linux-ranlib
endif

ifeq ($(ELSE_TARGET) , 2) 
CC			=	arm-linux-uclibcgnueabi-gcc
CXX			=	arm-linux-uclibcgnueabi-g++
LD			=	arm-linux-uclibcgnueabi-gcc
AR			=	arm-linux-uclibcgnueabi-ar rc
RANLIB		=	arm-linux-uclibcgnueabi-ranlib
endif

ifeq ($(ELSE_TARGET) , 3) 
CC			=	arm-linux-gnueabihf-gcc
CXX			=	arm-linux-gnueabihf-g++
LD			=	arm-linux-gnueabihf-gcc
AR			=	arm-linux-gnueabihf-ar rc
RANLIB		=	arm-linux-gnueabihf-ranlib
endif

ifeq ($(ELSE_TARGET) , 4) 
CC			=	mips-linux-uclibc-gcc
CXX			=	mips-linux-uclibc-g++
LD			=	mips-linux-uclibc-gcc
AR			=	mips-linux-uclibc-ar rc
RANLIB		=	mips-linux-uclibc-ranlib
endif

################################################################################


################################################################################
# 기본 설정 내용.
LIBDIR			= $(LIBROOT)/lib/$(MXLIB_PLATFORM)/lib/$(MXLIB_BUILD)

CFLAGS			= -c -I$(UCLINUX_KERNEL)/include -I$(LIBROOT)
CFLAGS          += -D_LARGEFILE64_SOURCE=1 -DSIZE_64BIT_SUPPORT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS          += -I$(LIBROOT)/
CFLAGS          += -I$(LIBROOT)/mxkernel
CFLAGS          += -I$(LIBROOT)/mxdlna
CFLAGS		+= -I$(LIBROOT)/../transcoder/transcoder_Control 
CFLAGS		+= -I$(LIBROOT)/../transcoder/transcoderWrapper_HMS_HW 
CFLAGS		+= -I$(LIBROOT)/../openssl/Linux/include
ifeq ($(MX_DTCP_SUPPORT), y)
ifeq ($(MX_DTCP_VER), DTCP10)
CFLAGS          += -I$(LIBROOT)/mxdtcp
CFLAGS          += -I$(LIBROOT)/mxdtcp/include
else
CFLAGS          += -I$(LIBROOT)/mxdtcp17
CFLAGS          += -I$(LIBROOT)/mxdtcp17/dtcp_lib/include
CFLAGS          += -I$(LIBROOT)/mxdtcp17/dtcp_lib/wraplib/include
CFLAGS          += -I$(LIBROOT)/mxdtcp17/ippcp_lib/include
endif
endif
CFLAGS          += -I$(LIBROOT)/mxdbc
CFLAGS          += -I$(LIBROOT)/mxfile
CFLAGS          += -I$(LIBROOT)/mxresource
CFLAGS          += -I$(LIBROOT)/mxmedia
ifeq ($(MX_SATIP_SUPPORT), y)
CFLAGS			+= -I$(LIBROOT)/mxsatip
endif
ifeq ($(MX_DIAL_SUPPORT), y)
CFLAGS			+= -I$(LIBROOT)/mxdial
endif

DEBUG_MODE_FLAG = 0

ifndef MXLIB_BUILD
MXLIB_BUILD	=	debug
endif

ifeq "$(MXLIB_BUILD)" "DEBUG"
	CFLAGS 		+= -DCONFIG_DEBUG
	CFLAGS 		+= -g
	DEBUG_MODE_FLAG = 1
endif

ifeq "$(MXLIB_BUILD)" "debug"
	CFLAGS 		+= -DCONFIG_DEBUG
	CFLAGS 		+= -g
	DEBUG_MODE_FLAG = 1
endif

ifeq ($(DEBUG_MODE_FLAG) , 0) 
	CFLAGS 		+= -O2
endif


ifeq "$(MXLIB_SUPPORT_CVP1)" "YES"
	CFLAGS	+= -DDLNAQOS
endif

ifeq "$(MXLIB_TYPE)" "SHARED"
	CFLAGS		+= -fPIC -shared
endif

ifeq ($(MX_WOON_SUPPORT), y)
	CFLAGS		+= -D_POSIX -D_WOON_VERSION_SUPPORT -DMXWOON_MAKE_MODULE_SERVER
	CFLAGS		+= -I$(LIBROOT)/mxdlna/core
	CFLAGS		+= -I$(LIBROOT)/../Ra
	CFLAGS		+= -I$(LIBROOT)/../Ra/common 
	CFLAGS		+= -I$(LIBROOT)/../Ra/RaServer/Collector
	CFLAGS		+= -I$(LIBROOT)/../Ra/RaServer/DB_Manager 
	CFLAGS		+= -I$(LIBROOT)/../Ra/RaServer/MicroServer 
	CFLAGS		+= -I$(LIBROOT)/../Ra/RaServer/WanCP 
	CFLAGS		+= -I$(LIBROOT)/../Ra/RaServer/WidMgrClient 
endif
################################################################################


################################################################################
# BASE 설정
ifeq "$(MXLIB_TARGET)" "BASE"
	CFLAGS		+= -D_POSIX
	CFLAGS		+= -DUSE_SQLITE_AMA
endif	
################################################################################


################################################################################
# 아래 STLINUX와 SHEEVA도 HUMAX 설정입니다. 
# 정확히 말하면, 컴파일러 플랫폼만 다른 것인데, 하나의 컴파일러를 HUMAX, HUMAX2로 분활 했습니다. 
# 아마도 DTCP 작업을 하면서 그렇게 한것 같습니다. 
# 다시 확인해서 정리할 필요가 있습니다. 
HUMAXFLATFORM=0

ifeq "$(MXLIB_TARGET)" "HUMAX"
HUMAXFLATFORM=1
endif

ifeq "$(MXLIB_TARGET)" "HUMAX2"
HUMAXFLATFORM=1
endif

ifeq ($(HUMAXFLATFORM),1)
	CFLAGS		+= -D_POSIX

# 201111226 whiterub enabled for dong-ho kim in Humax
# H/W En/DeCrypt
	CFLAGS		+= -D_HUMAX_7405_

#20111025 added for humax DTCP TEST by whiterub
#20111206 blocked for test by whiterub
# HMX Key Modification
#	CFLAGS		+= -DHUMAX_AES

# 20120328 dtcp temporary disable by jh
#	CFLAGS		+= -D_HUMAX_SECURE_FILE
#	CFLAGS 		+= -DDTCP_SUPPORT
#	CFLAGS		+= -DSAME_COMPANY_PROTECTIONFILE
	CFLAGS		+= -DDMS_DO_NOT_ANSWER_FROM_SAME_IP_DMP
#	CFLAGS		+= -DDTCP_SUPPORT_HMX
	CFLAGS		+= -DUSE_SQLITE_AMA
# for shared Lib 20120424 by jonghyun
endif
################################################################################


################################################################################
# STLINUX 환경
ifeq "$(MXLIB_TARGET)" "STLINUX"
	CFLAGS		+= -D_POSIX
	CFLAGS		+= -D_HUMAX_SECURE_FILE
	CFLAGS		+= -DSAME_COMPANY_PROTECTIONFILE
	CFLAGS		+= -DDMS_DO_NOT_ANSWER_FROM_SAME_IP_DMP
	CFLAGS		+= -DDTCP_SUPPORT_HMX
	CFLAGS		+= -DUSE_SQLITE_AMA
endif
################################################################################


################################################################################
# SHEEVA 환경
ifeq "$(MXLIB_TARGET)" "SHEEVA"
	CFLAGS		+= -D_POSIX
	CFLAGS		+= -DSUPPORT_SUBTITLE_KT
	CFALGS		+= -DOS_UNIX	
endif
################################################################################

ifeq ($(MX_DTCP_SUPPORT), y)
ifeq ($(MX_DTCP_VER), DTCP10)
	CFLAGS		+= -DDTCP_SUPPORT
else
	CFLAGS		+= -DDTCP17_SUPPORT
endif
endif

################################################################################
# C++ 설정도 동일하게 만들어 줍니다.
CXXFLAGS	+= $(CFLAGS)
################################################################################
