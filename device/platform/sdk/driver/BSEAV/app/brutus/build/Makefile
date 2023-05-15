############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 210 $
# $brcm_Date: 9/18/12 11:16a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus/build/Makefile $
# 
# 210   9/18/12 11:16a mward
# SW7425-3739: Eliminate the make -q, to check driver make, just make it.
# Otherwise, build of 2.6.18 platform reports an error which does not
# stop the build, but disrupts AUTOTEST build.
# 
# 209   9/13/12 7:03p mphillip
# SW7425-3739: Update makefiles for new linux/driver/build hierarchy
# 
# 208   9/6/12 11:27p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:15p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 207   8/2/12 2:09p erickson
# SW7400-2511: use NEXUS_PLATFORM, not PLATFORM
# 
# 206   8/1/12 7:06p mward
# SW7425-2690:  Default on POWERSTANDBY_SUPPORT for 7019, 7125, 7425,
# 7435.
# 
# 205   6/27/12 2:58p jtna
# SW7425-3350: rework Brutus MTSIF frontend handling
# 
# 204   6/11/12 12:02p cdisc
# SW7425-3067: updating Makefile
# 
# 201   6/6/12 11:26a mward
# SW7435-101:  Don't condition copy libs with LIVE_STREAMING_SUPPORT,
# that build option is no longer used, and is now always enabled.
# 
# 200   5/13/12 5:06p jtna
# SW7425-2752: add NEXUS_FRONTEND_FIXED_MTSIF_CONFIG option to support
# legacy mode
# 
# 199   4/11/12 5:53p mward
# SW7435-101:  Don't copy libs for LIVE_STREAMING_SUPPORT unless
# PLAYBACK_IP_SUPPORT=y, which is required.
# 
# 198   4/3/12 1:56p jrubio
# SW7425-2664: fix 2618 platforms
# 
# 197   3/23/12 5:55p mward
# SW7425-2151:  Put the libtshdrbuilder.so into the refsw tarball.  Copy
# it from nexus/bin.
# 
# 196   3/22/12 10:04a erickson
# SW7425-2664: chmod +x settop
# 
# 195   3/21/12 6:32p mward
# SW7425-2151: Back out change to libtshdrbuilder.so into the refsw
# tarball. Trouble with fresh view.
# 
# 194   3/21/12 5:18p mward
# SW7425-2151:  Put the libtshdrbuilder.so into the refsw tarball.
# 
# 193   3/21/12 4:28p erickson
# SW7425-2664: remove unused code
# 
# 192   3/21/12 3:57p mward
# SW7425-2664:  remove cheapdebug from OTHER_MAKES, it was removed
# elsewhere.
# 
# 191   3/21/12 11:42a erickson
# SW7425-2664: remove old logic
# 
############################################################

#
# Reference Software Makefile
#
# This Makefile will build 3 things
# 1) driver(s)
# 2) settop api (shared or static)
# 3) brutus
#

include ../../../api/build/tools.mak
BSEAV = $(shell cd "../../.." && ${PWD})
NEXUS = $(shell cd "../../../../nexus" && ${PWD})
BINDIR ?= ${BSEAV}/bin
ifeq ($(NEXUS_PLATFORM),)
# PLATFORM is deprecated
NEXUS_PLATFORM = $(PLATFORM)
endif
ifeq ($(NEXUS_MODE),)
# MODE is deprecated
NEXUS_MODE = $(MODE)
endif

ifeq ($(SYSTEM),vxworks)
APP = brutus.out
else
APP = brutus
endif
DRIVERDIR=$(BSEAV)/${SYSTEM}/driver/build

ifeq ($(SYSTEM),vxworks)
OTHER_MAKES = api
else
OTHER_MAKES = driver api
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97550),$(NEXUS_PLATFORM))
ifeq ($(filter $(BCHP_VER), A0 A1),$(BCHP_VER))
ifeq ($(SOFT_GFX_SUPPORT),)
export SOFT_GFX_SUPPORT=y
endif
endif
export NEXUS_USE_SW_FILTER=y
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97019 97125 97420 97408 97425 97435),$(NEXUS_PLATFORM))
ifeq ($(POWERSTANDBY_SUPPORT),)
export POWERSTANDBY_SUPPORT=y
endif
else
ifeq ($(POWERSTANDBY_SUPPORT),)
export POWERSTANDBY_SUPPORT=n
endif
endif

ifeq ($(MSDRM_ND_SUPPORT),y)
DRMND_SUPPORT=y
endif

# These aren't supported in VxWorks.
ifeq ($(SYSTEM),vxworks)
# IP playback only being supported on Linux platforms
PLAYBACK_IP_SUPPORT=n
NETACCEL_STANDALONE=n
BCMEMAC_SUPPORT=n
endif

# Brutus now defaults to IP Settop mode for most platforms (some exceptions like 97400)
# You can turn this default off with PLAYBACK_IP_SUPPORT=n
ifneq ($(filter $(NEXUS_PLATFORM), 97400 97325 97420 97410 97409 93380vms 97550 ), $(NEXUS_PLATFORM))
ifneq ($(PLAYBACK_IP_SUPPORT),n)
PLAYBACK_IP_SUPPORT=y
export PLAYBACK_IP_SUPPORT
endif
endif

# Enable IPSTB related env variable if PLAYBACK_IP_SUPPORT is on
ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(DTCP_IP_SUPPORT),y)
SSL_SUPPORT=y
export SSL_SUPPORT
ifneq ($(DTCP_IP_HARDWARE_DECRYPTION),n)
DTCP_IP_HARDWARE_DECRYPTION=y
endif #DTCP_IP_HARDWARE_DECRYPTION
ifneq ($(DTCP_IP_HARDWARE_ENCRYPTION),n)
DTCP_IP_HARDWARE_ENCRYPTION=y
endif #DTCP_IP_HARDWARE_ENCRYPTION
export DTCP_IP_HARDWARE_ENCRYPTION
export DTCP_IP_HARDWARE_DECRYPTION
export DTCP_IP_SUPPORT
endif
ifeq ($(SSL_SUPPORT),y)
OPENSSL_DIR := $(BSEAV)/lib/openssl/src
export OPENSSL_DIR
endif

ifeq ($(NETACCEL_SUPPORT),y)
ifeq ($(B_HAS_PLAYPUMP_IP),y)
NETACCEL_STANDALONE=n
else
NETACCEL_STANDALONE=y
export NETACCEL_STANDALONE
endif
BCMEMAC_SUPPORT=y
export BCMEMAC_SUPPORT
endif #NETACCEL_SUPPORT

ifneq ($(LIVEMEDIA_SUPPORT),n)
LIVEMEDIA_SUPPORT=y
export LIVEMEDIA_SUPPORT
endif #LIVEMEDIA_SUPPORT

# following platforms didn't load psip lib
ifneq ($(filter $(NEXUS_PLATFORM),97550 97358 97552), $(NEXUS_PLATFORM))
#When Live Streaming is ON, we turn on auto PSI as well unless it is specifically off
ifneq ($(AUTO_PSI_SUPPORT),n)
AUTO_PSI_SUPPORT=y
export AUTO_PSI_SUPPORT
endif
endif
endif #PLAYBACK_IP_SUPPORT

.PHONY: settop bin_install $(OTHER_MAKES) brutus
.PHONY: do_install

#
# The name of the installation tar ball
#
BUILDDATE = `date +%Y%m%d`
BRUTUS_TAR_BALL = refsw-$(BUILDDATE).$(NEXUS_PLATFORM)-$(SYSTEM)
BRUTUS_TAR_BALL := $(BRUTUS_TAR_BALL)-uclibc
BRUTUS_TAR_BALL := $(BRUTUS_TAR_BALL).bin.tgz

#
# Set B_REFSW_SHAREABLE based on a variety of flags
#
ifeq ($(filter y,$(STATIC_SETTOPAPI) $(STATIC_BRUTUS)),y)
B_REFSW_SHAREABLE=n
endif
ifeq ($(filter n,$(B_REFSW_SHAREABLE) $(SHAREABLE)),n)
B_REFSW_SHAREABLE=n
endif

# bin_install is the default rule.
# 'all' builds everything but doesn't perform bin_install or install.
# bin_install copies only binaries into BSEAV/bin. It allows you to
#  run from that directory without having your configuration files overwritten.
# install builds an installation tarball in BSEAV/bin.

bin_install: all settop ${BINDIR}
	@echo [Install... binaries]
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(BINDIR) install
ifneq (${B_REFSW_SHAREABLE},n)
	${Q_}$(CP) $(BSETTOP_LIBS) ${BINDIR}
ifeq ($(SYSTEM),linux)
	@(cd ${BINDIR};touch -f ${APP} $(notdir ${BSETTOP_LIBS}))
endif
endif
ifeq ($(SYSTEM),linux)
	${Q_}$(MAKE) INSTALL_DIR=$(BINDIR) -C $(DRIVERDIR) install
endif
ifeq ($(SYSTEM),vxworks)
ifneq (${B_REFSW_SHAREABLE=n},n)
#       Timesaving step: combine all the objects into a single load module
	@echo "[Combining load modules...]"
#       Note: get lots of errors combining libs and brutus at present time.
	${Q_}$(LD) -r -o libs.out $(BSETTOP_LIBS)
	@$(CP) libs.out ${BINDIR}
endif
endif
ifeq ($(MAKECMDGOALS),)
	@echo "Build Complete"
endif

# this must come after bin_install
include $(BSEAV)/api/include/api.mak
include $(BSEAV)/api/build/tools.mak

as-lib: check_environment $(OTHER_MAKES) brutus_lib
all: check_environment $(OTHER_MAKES) brutus
clean: check_environment $(addprefix clean-,$(OTHER_MAKES)) clean-brutus

print-brutus-tarball:
	@echo $(BRUTUS_TAR_BALL)

${BINDIR}:
	$(Q_)${MKDIR} ${BINDIR}

ifeq ($(BSETTOP_LIBS),)
LIB_STRING = ${APP}
else
LIB_STRING = $(notdir $(BSETTOP_LIBS))
endif

ifeq ($(SYSTEM),vxworks)
settop:
	@echo "No settop script for vxworks"
else
settop: ${BINDIR}
	@${CP} -f $@ ${BINDIR}/$@
	@chmod +x ${BINDIR}/$@
endif

#
# Pass link options into Settop API build.
#
ifneq ($(B_REFSW_SHAREABLE),)
LINK_OPTION = B_REFSW_SHAREABLE=$(B_REFSW_SHAREABLE)
endif

api:
	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/build

clean-api:
	${Q_}$(MAKE) -C $(BSETTOP)/build veryclean
driver:
	${Q_}$(MAKE) -C $(DRIVERDIR)
clean-driver:
	${Q_}$(MAKE) -C $(DRIVERDIR) clean
brutus_lib: api
	${Q_}$(MAKE) -f Makefile.brutus as_lib
brutus: api
	${Q_}$(MAKE) -f Makefile.brutus
clean-brutus:
	${Q_}$(MAKE) -f Makefile.brutus clean

INSTALL_DIR = ${BINDIR}/setup
ifeq ($(SYSTEM),vxworks)
PRERENDERED_FONTS = fonts/mswebfonts/be/*.bwin_font
else
ifeq ($(B_REFSW_ARCH),mips-linux)
PRERENDERED_FONTS = fonts/mswebfonts/be/*.bwin_font
else
PRERENDERED_FONTS = fonts/mswebfonts/le/*.bwin_font
endif
endif

#
# This is the main installation rule.
# It will build the binaries and utilities, and then
# perform the actual installation procedure.
#
install: bin_install
	${Q_}$(MAKE) do_install

#
# INSTALL_DIR is phony because we always want to remove it and
# recreate it.
#
.PHONY: $(INSTALL_DIR)
$(INSTALL_DIR):
	@$(RM) -rf $(INSTALL_DIR)
	@${MKDIR} $(INSTALL_DIR)

#
# Copy everything into temporary INSTALL_DIR and tar it up.
# The final tarball is left in BINDIR.
#
do_install:  $(INSTALL_DIR) install-binaries install-support-files make-tarball

ifeq ($(SYSTEM), vxworks)
make-tarball:
else
make-tarball:install-binaries install-support-files
endif
	@echo [Build..... tarball]
	@(cd ${INSTALL_DIR};tar -pzckf ${BINDIR}/${BRUTUS_TAR_BALL} .)
	@$(RM) -rf $(INSTALL_DIR)
	@echo ""
	@echo "*********************************************"
	@echo "* Installation tarball created in $(BINDIR)."
	@echo "*"
	@echo "* To complete installation perform the"
	@echo "* following operations on the settop box:"
	@echo "*"
	@echo "* gunzip -c ${BRUTUS_TAR_BALL} | tar xvpf -"
	@echo "* settop brutus"
	@echo "*********************************************"
	@echo ""

#
# Install drivers, libraries and apps
#
.PHONY: install-binaries
install-binaries: $(INSTALL_DIR)
ifneq ($(SYSTEM),vxworks)
	${Q_}$(MAKE) INSTALL_DIR=$(INSTALL_DIR) -C $(DRIVERDIR) install
endif
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(INSTALL_DIR) install
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(INSTALL_DIR) install-config-files
	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/build INSTALL_DIR=$(INSTALL_DIR) install

#
# For some environments, we need to grab libraries from the
# toolchain because they are not present in the rootfs by default.
#

LINUX_VER_EQ_2_6_18 = $(shell (grep -m 1 '^VERSION = 2' ${LINUX}/Makefile >/dev/null && \
	grep -m 1 '^PATCHLEVEL = 6' ${LINUX}/Makefile >/dev/null && \
	grep -m 1 '^SUBLEVEL = 18' ${LINUX}/Makefile >/dev/null && echo 'y'))

ifeq ($(LINUX_VER_EQ_2_6_18),y)
TOOLCHAIN_LIB_DIR:=$(shell dirname `$(CC) -print-file-name=libgcc_s.so` 2>/dev/null)
endif

.PHONY: install-support-files
ifeq ($(SYSTEM),vxworks)
install-support-files:
else
install-support-files: $(INSTALL_DIR)
endif
ifeq ($(findstring linux,$(B_REFSW_ARCH)),linux)
	@echo [Install... toolchain libraries]
ifeq ($(LINUX_VER_EQ_2_6_18),y)
# more recent OS's have libstdc++ already
	$(Q_)$(CP) -d $(TOOLCHAIN_LIB_DIR)/libstdc++.so* $(INSTALL_DIR)
	$(Q_)$(CP) -d $(TOOLCHAIN_LIB_DIR)/libgcc_s.so* $(INSTALL_DIR)
	$(Q_)$(STRIP) --strip-all $(INSTALL_DIR)/libstdc++.so*
	$(Q_)$(STRIP) --strip-all $(INSTALL_DIR)/libgcc_s.so*
endif
ifeq ($(BLUETOOTH_SUPPORT),y)
	#$(Q_)$(CP) -d $(BSEAV)/lib/bluetooth/bluez-libs-2.25/src/.libs/sbin/hciconfig $(INSTALL_DIR)
endif
endif
	@echo [Install... scripts]
	$(Q_)$(CP) $(BINDIR)/settop $(INSTALL_DIR)
ifeq ($(DTCP_IP_SUPPORT),y)
	@echo [Install... dtcp_ip keys]
	@$(CP)  $(NEXUS)/lib/dtcp_ip/data/* $(INSTALL_DIR)
endif
ifeq ($(PLAYBACK_IP_SUPPORT),y)
	@echo [Install... live streaming]
	@$(CP)  $(NEXUS)/bin/libtshdrbuilder.so $(BINDIR)
	@$(CP)  $(BINDIR)/libtshdrbuilder.so $(INSTALL_DIR)
endif #PLAYBACK_IP_SUPPORT
	@echo [Install... fonts]
	@${MKDIR} $(INSTALL_DIR)/fonts
	@$(CP) $(PRERENDERED_FONTS) $(INSTALL_DIR)/fonts
ifeq ($(MSDRM_PRDY_SUPPORT),y)
ifeq ($(MSDRM_PD_SUPPORT),y)
        $(error MSDRM_PD_SUPPORT and MSDRM_PRDY_SUPPORT cannot be enabled at the same time.)
else
ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
ifneq ($(findstring libnexus_security,$(NEXUS_EXTRALIBS)), libnexus_security)
ifneq ($(BHSM_KEYLADDER), ON)
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Please include the libnexus_security lib in the NEXUS_EXTRALIBS export, or enable following exports .. BHSM_KEYLADDER=ON,BHSM_OTPMSP=ON,HSM_SOURCE_AVAILABLE=y)
endif
ifneq ($(BHSM_OTPMSP), ON)
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Please include the libnexus_security lib in the NEXUS_EXTRALIBS export, or enable following exports .. BHSM_KEYLADDER=ON,BHSM_OTPMSP=ON,HSM_SOURCE_AVAILABLE=y)
endif
ifneq ($(HSM_SOURCE_AVAILABLE), y)
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Please include the libnexus_security lib in the NEXUS_EXTRALIBS export, or enable following exports .. BHSM_KEYLADDER=ON,BHSM_OTPMSP=ON,HSM_SOURCE_AVAILABLE=y)
endif
endif
ifneq ($(KEYLADDER_SUPPORT), y)
ifndef NEXUS_SECURITY_KEYLADDER_EXTENSION_INC
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Define KEYLADDER_SUPPORT for 65nm plat or NEXUS_SECURITY_KEYLADDER_EXTENSION_INC for 40nm)
endif
endif
ifneq ($(USERCMD_SUPPORT), y)
ifndef NEXUS_SECURITY_USERCMD_EXTENSION_INC
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Define USERCMD_SUPPORT for 65nm plat or NEXUS_SECURITY_USERCMD_EXTENSION_INC for 40nm)
endif
endif
ifneq ($(OTPMSP_SUPPORT), y)
ifndef NEXUS_SECURITY_OTPMSP_EXTENSION_INC
$(error PRDY_ROBUSTNESS_ENABLE is enabled. Define OTPMSP_SUPPORT for 65nm plat or NEXUS_SECURITY_OTPMSP_EXTENSION_INC for 40nm)
endif
endif
	@echo [Install... playready data files - PRDY_ROBUSTNESS_ENABLE is enabled, no dat files will be copied]
else
	@echo [Install... playready data files]
	@$(CP) $(BSEAV)/lib/playready/samples/dat/*.dat $(INSTALL_DIR)
endif
endif
endif


ifeq ($(DRMND_SUPPORT),y)
	@echo [Install... drmnd]
	${Q_}$(MAKE) -C $(BSEAV)/lib/drmnd/build INSTALL_DIR=$(INSTALL_DIR) install
endif
ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(ASF_SUPPORT),y)
ifeq ($(WMS_SUPPORT),y)
	@echo [Install... asfip]
	${Q_}$(MAKE) -C $(BSEAV)/lib/rtp/asf/build INSTALL_DIR=$(INSTALL_DIR) install
endif
endif
endif
ifeq ($(BRUTUS_LIBTUNE_SUPPORT),y)
	@echo [Install... libtune]
	$(Q_)$(CP) $(BINDIR)/libtune.so $(INSTALL_DIR)
	$(Q_)$(CP) -d $(BINDIR)/libsqlite3.so* $(INSTALL_DIR)
# alternatively, it could be copied from AppLibs/opensource/sqlite/sqlite-3.6.23.1/97420b0/lib/
endif
