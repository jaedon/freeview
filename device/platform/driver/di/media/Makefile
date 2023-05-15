#
#add by junsy (for media and iptuner)
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/bfile
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/utils
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/media
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/avi
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/asf

inccflag-y += -I$(HUMAX_WORK_DIR)/platform/driver/di/iptuner

# to know the nexus tree version.
# in HumaxDefaultRule.mak, below config file is not included. So, we do it here.
include $(HUMAX_WORK_DIR)/platform/sdk/driver/bcm_environment.config

# what is the difference between NEXUS_TREE and SECURITY_TREE_1401?
ifeq ($(NEXUS_TREE),TREE_1401)
 # below playready20/ folder is not belong to broadcom. it is from microsoft. 
 # in di media 1.0 with nexus 14.01, we should use microsoft code.
 BSEAV = $(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV
 include $(BSEAV)/lib/playready20/playready.inc

 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready20/compat
 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready20/source/inc
 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/common_crypto/include
 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/basemodules/mem
 inccflag-y += $(addprefix -I,$(HUMAX_MSPRDY_INCLUDES))
 inccflag-y += $(addprefix -D,$(HUMAX_MSPRDY_DEFINES))
else
 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready
 inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready/inc
endif

HUMAX_EXTRA_CFLAG += -DDRM_BUILD_PROFILE=10 -DTARGET_LITTLE_ENDIAN=1

ifeq ($(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE), y)
HUMAX_EXTRA_CFLAG += -DPRDY_ROBUSTNESS
endif

ifneq ($(CONFIG_PLAYREADY_20),)
# new MSDRM PRDY header file generates many warnings with -pedantic option.
# ex:  warning: ISO C does not allow extra ';' outside of a function..
# so, we remove -pedantic option only for this (and subfolder if exist) source files
HUMAX_EXTRA_CFLAG := $(filter-out -pedantic,$(HUMAX_EXTRA_CFLAG))
endif

obj-y := di_media.o

#------------------------------------------------------------------
ifeq ($(CONFIG_PLAYREADY),y)
 ifeq ($(NEXUS_TREE),TREE_1401)
  fullpath-obj-y += $(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready20/compat/prdy_http.o
 else
  # for old nexus tree, prdy_xxx.o is built in broadcom nexus transport.inc.
 endif
endif
#------------------------------------------------------------------

