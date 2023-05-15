###############################################################################
#                                                                             #
#                             Humax Templte Makefile                          #
#                                                                             #
# Author: dklee (Plat1, STB division)                                         #
#                                                                             #
# Copyright (c) Humax co, Ltd., 2007                                          #
#                                                                             #
###############################################################################
# $Header:
# List up sub-directories
###############################################################################
export FACTORYSW_DIR := $(CURDIR)/../..
export FACTORYSW_PRODUCT_CONFIG_DIR := $(addsuffix /make/factorysw_config,$(FACTORYSW_DIR))
export FACTORYSW_MAKE_DIR := $(addsuffix /make/build_rule,$(FACTORYSW_DIR))

include toolset.mak

export FACTORYSW_LIB := $(FACTORYSW_DIR)/$(FACTORYSW_OUTPUT_NAME)


FACTORYSW_DEF_FILE := $(FACTORYSW_PRODUCT_CONFIG_DIR)/factorysw_def.cfg
BOX_FACTORYSW_CFG_FILE := $(FACTORYSW_DIR)/config/$(FACTORYSW_NAME)/factorysw.cfg
BOX_FACTORYSW_PLATFORM_CFG_FILE := $(FACTORYSW_DIR)/config/$(FACTORYSW_NAME)/factorysw_platform.cfg
FACTORYSW_DEF_HEADER := $(addprefix $(FACTORYSW_DIR)/include/, factorysw_def.h)
BOX_FACTORYSW_CFG_HEADER := $(addprefix $(FACTORYSW_DIR)/include/, BOX_factorysw.h)
BOX_FACTORYSW_PLATFORM_CFG_HEADER := $(addprefix $(FACTORYSW_DIR)/include/, BOX_factorysw_platform.h)

SUBDIRS_LIST =	$(FACTORYSW_DIR)/fai \
		$(FACTORYSW_DIR)/fdi \
		$(FACTORYSW_DIR)/fvk

###############################################################################
# Rule for build
###############################################################################
OBJS_LIST =

###############################################################################
# List up all directories of header files whih are need to build
###############################################################################
INCLUDES_LIST :=

###############################################################################
# Do not change	this : This routine is exist in only root folder
###############################################################################

build: $(FACTORYSW_DEF_HEADER) $(BOX_FACTORYSW_CFG_HEADER) $(BOX_FACTORYSW_PLATFORM_CFG_HEADER) init_mklist build_obj
	$(FACTORYSW_MAKE)	-C $(CURDIR) build_lib

clean: clean_cfgh clean_lib

$(FACTORYSW_DEF_HEADER): $(FACTORYSW_DEF_FILE)
	@$(FACTORYSW_MAKE_DIR)/bin/$(FACTORYSW_CFGTOH) -q -y -s -i$< -c$@ -1h -pFACTORYSW_DEF

$(BOX_FACTORYSW_CFG_HEADER): $(BOX_FACTORYSW_CFG_FILE)
	@$(FACTORYSW_MAKE_DIR)/bin/$(FACTORYSW_CFGTOH) -q -y -s -i$< -c$@ -1h -pBOX_FACTORYSW_CFG

$(BOX_FACTORYSW_PLATFORM_CFG_HEADER): $(BOX_FACTORYSW_PLATFORM_CFG_FILE)
	@$(FACTORYSW_MAKE_DIR)/bin/$(FACTORYSW_CFGTOH) -q -y -s -i$< -c$@ -1h -pBOX_FACTORYSW_PLATFORM_CFG

clean_cfgh:
	$(FACTORYSW_RM) $(FACTORYSW_DEF_HEADER)
	$(FACTORYSW_RM) $(BOX_FACTORYSW_CFG_HEADER)
	$(FACTORYSW_RM) $(BOX_FACTORYSW_PLATFORM_CFG_HEADER)

###############################################################################
# Export some macros for build
###############################################################################
include rules.mak
