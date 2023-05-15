#
# If TESTBOX is true then use bcmTestWrapper 
#
ifeq (${TESTBOX},true)
OBJS = bcmTestWrapper.o testLanIF.o testWanIF.o testUserIF.o testSysIF.o
OBIN=	bcmWrapperlib.o 
else
OBJS = bcmWrapper.o bcmLanIF.o bcmWanIF.o bcmUserIF.o bcmSysIF.o
OBIN=	bcmWrapperlib.o
endif

ifeq ($(strip $(BRCM_VODSL_CFGMGR)),1)
ifeq ($(strip $(SIPLOAD)),1)
OBJS +=  bcmVoiceWrapper.o
CFLAGS += -DWT104_SUPPORT
CFLAGS += -DVOIP_CFGMGR
CFLAGS += -I$(USERAPPS_DIR)/broadcom/vodsl/voip/cfgmgr
CFLAGS += -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/codec
CFLAGS += -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/endpt/inc 
CFLAGS += -I$(XCHANGE_DIR)/dslx_common/xchg_common/bos/publicInc 
CFLAGS += $(BRCM_DSP_CODEC_DEFINES)
CFLAGS +=  -DFLEXIBLE_CONFERENCING_SUPPORTED=1                                           \
                 -DLINUX -DBOS_OS_LINUXUSER                                                    \
                 -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(KERNEL_DIR)/include/asm/mach-generic                                    \
                 -I$(INC_ENDPOINT_PATH)                                                        \
                 -I$(INC_ENDPOINT_PATH)/cfg                                                    \
                 -I$(XCHANGE_DIR)/dslx/src/mta/inc/cfg                    \
                 -I$(XCHANGE_DIR)/dslx/apps/$(XCHANGE_BUILD_APP)/cfginc/mta                    \
                 -I$(XCHANGE_DIR)/dslx/apps/$(XCHANGE_BUILD_APP)/cfginc/xchg_common            \
                 -I$(XCHANGE_DIR)/dslx/apps/$(XCHANGE_BUILD_APP)/cfginc/xdrv                   \
                 -I$(XCHANGE_DIR)/dslx/apps/$(XCHANGE_BUILD_APP)/cfginc/vrg                    \
                 -I$(XCHANGE_DIR)/dslx/apps/cfginc                                             \
                 -I$(XCHANGE_DIR)/dslx/apps/cfginc/default                                     \
                 -I$(XCHANGE_DIR)/dslx/apps/cfginc/xchg_common                                 \
                 -I$(XCHANGE_DIR)/dslx/apps/cfginc/xdrv                                        \
                 -I$(XCHANGE_DIR)/dslx/apps/cfginc/vrg                                         \
                 -I$(XCHANGE_DIR)/dslx_common/xchg_common/bos/publicInc                        \
                 -I$(XCHANGE_DIR)/dslx_common/xchg_common/assert/cfginc                        \
                 -I$(XCHANGE_DIR)/dslx_common/xchg_common/assert/inc                           \
                 -I$(XCHANGE_DIR)/dslx_common/xchg_common/str                                  \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/inc                                 \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/casCtl/inc                          \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/endpt/inc                           \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/endpt/vrg/cfginc                    \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/endpt/vrg/inc                       \
                 -I$(XCHANGE_DIR)/dslx_common/voice_res_gw/codec                               \
                 -I$(XCHANGE_DIR)/dslx_common/xchg_drivers/inc                                 \
                 -I$(XCHANGE_DIR)/dslx_common/prot_callctrl/apps/vrg/vrgcallmgr/cfginc         \
                 -I$(XCHANGE_DIR)/dslx_common/prot_callctrl/apps/vrg/vrgcallmgr/inc
endif
endif

$(OBIN):	$(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	rm -f *~ $(OBJS) $(OBIN)

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)




