############################################################
#     (c)2007-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
############################################################
#
# Description:
# ===========
#
# The Makefile now has intelligence about what targets have been configured
# and compiled and it handles prerequisites correctly.  The intelligence goes
# as far as knowing whether the build options or build environment has changed
# since the last build.  If it has, then some software modules may
# automatically be rebuilt.  This intelligence allows for an efficient means
# of building DirectFB repeatedly without having to rebuild software modules
# unncessarily.
#
# Makefile Targets and Usage
# --------------------------
#
# default:            only the software modules that have not been configured
#                     and compiled before will be built.  Any changes to source
#                     files will not be tracked.  This is useful if an
#                     application such as Webkit has DFB as a dependency along
#                     with other shared libs such as libpng, libjpeg, and
#                     freetype.  This will prevent these libs from being
#                     configured and built multiple times.
#
# all:                reconfigure and rebuild all software modules.  This was the
#                     original default option on previous DirectFB releases.
#
# install:            install all software modules (only if they require
#                     installing.  This target will also create a tarball that
#                     can then be untarred on the target system.
#
# compile:            compile all software modules (only if they require building).
#
# config:             configure all software modules (only if they require
#                     configuring.
#
# uninstall:          remove all installed software modules from the internal
#                     installation target installation directories.
#
# uninstall-target:   remove all installed files from the target installation
#                     directory (e.g. /AppLibs/target).
#
# clean:              remove most generated files.
#
# distclean:          remove all generated files including sources with user prompt.
#
# mrproper:           remove all generated files including sources without user prompts.
#
# xxxx-config:        force xxxx to be reconfigured (where xxxx can be:
#                     "directfb", "sawman", "ppdfb", "freetype",
#                     "png", "jpeg" and "zlib").
#
# xxxx-autogen:       force configuration files for xxxx to be re-generated and
#                     re-configured (where xxxx can be:
#                     "directfb" and "sawman").
#
# xxxx:               force xxxx to be compiled and installed.
#                     This will track any changes made to the source files.
#                     It will also build any software depedent modules that
#                     were not already compiled.  Use this target when making
#                     changes to the source code for xxxx. xxxx can be:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png", "jpeg" and "zlib".
#
# xxxx-compile:       force xxxx to be compiled.
#                     This will track any changes made to the source files.
#                     It will also build any software depedent modules that
#                     were not already compiled.  Use this target when making
#                     changes to the source code for xxxx. xxxx can be:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png", "jpeg" and "zlib".
#
# xxxx-install:       force xxxx to be installed.  This may automatically
#                     cause xxxx and associated libraries to be compiled.
#                     xxxx can be any of the following:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png" "jpeg" and "zlib".
#
# xxxx-uninstall:     force xxxx to be uninstalled. xxxx can be any of the
#                     following:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png" "jpeg" and "zlib".
#
# xxxx-clean:         remove all generated files for xxxx.  xxxx can be:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png" "jpeg" and "zlib".
#
# xxxx-distclean:     remove all files including the source (optional) for
#                     xxxx.  xxxx can be any of the following:
#                     "directfb", "fusion", "sawman", "ppdfb",
#                     "freetype", "png" "jpeg" and "zlib".
#
# Examples:
# --------
#
# If you want to build everything first from scratch, then modify DirectFB
# source code and rebuild/install, then follow these steps for a
# multi-app version of DirectFB that uses SaWMan:
#
# 1. make all DIRECTFB_MULTI=y
# 2. modify DirectFB source as you require.
# 3. make directfb DIRECTFB_MULTI=y
# 4. make install DIRECTFB_MULTI=y
#
# If you are building DirectFB as a prerequisite for another software module
# then you can simply call make without any targets specified (make sure you
# do have the correct make flags such as DIRECTFB_MULTI=[y/n] set though).
# Any further call to make without a target specified, will avoid the need
# to recompile all the software modules again (unless the build environment
# has changed between builds).
#
############################################################

# We base a lot of things on the particular platform we compiling for.
# This variable MUST be defined in order to continue.  Make sure it is!
ifndef NEXUS_PLATFORM
ifndef PLATFORM
$(error NEXUS_PLATFORM environment variable is required)
else
NEXUS_PLATFORM := $(PLATFORM)
endif
endif

#
# This is the default architecture of the target.
# This can be overriden.
#
ifeq ($(B_REFSW_ARCH),arm-linux)
ARCH ?= arm
else
ARCH ?= mips
endif

ifeq ($(shell test -d ../../../../AppLibs && echo "y"),y)
APPLIBS_TOP    ?= $(shell cd ../../../../AppLibs ; pwd)
else
ifeq ($(shell test -d ../../../../../AppLibs && echo "y"),y)
APPLIBS_TOP    ?= $(shell cd ../../../../../AppLibs ; pwd)
else
ifeq ($(shell test -d ../../../../../directfb_src && echo "y"),y)
APPLIBS_TOP    ?= $(shell cd ../../../../../directfb_src ; pwd)
else
$(error *** Cannot find AppLibs root dir ***)
endif
endif
endif

ifeq ($(shell test -d $(APPLIBS_TOP)/../rockford && echo "y"),y)
ROCKFORD_TOP   ?= $(shell cd $(APPLIBS_TOP)/../rockford ; pwd)
else
ROCKFORD_TOP   ?= /tmp
endif

ifeq ($(shell test -d $(APPLIBS_TOP)/../nexus && echo "y"),y)
NEXUS_TOP      ?= $(shell cd $(APPLIBS_TOP)/../nexus ; pwd)
else
NEXUS_TOP      ?= /tmp
endif

NEXUS_BIN_DIR  ?= ${NEXUS_TOP}/bin
NEXUS_C_STD    ?= c99

##
#
# PHONY TARGETS
#
.PHONY: default all ALWAYS check check-tools check-autogen-tools config compile tarball install uninstall uninstall-target
.PHONY: release release-check clean distclean mrproper precompile-clean precompile-distclean help
.PHONY: directfb-all directfb-defines directfb-source directfb-check-config directfb-autogen directfb-config directfb directfb-compile
.PHONY: directfb-install directfb-post-install directfb-uninstall directfb-clean directfb-distclean directfb-mrproper


##
#
# Default TARGET is to only build the components if they have not
# already been compiled and then install them (creates installation tarball).
#
default: install

##
#
# 1. Include build configuration (only for older Nexus build system that doesn't use namespace)
#

ifeq ($(shell test -e $(NEXUS_TOP)/build/tools.inc && echo "y"),y)
include $(NEXUS_TOP)/build/tools.inc
endif

##
#
# 2. Include platform application make include file
#
ifeq ($(shell test -e $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc && echo "y"),y)
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

##
#
# 3. Include build rules
#
include packages/Rules.make

##
#
# 4. Include DirectFB Common Makefile
#
include directfb_common.inc

#
# Tools
#
MKDIR   = mkdir -p
RMDIR   = rmdir --ignore-fail-on-non-empty
PWD     = pwd
CD      = cd
CP      = cp -fp
MV      = mv
LN      = ln -fs
RM      = rm -f
CAT     = cat
STAT    = stat
CHMOD   = chmod
READ    = read
GREP    = grep
ECHO    = echo
CUT     = cut
SED     = sed
DIFF    = diff -q
FIND    = find
WHICH   = which
TOUCH   = touch
INSTALL = install
EXPORT  = export
TAR     = tar
UMASK   = umask
FALSE   = /bin/false
TRUE    = /bin/true
XARGS   = xargs

#
# Include other packages
#
include packages/refsw_parser.inc
include packages/directfb_broadcom.inc

##
#
# Function to check tool versions.
# Inputs: $1 - first arg  = tool ver
#         $2 - second arg = minimum tool ver
#
define fn-check-versions
(\
fail=0;\
pos=1;\
ver1=0;\
ver2=0;\
tot1=0;\
tot2=0;\
while [ $$fail -ne 1 -a "$$ver1" != "" -a "$$ver2" != "" ]; do\
    ver1=`$(ECHO) $(1) | $(CUT) -d. -f$$pos`;\
    ver2=`$(ECHO) $(2) | $(CUT) -d. -f$$pos`;\
    if [ "$$ver1" != "" -a "$$ver2" != "" ]; then\
	tot1=`expr $$tot1 \* 256 \+ $$ver1`;\
	tot2=`expr $$tot2 \* 256 \+ $$ver2`;\
        fi;\
   pos=`expr $$pos + 1`;\
done;\
if [ "$$tot1" -lt "$$tot2" ]; then \
    $(FALSE);\
else\
    $(TRUE);\
fi;\
)
endef

##
#
# Check host tool versions
#
AUTOCONF            := autoconf
AUTOMAKE            := automake
LIBTOOL             := libtool

MAKE_VER_MIN        := 3.80
AUTOCONF_VER_MIN    := 2.52
AUTOMAKE_VER_MIN    := 1.4
LIBTOOL_VER_MIN     := 1.3.4

##
#
# Check make actually exists
#
ifeq ($(shell ${WHICH} ${MAKE}),)
$(error FATAL: Cannot find "make" to compile source code - aborting!!!)
else
MAKE_VER            := $(shell ${MAKE}     --version | $(SED) -ne "1{s/[^0-9]\+//;p}" | $(SED) -ne "1{s/[^0-9.]\+//;p}" | $(CUT) -d\  -f1)
endif
ifneq ($(shell ${WHICH} ${AUTOCONF}),)
AUTOCONF_VER        := $(shell ${AUTOCONF} --version | $(SED) -ne "1{s/[^0-9]\+//;p}" | $(SED) -ne "1{s/[^0-9.]\+//;p}" | $(CUT) -d\  -f1)
endif
ifneq ($(shell ${WHICH} ${AUTOMAKE}),)
AUTOMAKE_VER        := $(shell ${AUTOMAKE} --version | $(SED) -ne "1{s/[^0-9]\+//;p}" | $(SED) -ne "1{s/[^0-9.]\+//;p}" | $(CUT) -d\  -f1)
endif
ifneq ($(shell ${WHICH} ${LIBTOOL}),)
LIBTOOL_VER         := $(shell ${LIBTOOL}  --version | $(SED) -ne "1{s/[^0-9]\+//;p}" | $(SED) -ne "1{s/[^0-9.]\+//;p}" | $(CUT) -d\  -f1)
endif

##
#
# Broadcom C & CPP Flags
#
# NOTE: DirectFB Multi-Application Support comes in 2 flavours:
#       1) Internal Experimental Support where fusion is completely in user-space [BUILD_FUSION=n]
#       2) External fusion kernel module [BUILD_FUSION=y]
#

# ifeq ($(HUMAX_PLATFORM_BASE),y)
CFLAGS += -include $(HUMAX_CONFIG_DIR)/autoconf.h -DHUMAX_PLATFORM_BASE
LDFLAGS += -lstdc++
# endif

BRCM_CFLAGS = $(CFLAGS) \
              $(NEXUS_CFLAGS) \
	      -I$(MAGNUM)/syslib/grclib/7401/ \
              $(addprefix -I, ${DIRECTFB_OBJECT_DIR}/include) \
              $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) \
              $(addprefix -D,$(NEXUS_APP_DEFINES))

BRCM_LDFLAGS = -L${DIRECTFB_OBJECT_DIR}/lib -L${NEXUS_BIN_DIR} -Wl,-rpath-link=${DIRECTFB_OBJECT_DIR}/lib:${NEXUS_BIN_DIR}
BRCM_LIBS = -lnexus -lrt
BRCM_CLIENT_LIBS = -lnexus${NEXUS_CLIENT_LIB_SUFFIX} -lrt


BRCM_CPPFLAGS = -fPIC -DPIC -I${LINUX}/include -I${LINUX}/include/asm/mach-generic/ -I${DIRECTFB_INCLUDE_DIR} \
				-I${DIRECTFB_SRC_DFB_DIR} -I${DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR} \
				-I${DIRECTFB_INSTALL_DIRECTFB_INTERNAL_INCLUDE_DIR} -I${DIRECTFB_OBJECT_DIR}/include \
				-I${DIRECTFB_PRE_INSTALL_INCLUDE_DIR} -I${DIRECTFB_LIB_DIR}


# ifeq ($(HUMAX_PLATFORM_BASE),y)
BRCM_CPPFLAGS += -include $(HUMAX_CONFIG_DIR)/autoconf.h -DHUMAX_PLATFORM_BASE
# endif

ifeq (${DIRECTFB_GFX_3D},"v3d")
V3D_DIR ?= ${ROCKFORD_TOP}/middleware/v3d
BRCM_CFLAGS := ${BRCM_CFLAGS} -I${V3D_DIR}/interface/khronos/include
endif



ifeq ($(BUILD_FUSION),y)
BRCM_CPPFLAGS := ${BRCM_CPPFLAGS} -I${FUSION_INSTALL_INCLUDE_DIR}
endif

ifeq ($(BUILD_AUTOMATION),y)
BRCM_CPPFLAGS  += -DBUILD_AUTOMATION
endif

##
#
# Make Flags
#
ifeq ($(MULTI_BUILD),y)
MAKE_OPTIONS ?= -j4
endif


############################################################
#
# Include all our dependencies packages 
# to add relevant defines and rules
#
############################################################

include packages/directfb_examples.inc

include packages/linux_fusion.inc

include packages/opensource_libraries.inc

include packages/sawman.inc

include packages/ppdfb.inc

include packages/divine.inc

include packages/ffmpeg.inc

-include ${OPENSOURCE_TOP}/directfb_internal/packages/insignia.inc

-include ${OPENSOURCE_TOP}/directfb_internal/packages/tacho.inc

-include packages/directfb_autotest.inc

-include $(OPENSOURCE_TOP)/directfb_internal/packages/directfb_brcm_examples_private.inc

############################################################
#
# Targets
#
############################################################

##
#
# help: Display Make Help Options/Target
#
help:
	@$(ECHO) "=============================="; \
	$(ECHO)  "= DirectFB Build System Help ="; \
	$(ECHO)  "=============================="; \
	$(ECHO)  "Generic targets:"; \
	$(ECHO)  "  default (or blank)          - build and install all software modules only if they require building and installing"; \
	$(ECHO)  "  all                         - force ALL software modules to be compiled and installed"; \
	$(ECHO)  "  install                     - install all software modules (only ones that require installing) and create tarball"; \
	$(ECHO)  "  uninstall                   - uninstall all software modules from both installation and target directories"; \
	$(ECHO)  "  uninstall-target            - uninstall all software modules from only the target directory"; \
	$(ECHO)  "  compile                     - compile all software modules (only ones that require compiling)."; \
	$(ECHO)  "  config                      - configure all software modules (only ones that require configuring)"; \
	$(ECHO)  "  clean                       - remove most generated files from all software modules but keep the configs"; \
	$(ECHO)  "  distclean                   - remove all generated files from all software modules including the configs and source (with prompts)"; \
	$(ECHO)  "  mrproper                    - remove all generated files from all software modules including the configs and source"; \
	$(ECHO)  "  release                     - create a release tarball with Open Source and Broadcom SLA specific code"; \
	$(ECHO)  "  check-tools                 - check the version of \"Make\" for compatibility"; \
	$(ECHO)  "  check-autogen-tools         - check the versions of \"autoconf\", \"automake\" and \"libtool\" for compatibility"; \
	$(ECHO)  "  directfb-defines            - create/update the DirectFB autogenerated include define files"; \
	$(ECHO)  "  nexus                       - check that Nexus driver code has already been built"; \
	$(ECHO)  "  help                        - display this help information"; \
	$(ECHO)  ""; \
	$(ECHO)  "Clean specific targets:"; \
	$(ECHO)  "  precompile-clean            - remove all files in the precompiled installation directory (and subdirs)"; \
	$(ECHO)  "  precompile-distclean        - remove all files in the precompiled top-level directory (and subdirs)"; \
	$(ECHO)  "  directfb-clean              - remove most DirectFB generated files but keep the config"; \
	$(ECHO)  "  directfb-distclean          - remove all DirectFB generated files including the config and source (with prompt)"; \
	$(ECHO)  "  directfb-mrproper           - remove all DirectFB generated files including the config and source"; \
	$(ECHO)  "  directfb-examples-clean     - remove most DirectFB Examples generated files but keep the config"; \
	$(ECHO)  "  directfb-examples-distclean - remove all DirectFB Examples generated files including the config and source (with prompt)"; \
	$(ECHO)  "  directfb-examples-mrproper  - remove all DirectFB Examples generated files including the config and source"; \
	$(ECHO)  "  fusion-clean                - remove most linux-fusion driver generated files but keep the config"; \
	$(ECHO)  "  fusion-distclean            - remove all linux-fusion driver generated files including the config and source (with prompt)"; \
	$(ECHO)  "  fusion-mrproper             - remove all linux-fusion driver generated files including the config"; \
	$(ECHO)  "  sawman-clean                - remove most SaWMan generated files but keep the config"; \
	$(ECHO)  "  sawman-distclean            - remove all SaWMan generated files including the config and source (with prompt)"; \
	$(ECHO)  "  sawman-mrproper             - remove all SaWMan generated files including the config and source"; \
	$(ECHO)  "  ppdfb-clean                 - remove most ++DFB generated files but keep the config"; \
	$(ECHO)  "  ppdfb-distclean             - remove all ++DFB generated files including the config and source (with prompt)"; \
	$(ECHO)  "  ppdfb-mrproper              - remove all ++DFB generated files including the config and source"; \
	$(ECHO)  "  divine-clean                - remove most DiVine generated files but keep the config"; \
	$(ECHO)  "  divine-distclean            - remove all DiVine generated files including the config and source (with prompt)"; \
	$(ECHO)  "  divine-mrproper             - remove all DiVine generated files including the config and source"; \
	$(ECHO)  "  insignia-clean              - remove most Insignia generated files but keep the config"; \
	$(ECHO)  "  insignia-distclean          - remove all Insignia generated files including the config and source (with prompt)"; \
	$(ECHO)  "  insignia-mrproper           - remove all Insignia generated files including the config and source"; \
	$(ECHO)  "  tacho-clean                 - remove most Tacho generated files but keep the config"; \
	$(ECHO)  "  tacho-distclean             - remove all Tacho generated files including the config and source (with prompt)"; \
	$(ECHO)  "  tacho-mrproper              - remove all Tacho generated files including the config and source"; \
	$(ECHO)  "  freetype-clean              - remove most FreeType generated files but keep the config"; \
	$(ECHO)  "  freetype-distclean          - remove all FreeType generated files including the config and source (with prompt)"; \
	$(ECHO)  "  freetype-mrproper           - remove all FreeType generated files including the config and source"; \
	$(ECHO)  "  png-clean                   - remove most PNG library generated files but keep the config"; \
	$(ECHO)  "  png-distclean               - remove all PNG library generated files including the config and source (with prompt)"; \
	$(ECHO)  "  png-mrproper                - remove all PNG library generated files including the config and source"; \
	$(ECHO)  "  jpeg-clean                  - remove most JPEG library generated files but keep the config"; \
	$(ECHO)  "  jpeg-distclean              - remove all JPEG library generated files including the config and source (with prompt)"; \
	$(ECHO)  "  jpeg-mrproper               - remove all JPEG library generated files including the config and source"; \
	$(ECHO)  "  zlib-clean                  - remove most ZLIB library generated files but keep the config"; \
	$(ECHO)  "  zlib-distclean              - remove all ZLIB library generated files including the config and source (with prompt)"; \
	$(ECHO)  "  zlib-mrproper               - remove all ZLIB library generated files including the config and source"; \
	$(ECHO)  "  ffmpeg-clean                - remove most FFmpeg library generated files but keep the config"; \
	$(ECHO)  "  ffmpeg-distclean            - remove all FFmpeg library generated files including the config and source (with prompt)"; \
	$(ECHO)  "  ffmpeg-mrproper             - remove all FFmpeg library generated files including the config and source"; \
	$(ECHO)  ""; \
	$(ECHO)  "Configuration specific targets:"; \
	$(ECHO)  "  directfb-config             - configure DirectFB (may compile prerequisite software modules)"; \
	$(ECHO)  "  directfb-autogen            - autogenerate and configure DirectFB (may compile prerequisite software modules)"; \
	$(ECHO)  "  directfb-examples-config    - configure DirectFB Examples (may compile prerequisite software modules)"; \
	$(ECHO)  "  directfb-examples-autogen   - autogenerate and configure DirectFB Examples (may compile prerequisite software modules)"; \
	$(ECHO)  "  sawman-config               - configure SaWMan (may compile prerequisite software modules)"; \
	$(ECHO)  "  sawman-autogen              - autogenerate and configure SaWMan (may compile prerequisite software modules)"; \
	$(ECHO)  "  ppdfb-config                - configure ++DFB (may compile prerequisite software modules)"; \
	$(ECHO)  "  divine-config               - configure DiVine (may compile prerequisite software modules)"; \
	$(ECHO)  "  insignia-config             - configure Insignia (may compile prerequisite software modules)"; \
	$(ECHO)  "  tacho-config                - configure Tacho (may compile prerequisite software modules)"; \
	$(ECHO)  "  freetype-config             - configure FreeType"; \
	$(ECHO)  "  png-config                  - configure PNG library (may compile zlib prerequisite software module)"; \
	$(ECHO)  "  jpeg-config                 - configure JPEG library"; \
	$(ECHO)  "  zlib-config                 - configure ZLIB library"; \
	$(ECHO)  "  ffmpeg-config               - configure FFMPEG library"; \
	$(ECHO)  ""; \
	$(ECHO)  "Compilation specific targets:"; \
	$(ECHO)  "  directfb-compile            - compile DirectFB"; \
	$(ECHO)  "  directfb-examples-compile   - compile DirectFB Examples"; \
	$(ECHO)  "  fusion-compile              - compile linux-fusion driver"; \
	$(ECHO)  "  sawman-compile              - compile SaWMan"; \
	$(ECHO)  "  ppdfb-compile               - compile ++DFB"; \
	$(ECHO)  "  divine-compile              - compile DiVine"; \
	$(ECHO)  "  insignia-compile            - compile Insignia"; \
	$(ECHO)  "  tacho-compile               - compile Tacho"; \
	$(ECHO)  "  freetype-compile            - compile FreeType"; \
	$(ECHO)  "  png-compile                 - compile PNG library"; \
	$(ECHO)  "  jpeg-compile                - compile JPEG library"; \
	$(ECHO)  "  zlib-compile                - compile ZLIB library"; \
	$(ECHO)  "  ffmpeg-compile              - compile FFMPEG library"; \
	$(ECHO)  ""; \
	$(ECHO)  "Installation specific targets:"; \
	$(ECHO)  "  directfb-install            - install DirectFB"; \
	$(ECHO)  "  directfb-post-install       - install additional DirectFB files (e.g. scripts)"; \
	$(ECHO)  "  directfb-examples-install   - install DirectFB Examples"; \
	$(ECHO)  "  fusion-install              - install linux-fusion driver"; \
	$(ECHO)  "  sawman-install              - install SaWMan"; \
	$(ECHO)  "  ppdfb-install               - install ++DFB"; \
	$(ECHO)  "  divine-install              - install DiVine"; \
	$(ECHO)  "  insignia-install            - install Insignia"; \
	$(ECHO)  "  tacho-install               - install Tacho"; \
	$(ECHO)  "  freetype-install            - install FreeType"; \
	$(ECHO)  "  png-install                 - install PNG library"; \
	$(ECHO)  "  jpeg-install                - install JPEG library"; \
	$(ECHO)  "  zlib-install                - install ZLIB library"; \
	$(ECHO)  "  ffmpeg-install              - install FFMPEG library"; \
	$(ECHO)  ""; \
	$(ECHO)  "Uninstallation specific targets:"; \
	$(ECHO)  "  directfb-uninstall          - uninstall DirectFB"; \
	$(ECHO)  "  directfb-examples-uninstall - uninstall DirectFB Examples"; \
	$(ECHO)  "  fusion-uninstall            - uninstall linux-fusion driver"; \
	$(ECHO)  "  sawman-uninstall            - uninstall SaWMan"; \
	$(ECHO)  "  ppdfb-uninstall             - uninstall ++DFB"; \
	$(ECHO)  "  divine-uninstall            - uninstall DiVine"; \
	$(ECHO)  "  insignia-uninstall          - uninstall Insignia"; \
	$(ECHO)  "  tacho-uninstall             - uninstall Tacho"; \
	$(ECHO)  "  freetype-uninstall          - uninstall FreeType"; \
	$(ECHO)  "  png-uninstall               - uninstall PNG library"; \
	$(ECHO)  "  jpeg-uninstall              - uninstall JPEG library"; \
	$(ECHO)  "  zlib-uninstall              - uninstall ZLIB library"; \
	$(ECHO)  "  ffmpeg-uninstall            - uninstall FFMPEG library"; \
	$(ECHO)  ""; \
	$(ECHO)  "One pass compilation and installation targets:"; \
	$(ECHO)  "  directfb                    - compile + install DirectFB"; \
	$(ECHO)  "  directfb-examples           - compile + install DirectFB Examples"; \
	$(ECHO)  "  fusion                      - compile + install linux-fusion driver"; \
	$(ECHO)  "  sawman                      - compile + install SaWMan"; \
	$(ECHO)  "  ppdfb                       - compile + install ++DFB"; \
	$(ECHO)  "  divine                      - compile + install DiVine"; \
	$(ECHO)  "  insignia                    - compile + install Insignia"; \
	$(ECHO)  "  tacho                       - compile + install Tacho"; \
	$(ECHO)  "  freetype                    - compile + install FreeType"; \
	$(ECHO)  "  png                         - compile + install PNG library"; \
	$(ECHO)  "  jpeg                        - compile + install JPEG library"; \
	$(ECHO)  "  zlib                        - compile + install ZLIB library"; \
	$(ECHO)  "  ffmpeg                      - compile + install FFMPEG library"; \
	$(ECHO)  ""; \
	$(ECHO)  "Generic build flags:"; \
	$(ECHO)  "  DEBUG=                      - options are \"y\" or \"n\" to enable debugging"; \
	$(ECHO)  "  ARCH=                       - default is \"mipsel-linux\" for little endian but for big endian \"mips-linux\" can be set"; \
	$(ECHO)  "  TRACE=                      - default is \"n\" which disables compile-time trace information"; \
	$(ECHO)  "  VERBOSE=                    - default is \"n\" which disables verbose build-time information"; \
	$(ECHO)  "  SMP=                        - setting \"y\" will build linux-fusion driver in SMP mode"; \
	$(ECHO)  ""; \
	$(ECHO)  "Extra build flags:"; \
	$(ECHO)  "  DIRECTFB_PREFIX=            - default is \"/usr/local\" and is where DirectFB will be installed on the target system"; \
	$(ECHO)  "  DIRECTFB_VERSION=           - default is \"1.4.1\" but older code can be compiled (e.g. 1.0.0)"; \
	$(ECHO)  "  DIRECTFB_APPLIBS=           - default is \"y\" to use \"AppLibs\" libraries or \"n\" to use \"BSEAV/lib/\" libraries instead (e.g. zlib)"; \
	$(ECHO)  "  DIRECTFB_MULTI=             - default is \"n\" to build DirectFB in single-application mode.  \"y\" will enable multi-app mode"; \
	$(ECHO)  "  DIRECTFB_ACCEL=             - default is \"y\" to use hardware graphics acceleration"; \
	$(ECHO)  "  DIRECTFB_SHARED=            - default is \"y\" to build DirectFB with shared prerequisite libraries (e.g. zlib.so)"; \
	$(ECHO)  "  DIRECTFB_SW_SMOOTH_SCALING  - default is \"n\" to build DirectFB without software smooth scaling"; \
	$(ECHO)  "  DIRECTFB_SW_DITHERING       - default is \"n\" to build DirectFB without software dithering"; \
	$(ECHO)  "  DIRECTFB_HW_DITHERING       - default is \"y\" to build DirectFB with graphics hardware dithering enabled"; \
	$(ECHO)  "  DIRECTFB_GLES_SUPPORT       - default is \"n\" to build DirectFB without OpenGL ES 1.0 support"; \
	$(ECHO)  "  BUILD_TESTS=                - default is \"n\" to prevent building DirectFB tests"; \
	$(ECHO)  "  BUILD_EXAMPLES=             - default is \"y\" to prevent building DirectFB-examples"; \
	$(ECHO)  "  BUILD_FUSION=               - default is \"n\" if building DirectFB in single-app mode, otherwise \"y\" for multi-app mode"; \
	$(ECHO)  "  BUILD_SAWMAN=               - default is \"n\" if building DirectFB in single-app mode, otherwise \"y\" for multi-app mode"; \
	$(ECHO)  "  BUILD_PPDFB=                - default is \"n\". Set to \"y\" to build ++DFB C++ bindings"; \
	$(ECHO)  "  BUILD_DIVINE=               - default is \"n\". Set to \"y\" to build DiVine module"; \
	$(ECHO)  "  BUILD_VOODOO=               - default is \"n\". Set to \"y\" to build Voodoo networking support"; \
	$(ECHO)  "  BUILD_INSIGNIA=             - default is \"n\". Set to \"y\" to build Insignia test (requires NDA)"; \
	$(ECHO)  "  BUILD_TACHO=                - default is \"n\". Set to \"y\" to build Tacho test (requires NDA)"; \
	$(ECHO)  "  BUILD_FFMPEG=               - default is \"n\". Set to \"y\" to build FFmpeg"


##
#
# all: build everything again (including configure)
#

all: check-tools nexus ${FUSION_BUILD_TARGET} ${EXTRA_PRE_DFB_BUILD_TARGETS} directfb-all directfb-brcm-all ${EXTRA_BUILD_TARGETS} install

##
#
# ALWAYS: this target always executes
#
ALWAYS:

##
#
# check: ensure that the user and group permissions are writeable and
#        make the temporary object directory structure.
#
check:
	@$(UMASK) g+w; \
	if [ ! -d $(DIRECTFB_OBJECT_DIR) ]; then \
		$(MKDIR) $(DIRECTFB_OBJECT_DIR); \
	fi; \
	if [ ! -d $(DIRECTFB_OBJECT_DIR)/bin ]; then \
		$(MKDIR) -p $(DIRECTFB_OBJECT_DIR)/bin; \
	fi; \
	if [ ! -d $(DIRECTFB_OBJECT_DIR)/include ]; then \
		$(MKDIR) -p $(DIRECTFB_OBJECT_DIR)/include; \
	fi; \
	if [ ! -d $(DIRECTFB_OBJECT_DIR)/lib ]; then \
		$(MKDIR) -p $(DIRECTFB_OBJECT_DIR)/lib; \
	fi

##
#
# check-tools: ensure that the version of "make" is suitable.
#
check-tools:
	@if ! $(call fn-check-versions,${MAKE_VER},${MAKE_VER_MIN}); then \
		$(ECHO) "ERROR: \"make\" version needs to be ${MAKE_VER_MIN} or higher in order to build DirectFB!";\
		$(FALSE);\
	fi

##
#
# check-autogen-tools: ensure that the versions of autconf, automake and libtool are suitable.
#
check-autogen-tools: check-tools
	@if ! $(call fn-check-versions,${AUTOCONF_VER},${AUTOCONF_VER_MIN}); then \
		$(ECHO) "ERROR: \"autoconf\" version needs to be ${AUTOCONF_VER_MIN} or higher in order to build DirectFB!";\
		$(FALSE);\
	fi
	@if ! $(call fn-check-versions,${AUTOMAKE_VER},${AUTOMAKE_VER_MIN}); then \
		$(ECHO) "ERROR: \"automake\" version needs to be ${AUTOMAKE_VER_MIN} or higher in order to build DirectFB!";\
		$(FALSE);\
	fi
	@if ! $(call fn-check-versions,${LIBTOOL_VER},${LIBTOOL_VER_MIN}); then \
		$(ECHO) "ERROR: \"libtool\" version needs to be ${LIBTOOL_VER_MIN} or higher in order to build DirectFB!";\
		$(FALSE);\
	fi

include packages/nexus_build.inc

##
#
# tarball: create a DirectFB target tarball release.
#
tarball:
	@if [ ! -e ${APPLIBS_TARGET_LIB_DIR}/libdirectfb.so ]; then \
		$(ECHO) "*** Please run \"make install\" to install software first! ***"; \
		$(FALSE); \
	else \
		if [ ! -d ${DIRECTFB_BIN_DIR} ]; then \
			${MKDIR} -p ${DIRECTFB_BIN_DIR}; \
		fi; \
		if [ -e "${DIRECTFB_TARGET_TARBALL}" ]; then \
			$(RM) ${DIRECTFB_TARGET_TARBALL}; \
		fi; \
		$(CD) ${APPLIBS_TARGET_ROOT_DIR}; \
		$(TAR) czf ${DIRECTFB_TARGET_TARBALL} `$(ECHO) ${DIRECTFB_PREFIX} | $(CUT) -b2-`; \
		$(ECHO) "***********************************************************************************************"; \
		$(ECHO) "* Please copy ${DIRECTFB_TARGET_TARBALL} to target platform. "; \
		$(ECHO) "***********************************************************************************************"; \
	fi

##
#
# tarball-install: Install the tarballed release into the NFS directory specified
#
tarball-install:
	@if [ -e ${DIRECTFB_TARGET_TARBALL} ]; then \
		if [ ! -d ${DIRECTFB_NFS_ROOT_DIR}/${PLATFORM} ]; then \
			${MKDIR} -p ${DIRECTFB_NFS_ROOT_DIR}/${PLATFORM}; \
		fi; \
		$(TAR) zxf ${DIRECTFB_TARGET_TARBALL} -C ${DIRECTFB_NFS_ROOT_DIR}/${PLATFORM} ; \
		$(ECHO) "***********************************************************************************************"; \
		$(ECHO) "* ${DIRECTFB_TARGET_TARBALL} installed in ${DIRECTFB_NFS_ROOT_DIR}/${PLATFORM}. "; \
		$(ECHO) "***********************************************************************************************"; \
	fi

##
#
# install: create the installation directory and target tarball
#
install: check-tools nexus-install ${NEXUS_FLAG} ${EXTRA_PRE_DFB_TARGET_INSTALLED_TARGETS} ${DIRECTFB_FLAG} ${DIRECTFB_BRCM_FLAG} ${EXTRA_TARGET_INSTALLED_TARGETS} ALWAYS
	@if [ ! -d ${APPLIBS_TARGET_ROOT_DIR} ]; then \
		${MKDIR} -p ${APPLIBS_TARGET_ROOT_DIR}; \
	fi; \
	if [ ! -d ${APPLIBS_TARGET_ROOT_DIR} ]; then \
		$(ECHO) "ERROR: Cannot create \"${APPLIBS_TARGET_ROOT_DIR}\" directory!"; \
		$(FALSE); \
	else \
		$(CD) ${DIRECTFB_INSTALL_DIR}; \
		$(TAR) c `$(ECHO) ${DIRECTFB_PREFIX} | $(CUT) -b2-` \
			--exclude include                              --exclude pkgconfig \
			--exclude man                                  --exclude \*.la     \
			--exclude $(notdir ${DIRECTFB_POST_INSTALLED_FLAG}) \
			--exclude $(notdir ${DIRECTFB_INSTALLED_FLAG}) --exclude $(notdir ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}) \
			--exclude $(notdir ${SAWMAN_INSTALLED_FLAG}) \
			--exclude $(notdir ${PPDFB_INSTALLED_FLAG})    --exclude $(notdir ${DIVINE_INSTALLED_FLAG}) \
			--exclude $(notdir ${INSIGNIA_INSTALLED_FLAG}) --exclude $(notdir ${TACHO_INSTALLED_FLAG}) \
			--exclude $(notdir ${DIRECTFB_FLAG})           --exclude $(notdir ${DIRECTFB_EXAMPLES_FLAG}) \
		  	--exclude $(notdir ${SAWMAN_FLAG}) \
			--exclude $(notdir ${PPDFB_FLAG})              --exclude $(notdir ${DIVINE_FLAG}) \
			--exclude $(notdir ${INSIGNIA_FLAG})           --exclude $(notdir ${TACHO_FLAG}) | \
		$(TAR) x -C ${APPLIBS_TARGET_ROOT_DIR}; \
		$(ECHO) "*************************************************************************************"; \
		$(ECHO) "* DirectFB Installation Complete! "; \
		$(ECHO) "* You may run \"make tarball\" to create a target tarball."; \
		$(ECHO) "*************************************************************************************"; \
	fi

##
#
# compile: compile all software modules that DirectFB requires (including DirectFB) if they have not already been built.
#
compile: check-tools nexus ${DIRECTFB_COMPILED_FLAG} ${DIRECTFB_BRCM_COMPILED_FLAG} ${EXTRA_COMPILED_TARGETS} ALWAYS
	@$(ECHO) "*************************************************************************************"; \
	$(ECHO)  "* DirectFB Compilation Complete! "; \
	$(ECHO)  "*************************************************************************************"

##
#
# config: configure all software modules that DirectFB requires (including DirectFB) if they have not already been built.
#
config: check-tools nexus ${DIRECTFB_CONFIGURED_FLAG} ${EXTRA_CONFIGURED_TARGETS} ALWAYS
	@$(ECHO) "*************************************************************************************"; \
	$(ECHO)  "* DirectFB Configuration Complete! "; \
	$(ECHO)  "*************************************************************************************"

##
#
# uninstall-target: remove all installation files from the target dir
#
uninstall-target:
	@-if [ -d "${APPLIBS_TARGET_ROOT_DIR}" -a -d "${DIRECTFB_INSTALL_DIR}" ]; then \
		$(CD) ${DIRECTFB_INSTALL_DIR}; \
		$(FIND) `$(ECHO) ${DIRECTFB_PREFIX} | $(CUT) -b2-` -type l -exec $(RM) ${APPLIBS_TARGET_ROOT_DIR}/{} \; ; \
		$(FIND) `$(ECHO) ${DIRECTFB_PREFIX} | $(CUT) -b2-` -type f -exec $(RM) ${APPLIBS_TARGET_ROOT_DIR}/{} \; ; \
		$(FIND) `$(ECHO) ${DIRECTFB_PREFIX} | $(CUT) -b2-` -depth -type d ! -regex ".*/include.*" -a ! -regex ".*/man.*" -a ! -regex ".*/pkgconfig.*" \
		-exec $(RMDIR) ${APPLIBS_TARGET_ROOT_DIR}/{} \; ; \
	fi
	@-if [ -d "${APPLIBS_TARGET_MOD_DIR}" ]; then \
		$(RM) -r ${APPLIBS_TARGET_MOD_DIR}; \
	fi
	@-if [ -d "${APPLIBS_TARGET_LIB_DIR}" ]; then \
		$(RMDIR) ${APPLIBS_TARGET_LIB_DIR}; \
	fi
	@-if [ -d "${APPLIBS_TARGET_LIB_DIR}" ]; then \
		$(ECHO) "*** WARNING: Files still present in \"${APPLIBS_TARGET_LIB_DIR}\" - please manually remove!"; \
	fi
	@-if [ -d "${APPLIBS_TARGET_DIR}" ]; then \
		$(RMDIR) ${APPLIBS_TARGET_DIR}; \
	fi
	@-if [ -d "${APPLIBS_TARGET_DIR}" ]; then \
		$(ECHO) "*** WARNING: Files still present in \"${APPLIBS_TARGET_DIR}\" - please manually remove!"; \
	fi
	@-if [ -d "${APPLIBS_TARGET_ROOT_DIR}/$(dir ${DIRECTFB_TARGET_ROOT_DIR})" ]; then \
		$(RMDIR) ${APPLIBS_TARGET_ROOT_DIR}/$(dir ${DIRECTFB_TARGET_ROOT_DIR}); \
	fi
	@-if [ -d "${APPLIBS_TARGET_ROOT_DIR}" ]; then \
		$(RMDIR) ${APPLIBS_TARGET_ROOT_DIR}; \
	fi

##
#
# uninstall: remove all installation files
#
uninstall: uninstall-target nexus-uninstall freetype-uninstall jpeg-uninstall zlib-uninstall png-uninstall directfb-uninstall \
		   ${FUSION_UNINSTALL_TARGET} ${EXTRA_UNINSTALL_TARGETS}
	@$(ECHO) "*************************************************************************************"; \
	$(ECHO)  "* DirectFB Uninstallation Complete! "; \
	$(ECHO)  "*************************************************************************************"

##
#
# clean: remove all intermediate object and dependency files
#
clean: uninstall freetype-clean jpeg-clean zlib-clean png-clean directfb-clean directfb-brcm-clean\
       ${FUSION_CLEAN_TARGET} ${EXTRA_CLEAN_TARGETS}
	@if [ -d "${DIRECTFB_OBJECT_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_OBJECT_DIR}/*; \
	fi; \
	$(ECHO) "*************************************************************************************"; \
	$(ECHO) "* DirectFB Clean Complete! "; \
	$(ECHO) "*************************************************************************************"

##
#
# distclean: remove everything as if starting from scratch with prompts
#
distclean: clean freetype-distclean jpeg-distclean zlib-distclean png-distclean directfb-distclean \
           ${FUSION_DISTCLEAN_TARGET} ${EXTRA_DISTCLEAN_TARGETS}
ifeq (${DIRECTFB_SYSTEM},y)
	@-if [ -d "${DIRECTFB_PRE_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DIRECTFB_PRE_DIR} completely."; \
		$(ECHO) "Removing ${DIRECTFB_PRE_DIR}..."; \
		$(RM) -r ${DIRECTFB_PRE_DIR}; \
	fi
endif
	@-$(call fn-distclean)
	@$(ECHO) "*************************************************************************************"; \
	$(ECHO) "* DirectFB Distribution Clean Complete! "; \
	$(ECHO) "*************************************************************************************"

##
#
# mrproper: remove everything as if starting from scratch without prompts
#
mrproper: clean freetype-mrproper jpeg-mrproper zlib-mrproper png-mrproper directfb-mrproper directfb-brcm-mrproper \
           ${FUSION_MRPROPER_TARGET} ${EXTRA_MRPROPER_TARGETS}
ifeq (${DIRECTFB_SYSTEM},y)
	@-if [ -d "${DIRECTFB_PRE_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_PRE_DIR}; \
	fi
endif
	@-$(call fn-distclean)
	@$(ECHO) "*************************************************************************************"; \
	$(ECHO) "* DirectFB Distribution Full Clean Complete! "; \
	$(ECHO) "*************************************************************************************"

##
#
# precompile-clean: remove all files from the precompiled installation directory
#
precompile-clean:
	@-if [ -d "${DIRECTFB_PRE_INSTALL_DIR}" ]; then \
		if [ -e "${DIRECTFB_POST_INSTALLED_FLAG}" ]; then \
			$(RM) ${DIRECTFB_POST_INSTALLED_FLAG}; \
		fi; \
		${RM} -r ${DIRECTFB_PRE_INSTALL_DIR}; \
		$(ECHO) "*************************************************************************************"; \
		$(ECHO) "* Precompile Clean Complete! "; \
		$(ECHO) "*************************************************************************************"; \
	fi

##
#
# precompile-distclean: remove all files from the precompiled top-level directory
#
precompile-distclean:
	@-if [ -d "${DIRECTFB_PRE_DIR}" ]; then \
		if [ -e "${DIRECTFB_POST_INSTALLED_FLAG}" ]; then \
			$(RM) ${DIRECTFB_POST_INSTALLED_FLAG}; \
		fi; \
		${RM} -r ${DIRECTFB_PRE_DIR}; \
		$(ECHO) "*************************************************************************************"; \
		$(ECHO) "* Precompile Distribution Clean Complete! "; \
		$(ECHO) "*************************************************************************************"; \
	fi

##
#
# release-check: check whether a release can be made
#
release-check:
	@if [ ! -d "${DIRECTFB_BRCM_DIR}" ]; then \
		$(ECHO) "ERROR: Cannot build release as \"${DIRECTFB_BRCM_DIR}\" directory not found!"; \
		$(FALSE); \
	fi

##
#
# release: create a software release with open-source and Broadcom SLA components
#
release: release-check directfb-brcm-clean
	@-if [ -e "${DIRECTFB_RELEASE_TARBALL}" ]; then \
		$(RM) ${DIRECTFB_RELEASE_TARBALL}; \
	fi; \
	$(TAR) czf ${DIRECTFB_RELEASE_TARBALL} -C ${APPLIBS_TOP}/.. ${DIRECTFB_RELEASE_LIST}; \
	$(ECHO) "*************************************************************************************"; \
	$(ECHO) "* Release tarball complete: ${DIRECTFB_RELEASE_TARBALL}"; \
	$(ECHO) "*************************************************************************************"

##
#
# fn-distclean: function to clean out all files.
#
define fn-distclean
	if [ -d "${DIRECTFB_OBJECT_DIR}" ]; then \
		$(RMDIR) ${DIRECTFB_OBJECT_DIR}; \
	fi; \
	if [ -d "${DIRECTFB_INSTALL_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_DIR}; \
	fi; \
	if [ -e "${DIRECTFB_TARGET_TARBALL}" ]; then \
		$(RM) ${DIRECTFB_TARGET_TARBALL}; \
	fi; \
	if [ -e "${DIRECTFB_RELEASE_TARBALL}" ]; then \
		$(RM) ${DIRECTFB_RELEASE_TARBALL}; \
	fi; \
	if [ -d "${APPLIBS_TARGET_ROOT_DIR}/$(dir ${DIRECTFB_TARGET_ROOT_DIR})" ]; then \
		$(RM) -r ${APPLIBS_TARGET_ROOT_DIR}/$(dir ${DIRECTFB_TARGET_ROOT_DIR}); \
	fi
endef



#
# Make Functions
#
define fn-create_directfb-source
$(ECHO) "**********************************"; \
$(ECHO) "* Creating DirectFB-${DIRECTFB_VERSION} Source *"; \
$(ECHO) "**********************************"; \
$(TAR) xzkf ${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_SOURCE_TARBALL} -C $(DIRECTFB_SRC_DIR); \
if [ -d "${DIRECTFB_PUB_DFB_VER2_DIR}" ]; then \
    $(CP) -r ${DIRECTFB_PUB_DFB_VER2_DIR}/* ${DIRECTFB_SRC_DFB_DIR}; \
fi; \
$(TOUCH) ${DIRECTFB_SRC_DFB_DIR}/.unpacked
endef



############################################################
#
# directfb-all: configure and build directfb package
#
directfb-all: directfb-config directfb-compile directfb-install directfb-post-install 

##
#
# directfb-source: create the directfb source tree.
#
directfb-source: check ${DIRECTFB_UNPACKED_FLAG}


##
#
# directfb-check-config: check to see whether the build environment or build configuration has changed.
#
directfb-check-config: ${DIRECTFB_CONFIGURATION_FLAG}

DIRECTFB-AUTOGEN-DEPEND := check-autogen-tools                   \
                           directfb-check-config

ifeq ($(BUILD_FUSION),y)
DIRECTFB-AUTOGEN-DEPEND += ${FUSION_FLAG}
endif

DIRECTFB-AUTOGEN-DEPEND += ${DFB_FREETYPE_FLAG} \
                           ${DFB_JPEG_FLAG}     \
                           ${DFB_PNG_FLAG}      \
                           ${DFB_ZLIB_FLAG}
##
#
# directfb-autogen: recreate the autoconf .in and configure files from automake files.
#
directfb-autogen: ${DIRECTFB-AUTOGEN-DEPEND}
	@$(ECHO) "***********************************************"; \
	$(ECHO)  "* Autogenerating DirectFB-${DIRECTFB_VERSION} Configuration *"; \
	$(ECHO)  "***********************************************"; \
	if [ -e "${DIRECTFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${DIRECTFB_SRC_DFB_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) LDFLAGS="${LDFLAGS} ${BRCM_LDFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) PKG_CONFIG_LIBDIR=${DFB_FREETYPE_INSTALL_LIB_DIR}; \
	$(EXPORT) PKG_CONFIG_PATH=${DFB_FREETYPE_INSTALL_PKG_CONFIG_DIR}; \
	$(CHMOD) u+wx configure; \
	$(CHMOD) u+w include/*; \
	./autogen.sh $(DIRECTFB_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n******************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate DirectFB - Aborting! *"; \
		$(ECHO) "******************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/${DIRECTFB_PKG_CONFIG}.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/directfb.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/direct/direct.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/fusion/fusion.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/voodoo/voodoo.pc; \
		$(SED) -i -e "s;${DIRECTFB_PREFIX};${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/directfb-config; \
		$(TOUCH) ${DIRECTFB_CONFIGURED_FLAG}; \
	fi


DIRECTFB_CONFIG_DEPEND := directfb-check-config

ifeq ($(BUILD_FUSION),y)
DIRECTFB_CONFIG_DEPEND += ${FUSION_FLAG}
endif

DIRECTFB_CONFIG_DEPEND += ${DFB_FREETYPE_FLAG} \
                          ${DFB_JPEG_FLAG}     \
                          ${DFB_PNG_FLAG}      \
                          ${DFB_ZLIB_FLAG}

##
#
# directfb-config: force directfb to be configured
#
# Force Broadcom DFB tree to reconfig if DFB is 
# reconfiged to pick up debug flags etc.
#

directfb-config: ${DIRECTFB_CONFIG_DEPEND}
	@if [ -e "${DIRECTFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DIRECTFB_BRCM_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_CONFIGURED_FLAG}; \
	fi; \
	$(call fn-directfb-configure)


##
#
# directfb-compile: compile directfb (tracks any changes made to DFB source files)
#
# NOTE: you can specify SUBDIRS=xxxx to only compile up a specific
#       directory in DirectFB.  e.g. SUBDIRS=tools
#
directfb-compile: ${DIRECTFB_CONFIGURED_FLAG}
	@if [ -e "${DIRECTFB_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_COMPILED_FLAG}; \
	fi
	@$(call fn-directfb-compile)



##
#
# directfb-install: install necessary files into the destination dir.
#
directfb-install: ${DIRECTFB_COMPILED_FLAG}
	@if [ -e "${DIRECTFB_INSTALLED_FLAG}" ]; then \
	         $(RM) ${DIRECTFB_INSTALLED_FLAG}; \
	fi; \
	$(call fn-directfb-install)


##
#
# directfb-post-install: install additional DirectFB related files into the destination dir.
#
directfb-post-install: ${DIRECTFB_INSTALLED_FLAG}
	@$(ECHO) "**********************************"; \
	$(ECHO) "* Post-Installing DirectFB-${DIRECTFB_VERSION} *"; \
	$(ECHO) "**********************************"
	@if [ -e "${DIRECTFB_POST_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_POST_INSTALLED_FLAG}; \
	fi; \
	$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libfusion.la; \
	$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libfusion.la; \
	$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libdirect.la; \
	$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libdirect.la; \
	$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libdirectfb.la; \
	$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libdirectfb.la
ifeq ($(BUILD_VOODOO),y)
	@$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libvoodoo.la; \
	$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_INSTALL_LIB_DIR}/libvoodoo.la
endif
ifeq ($(DIRECTFB_SHARED),y)
	@-$(CP) -a ${DIRECTFB_OBJECT_DIR}/lib/*.so* ${DIRECTFB_INSTALL_LIB_DIR}
endif

	@if [ "${DIRECTFB_CLIENT_LIB}" = "y" -a "${DIRECTFB_MASTER_LIB}" = "y" ]; then \
		$(MKDIR) -p ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}; \
		$(CD) ${DIRECTFB_INSTALL_LIB_DFB_DIR}; \
		$(TAR) c * --exclude \*nexus\*  | $(TAR) x -C ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}; \
	fi; \
	$(TOUCH) ${DIRECTFB_POST_INSTALLED_FLAG}


##
#
# directfb: compile and install directfb in one pass (tracks any changes made to DFB source files)
#
# NOTE: you can specify SUBDIRS=xxxx to only compile up a specific
#       directory in DirectFB.  e.g. SUBDIRS=tools
#
directfb: directfb-post-install

##
#
# directfb-uninstall: uninstall necessary files from the destination dir.
#
directfb-uninstall:
	@if [ -e "${DIRECTFB_POST_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_POST_INSTALLED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_SRC_DFB_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_SRC_DFB_DIR} uninstall DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_DATA_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_DATA_DIR}/*; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_SHARE_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_SHARE_DIR}/*; \
	fi

##
#
# directfb-clean: remove all intermediate object and dependency files as well as destination dir.
#

directfb-clean: directfb-uninstall
	@if [ -e "${DIRECTFB_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_COMPILED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_FLAG}" ]; then \
		$(RM) ${DIRECTFB_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_SRC_DFB_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_SRC_DFB_DIR} clean; \
	fi


##
#
# directfb-distclean: remove all directfb files with option to remove source tree.
#
directfb-distclean: directfb-clean
	@-$(call fn-directfb-distclean)
	@if [ -d "${DIRECTFB_SRC_DFB_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DIRECTFB_SRC_DFB_DIR} completely."; \
		$(ECHO) "Removing ${DIRECTFB_SRC_DFB_DIR}..."; \
		$(RM) -r ${DIRECTFB_SRC_DFB_DIR}; \
	fi

##
#
# directfb-mrproper: remove all directfb files including the source tree.
#
directfb-mrproper: directfb-clean
	@-$(call fn-directfb-distclean)
	@if [ -d "${DIRECTFB_SRC_DFB_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_SRC_DFB_DIR}; \
	fi

##
#
# fn-directfb-configure: function to configure directfb.
#
define fn-directfb-configure
	$(ECHO) "******************************"; \
	$(ECHO) "* Configuring DirectFB-${DIRECTFB_VERSION} *"; \
	$(ECHO) "******************************"; \
	$(CD) ${DIRECTFB_SRC_DFB_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) LDFLAGS="${LDFLAGS} ${BRCM_LDFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) PKG_CONFIG_LIBDIR=${DFB_FREETYPE_INSTALL_LIB_DIR}:${PKG_CONFIG_LIBDIR}; \
	$(EXPORT) PKG_CONFIG_PATH=${DFB_FREETYPE_INSTALL_PKG_CONFIG_DIR}:${PKG_CONFIG_PATH}; \
	$(CHMOD) -f u+wx configure; \
	$(CHMOD) -f u+w include/*; \
	./configure $(DIRECTFB_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***************************************************"; \
		$(ECHO)      "* ERROR: Failed to configure DirectFB - Aborting! *"; \
		$(ECHO)      "***************************************************"; \
	    $(FALSE); \
	else \
	    if [ ! -d ${DIRECTFB_SRC_DFB_DIR}/wm/default/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_SRC_DFB_DIR}/wm/default/.libs; \
		fi; \
	    if [ ! -d ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBFont/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBFont/.libs; \
		fi; \
	    if [ ! -d ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBImageProvider/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBImageProvider/.libs; \
		fi; \
	    if [ ! -d ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBVideoProvider/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_SRC_DFB_DIR}/interfaces/IDirectFBVideoProvider/.libs; \
		fi; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/${DIRECTFB_PKG_CONFIG}.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/directfb.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/direct/direct.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/fusion/fusion.pc; \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${DIRECTFB_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${DIRECTFB_INSTALL_LIB_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/lib/voodoo/voodoo.pc; \
		$(SED) -i -e "s;prefix=${DIRECTFB_PREFIX};prefix=${DIRECTFB_INSTALL_ROOT_DIR};g" ${DIRECTFB_SRC_DFB_DIR}/directfb-config; \
		$(TOUCH) ${DIRECTFB_CONFIGURED_FLAG}; \
	fi
endef



##
#
# fn-directfb-compile: function to compile directfb
#

define fn-directfb-compile
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) BRCM_CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) CPI_OBJECT_DIR="${DIRECTFB_OBJECT_DIR}"; \
	$(ECHO) "***************************"; \
	$(ECHO) "* Building DirectFB-${DIRECTFB_VERSION} *"; \
	$(ECHO) "***************************"; \
	$(CD) ${DIRECTFB_SRC_DFB_DIR} && \
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${DIRECTFB_INSTALL_DIR} && \
		if [ $$? -ne 0 ]; then \
			$(ECHO) "***********************************************"; \
			$(ECHO) "* ERROR: Failed to build DirectFB - Aborting! *"; \
			$(ECHO) "***********************************************"; \
			$(FALSE); \
		else \
			$(TOUCH) ${DIRECTFB_COMPILED_FLAG}; \
	fi
endef


##
#
# fn-directfb-install: function to install directfb
#
define fn-directfb-install
	$(ECHO) "*****************************"; \
	$(ECHO) "* Installing DirectFB-${DIRECTFB_VERSION} *"; \
	$(ECHO) "*****************************"; \
	$(CD) ${DIRECTFB_SRC_DFB_DIR}; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) BRCM_CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) CPI_OBJECT_DIR="${DIRECTFB_OBJECT_DIR}"; \
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) "*************************************************"; \
	    $(ECHO) "* ERROR: Failed to install DirectFB - Aborting! *"; \
		$(ECHO) "*************************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DIRECTFB_VERSION}"      > ${DIRECTFB_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DIRECTFB_SRC_DFB_DIR}" >> ${DIRECTFB_INSTALLED_FLAG}; \
	fi
endef



##
#
# fn-directfb-distclean: function to clean out all files.
#
define fn-directfb-distclean
	if [ -e "${DIRECTFB_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DIRECTFB_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DIRECTFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DIRECTFB_SRC_DFB_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_SRC_DFB_DIR} distclean; \
	fi; \
	if [ -d "${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb; \
	fi; \
	if [ -d "${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb-internal" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb-internal; \
	fi; \
	if [ -d "${DIRECTFB_INSTALL_LIB_DIR}/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}-0" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_LIB_DIR}/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}-0; \
	fi; \
	if [ -d "${DIRECTFB_INSTALL_SHARE_DIR}/directfb-${DIRECTFB_VERSION}" ]; then \
		$(RM) -r ${DIRECTFB_INSTALL_SHARE_DIR}/directfb-${DIRECTFB_VERSION}; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_UNPACKED_FLAG}:
	@if ! [ -e "${DIRECTFB_SRC_DFB_DIR}" ]; then \
		${MKDIR} "${DIRECTFB_SRC_DFB_DIR}";\
	fi
	@if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_SOURCE_TARBALL}'!";\
		$(FALSE);\
	elif [ `$(STAT) -t "${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_SOURCE_TARBALL}" | $(CUT) -d\  -f2` -eq 0 ]; then \
		$(ECHO) "ERROR: Size of '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_SOURCE_TARBALL}' is 0!";\
		$(FALSE);\
	else\
		$(call fn-create_directfb-source);\
	fi

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_CONFIGURATION_FLAG}: directfb-defines ALWAYS
	@$(ECHO) -n "GCC_VERSION="                                          > ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion                           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                                          >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine                           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"                         >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "NEXUS_PLATFORM=${NEXUS_PLATFORM}"                         >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "BCHP_VER=${BCHP_VER}"                                     >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "TRACE=${TRACE}"                                           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "SMP=${SMP}"                                               >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"                           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"                             >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}"                   >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "GL_SUPPORT=${GL_SUPPORT}"                                 >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"                       >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_APPLIBS=${DIRECTFB_APPLIBS}"                     >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"                       >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_MULTI=${DIRECTFB_MULTI}"                         >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_ACCEL=${DIRECTFB_ACCEL}"                         >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_IR_INPUT=${DIRECTFB_IR_INPUT}"                   >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_KEY_INPUT=${DIRECTFB_KEY_INPUT}"                 >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SID=${DIRECTFB_SID}"                             >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SW_SMOOTH_SCALING=${DIRECTFB_SW_SMOOTH_SCALING}" >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SW_DITHERING=${DIRECTFB_SW_DITHERING}"           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_LINOTYPE=${DIRECTFB_LINOTYPE}"                   >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_GLES_SUPPORT=${DIRECTFB_GLES_SUPPORT}"           >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_TESTS=${BUILD_TESTS}"                               >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_FUSION=${BUILD_FUSION}"                             >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_VOODOO=${BUILD_VOODOO}"                             >> ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$; \
	if [ -e "${DIRECTFB_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_CONFIGURATION_FLAG} ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} directfb-clean; \
			$(CP) ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$ ${DIRECTFB_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$ ${DIRECTFB_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_SRC_DFB_DIR}/.config.$$$$


ifeq ($(BUILD_FUSION),y)
DIRECTFB_CONFIGURED_DEPEND := ${FUSION_FLAG}
endif

DIRECTFB_CONFIGURED_DEPEND += ${DFB_FREETYPE_FLAG} \
                              ${DFB_JPEG_FLAG}     \
                              ${DFB_PNG_FLAG}      \
                              ${DFB_ZLIB_FLAG}     \
                              ${DIRECTFB_CONFIGURATION_FLAG}

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_CONFIGURED_FLAG}: ${DIRECTFB_CONFIGURED_DEPEND}
	@$(call fn-directfb-configure)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_COMPILED_FLAG}: ${DIRECTFB_CONFIGURED_FLAG}
	@$(call fn-directfb-compile)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_INSTALLED_FLAG}: ${DIRECTFB_COMPILED_FLAG}
	@$(call fn-directfb-install)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_POST_INSTALLED_FLAG}: ${DIRECTFB_INSTALLED_FLAG} directfb-post-install 
	@$(call fn-directfb-post-install)


##
#
# Dependency target not to be called directly.
#
${DIRECTFB_FLAG}: ${DIRECTFB_POST_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${DIRECTFB_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${DIRECTFB_PKG_CONFIG} > \
					 ${DIRECTFB_SRC_DFB_DIR}/.install.$$$$; \
	if [ -e "${DIRECTFB_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_FLAG} ${DIRECTFB_SRC_DFB_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_SRC_DFB_DIR}/.install.$$$$ ${DIRECTFB_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_SRC_DFB_DIR}/.install.$$$$ ${DIRECTFB_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_SRC_DFB_DIR}/.install.$$$$

-include $(DIRECTFB_OBJECT_DIR)/*.d
