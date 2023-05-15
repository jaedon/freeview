############################################################################
#      THIS MUST APPEAR AT THE TOP OF THE MAKEFILE.  DO NOT MODIFY.
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
include ../../../../../ndvd/bdvd_top.mak
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##

MAGNUM_TOP := $(shell cd ../../../../; pwd)
NDVD_TOP := $(shell cd ../../../../../ndvd; pwd)



# If build system can't find environment 'plat' variables, define here
#CFLAGS += -DBCHP_CHIP=7640
#CFLAGS += -DBCHP_VER=a0
#CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE

B_REFSW_OS=linuxuser

# Add in magnum base module paths also
include $(MAGNUM_TOP)/basemodules/kni/bkni.inc
include $(MAGNUM_TOP)/basemodules/std/bstd.inc
include $(MAGNUM_TOP)/basemodules/dbg/bdbg.inc
include $(MAGNUM_TOP)/basemodules/chp/bchp.inc
include $(MAGNUM_TOP)/basemodules/reg/breg.inc
include $(MAGNUM_TOP)/basemodules/err/berr.inc
include $(MAGNUM_TOP)/commonutils/lst/blst.inc


# Custom include paths
CFLAGS += -I./include -I./include/priv
CFLAGS += -I$(MAGNUM_TOP)/basemodules/kni
CFLAGS += -I$(MAGNUM_TOP)/basemodules/kni/linuxuser
CFLAGS += -I$(MAGNUM_TOP)/basemodules/std
CFLAGS += -I$(MAGNUM_TOP)/basemodules/std/types/linuxuser
CFLAGS += -I$(MAGNUM_TOP)/basemodules/std/config
CFLAGS += -I$(MAGNUM_TOP)/basemodules/dbg
CFLAGS += -I$(MAGNUM_TOP)/basemodules/chp
CFLAGS += -I$(MAGNUM_TOP)/basemodules/reg
CFLAGS += -I$(MAGNUM_TOP)/basemodules/err
CFLAGS += -I$(MAGNUM_TOP)/basemodules/lst
CFLAGS += -I$(MAGNUM_TOP)/commonutils/lst
CFLAGS += -I$(MAGNUM_TOP)/basemodules/int
CFLAGS += -I$(MAGNUM_TOP)/basemodules/mem
CFLAGS += -I$(MAGNUM_TOP)/basemodules/chp/7640/rdb/a0/
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/common
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/common/aegis
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/cust_generic/aegis
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/dvd_cmds
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/dvd_cmds/aegis
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/keyladder
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/user_misc_cmd
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/otpmsp/
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/cust_all/aegis
CFLAGS += -I$(MAGNUM_TOP)/portinginterface/hsm/7640/a0/cust_generic/aegis
CFLAGS += -DPIC -fpic



############################################################################
#                              
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##

# DRM Makefile exclude the 'include' subfolder which is built with the makefile
D_ALL_SUBDIRS		:= $(filter-out %include %lib, $(shell find . -type d))
D_ALL_MODULE_DIR	:= $(filter-out %include %lib, $(shell find . -maxdepth 1 -type d))
#$(error XXX D_ALL_SUBDIRS = ${D_ALL_SUBDIRS} XXX)
#$(error XXX D_ALL_MODULE_DIR = ${D_ALL_MODULE_DIR} XXX)

vpath %.c    		${D_ALL_SUBDIRS}
vpath %.h      		${D_ALL_SUBDIRS}
LIB_BASENAME        := hsm


############
# Set F_SRC_EXCLUDES to the basenames of C/C++ files you would like to 
# exclude from compilation.  By default, all *.c and *.cpp files will
# be compiled.  
#####
F_SRC_EXCLUDES      += 


############
# Set F_PUBLIC_INCS to the basenames of include files that are to be 
# used by other libraries or applications.  These files will be 
# installed into .../${BCHP_VER}/include/
#####
F_PUBLIC_INCS    	+= $(sort $(notdir $(shell find ${D_ALL_MODULE_DIR} -name '*.h')))
 

#$(error xxx F_PUBLIC_INCS = ${F_PUBLIC_INCS} xxx)

############
# If your library depends on another shared lib, you should add that 
# lib here (and any additional lib directories to search).
#####
LDFLAGS				+= -L$(NDVD_TOP)/stage/usr/local/lib -lsettop



############
# Directory where to install the headers
#####
D_FOR_INC_INSTALL	:= include



############
# You probably shouldn't have to modify the text below this point.
#####
F_PUBLIC_LIBS    	+= lib${LIB_BASENAME}.a lib${LIB_BASENAME}.so
D_FOR_LIB_INSTALL	:= lib
F_SRCS	       		:= $(filter-out ${F_SRC_EXCLUDES}, $(notdir $(wildcard $(addsuffix /*.c, ${D_ALL_SUBDIRS}))))
F_SRCS_GPG          := 
F_PREBUILT_OBJS     := $(patsubst %.gpg,%.o,${F_SRCS_GPG})
F_OBJS 		   		:= $(patsubst %.c,%.o, ${F_SRCS})  ${F_PREBUILT_OBJS}
F_INSTALLED_LIBS	:= $(addprefix ${D_FOR_LIB_INSTALL}/, ${F_PUBLIC_LIBS})
F_INSTALLED_INCS	:= $(addprefix ${D_FOR_INC_INSTALL}/, ${F_PUBLIC_INCS})
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                              MAIN TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
all: ${F_PUBLIC_LIBS}

clean: 
	find -name '*.d' -o -name '*.o'  | xargs rm -f
	ls *.a  | xargs rm -f
	ls *.so | xargs rm -f
#	find ./lib -name '*.a' -o -name '*.so' | xargs rm -f

distclean: clean
#	rm -f ${F_INSTALLED_LIBS} 
#	rm -f ${D_FOR_INC_INSTALL}/*.h ${F_INSTALLED_LIBS} 

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                             BUILD RULES
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
lib${LIB_BASENAME}.so: ${F_OBJS} 
	${CC} -shared -o $@  ${LDFLAGS}  $^ -Wl,-dy
	${STRIP_COMMAND}

lib${LIB_BASENAME}.a: ${F_OBJS} 
	${AR} rc  $@  $^ 
	${STRIP_COMMAND}

idirs:
	@ [ -d ${D_FOR_LIB_INSTALL} ] || mkdir -p ${D_FOR_LIB_INSTALL}
	@ [ -d ${D_FOR_INC_INSTALL} ] || mkdir -p ${D_FOR_INC_INSTALL}

install: all idirs ${F_INSTALLED_LIBS} ${F_INSTALLED_INCS}

${F_INSTALLED_INCS} : ${D_FOR_INC_INSTALL}/%.h : %.h
	install  -m 0664 $< $@

${D_FOR_LIB_INSTALL}/%.a : %.a
	install -m 0755 $< $@ 

${D_FOR_LIB_INSTALL}/%.so : %.so
	install -m 0755 $< $@ 

${F_PREBUILT_OBJS} : %.o : %.obj
	${BDVD_PPRINT.cp}
	@cp $^ $@

############################################################################
#                      DO NOT MODIFY ANYTHING BELOW
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
include ../../../../../ndvd/bdvd_btm.mak
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##