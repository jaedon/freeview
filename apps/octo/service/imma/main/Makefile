obj-y += imma.o

# Additional include path
ccflags-y += -I../../make/include

ifeq ($(CONFIG_DEBUG),y)
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/debug/include
else
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/release/include
endif
