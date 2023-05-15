#
obj-y := di_cm.o
obj-y += BcmMibPeek.o
obj-y += BcmCannedSnmpSwUpgrade.o

ifneq ($(CONFIG_CABLE_MODEM_BRCM_DSGCC),)
obj-y += bdsgcc/
endif

ifneq ($(CONFIG_CABLE_MODEM_BRCM_RPC),)
obj-y += brpc/
endif


