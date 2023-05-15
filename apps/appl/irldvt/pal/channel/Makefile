obj-y								+= pal_channel.o
#added by hmjoo
obj-y								+= pal_channel_sat.o
obj-$(CONFIG_MW_CH_SATELLITE)		+= pal_channel_sat.o
obj-$(CONFIG_MW_CH_TERRESTRIAL)		+= pal_channel_ter.o
obj-$(CONFIG_MW_CH_CABLE)			+= pal_channel_cab.o

# Basic include path
#include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.pal
# Additional include path
#ccflags-y += -I$(OCTO_TVSERVICE_DIR)/db/include
#ccflags-y += -I$(OCTO_TVSERVICE_DIR)/mw/common/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/pal/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/include
#ccflags-y += -I$(OCTO_TVSERVICE_DIR)/mw/include
