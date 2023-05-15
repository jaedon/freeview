#
#	Makefile for Plugin
#
#	Written by Jongwon Kim
#		2011. 11. 08.
#

-include ../../../build/config.mk

MAKE	:= make

### Targets
all: plugin

plugin:
	echo $(CXX)
	$(MAKE) -C tvportal

ifeq ($(CONFIG_PROD_HMS1000S),y)
	$(MAKE) -C maxdome
endif

ifeq ($(CONFIG_PROD_ICORDPRO),y)
	$(MAKE) -C maxdome
endif

ifeq ($(CONFIG_PROD_HDNEO),y)
	$(MAKE) -C maxdome
endif

ifeq ($(CONFIG_PROD_HGS1000S),y)
	$(MAKE) -C maxdome
endif

ifeq ($(CONFIG_DEBUG), y)
	$(MAKE) -C debug
endif

ifeq ($(CONFIG_OP_FREESAT),y)
	$(MAKE) -C freesat
endif

ifeq ($(CONFIG_OP_MIDDLE_EAST),y)
	$(MAKE) -C hmxdrm
endif

ifeq ($(CONFIG_OP_AUS_DTT),y)
	$(MAKE) -C hmxdrm
endif

ifeq ($(USE_OPERA_TVSTORE),YES)
	$(MAKE) -C otvs
endif

ifeq ($(CONFIG_PROD_HD9000I),y)
	$(MAKE) -C webapp_upgrade
endif

ifeq ($(CONFIG_NETFLIX_APP),y)
	$(MAKE) -C netflix
endif

clean:
	$(MAKE) -C tvportal clean

ifeq ($(CONFIG_PROD_HMS1000S),y)
	$(MAKE) -C maxdome clean
endif

ifeq ($(CONFIG_PROD_ICORDPRO),y)
	$(MAKE) -C maxdome clean
endif

ifeq ($(CONFIG_PROD_HDNEO),y)
	$(MAKE) -C maxdome clean
endif

ifeq ($(CONFIG_PROD_HGS1000S),y)
	$(MAKE) -C maxdome clean
endif

ifeq ($(CONFIG_DEBUG), y)
	$(MAKE) -C debug clean
endif

ifeq ($(CONFIG_OP_FREESAT),y)
	$(MAKE) -C freesat clean
endif

ifeq ($(CONFIG_OP_MIDDLE_EAST),y)
	$(MAKE) -C hmxdrm clean
endif

ifeq ($(CONFIG_OP_AUS_DTT),y)
	$(MAKE) -C hmxdrm clean
endif

ifeq ($(USE_OPERA_TVSTORE),YES)
	$(MAKE) -C otvs clean
endif

ifeq ($(CONFIG_PROD_HD9000I),y)
	$(MAKE) -C webapp_upgrade clean
endif
