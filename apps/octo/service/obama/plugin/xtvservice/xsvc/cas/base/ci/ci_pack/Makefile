obj-y += itc.o
obj-y += link.o
obj-y += linkfifo.o
obj-y += linkstat.o
obj-y += physical.o
obj-y += phystat.o
obj-y += sesscod.o
obj-y += session.o
obj-y += sesstat.o
obj-y += tranfifo.o
obj-y += trans.o
obj-y += transcod.o
obj-y += transtat.o

ifeq ($(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER),y)
	obj-y += physdrv_useExtCtrl.o
else
	obj-y += physdrv.o
endif

#obj-y += simrm.o
#obj-y += pdriver.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/pal/include

# Additional include path

