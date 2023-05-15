#
# Makefile for the Opera Build Environment
#
# Wrriten by Jongwon Kim.
# 2011. 03. 05
#

-include config.mk

################################################################
MAKE	:= make
MKDIR	:= mkdir
CP		:= cp
CD		:= cd
RM		:= rm
CHMOD	:= chmod
FIND	:= find
TAR		:= tar
CD		:= cd
SVN		:= svn
MV		:= mv
LN		:= ln -s
################################################################

SILENT		?= @

all: premake port uva corsair jsplugins package

ifeq ($(CONFIG_DEBUG),y)
DEBUGSYMBOLS := YES
export DEBUGSYMBOLS
endif

ifeq ($(CONFIG_OP_FREESAT),y)
FREESATSYMBOLS := YES
export FREESATSYMBOLS
endif

.PHONY: premake
premake:
	@if [ ! -d $(BUILD_SRC_DIR) ]; then	\
	   	$(MKDIR) -p $(BUILD_SRC_DIR);		\
	   	$(MKDIR) -p $(BUILD_SRC_DIR)/$(OPERA_OOIF_PACKAGE);		\
		$(TAR) xvf $(OPERA_OOIF_PACKAGE_PATH) -C $(BUILD_SRC_DIR)/$(OPERA_OOIF_PACKAGE)/ --strip-components 1;	\
		$(MV) $(BUILD_SRC_DIR)/$(OPERA_OOIF_PACKAGE)/* $(BUILD_SRC_DIR)/;	\
		$(MV) $(BUILD_SRC_DIR)/doc $(BUILD_SRC_DIR)/doc_sdk/;	\
		$(RM) -rf $(BUILD_SRC_DIR)/$(OPERA_OOIF_PACKAGE);	\
		$(MKDIR) -p $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE);		\
		$(TAR) xvf $(OPERA_HBBTV_PACKAGE_PATH) -C $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/ --strip-components 1;	\
		$(MV) $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/doc/html/ $(BUILD_SRC_DIR)/doc_hbbtv/;	\
		$(MV) $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/include/* $(BUILD_SRC_DIR)/include/;	\
		$(RM) -rf $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/include;	\
		$(MV) $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib/* $(BUILD_SRC_DIR)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib/;	\
		$(RM) -rf $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/$(PLATFORM);	\
		$(MV) $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE)/* $(BUILD_SRC_DIR)/;	\
		$(RM) -rf $(BUILD_SRC_DIR)/$(OPERA_HBBTV_PACKAGE);	\
		$(CP) -af $(OPERA_HUMAX_IMPL_HOME)/build/*.mk $(BUILD_SRC_DIR)/build/;	\
		$(RM) -rf $(BUILD_SRC_DIR)/components/corsair;	\
		$(LN) ./../../../humax-impl/components/corsair ./src/components/corsair;	\
		$(RM) -rf $(BUILD_SRC_DIR)/jsplugins/hbbtv;	\
		$(RM) -rf $(BUILD_SRC_DIR)/jsplugins/oipf;	\
		$(LN) ./../../../humax-impl/jsplugins/hbbtv ./src/jsplugins/hbbtv;	\
		$(LN) ./../../../humax-impl/jsplugins/oipf ./src/jsplugins/oipf;	\
		$(LN) ./../../../humax-impl/jsplugins/extension ./src/jsplugins/extension;	\
		$(LN) ./../../../../humax-impl/uva/backends/hmp ./src/uva/backends/hmp;  \
		$(RM) -rf ./src/uva/backends/drm;	\
		$(LN) ./../../../../humax-impl/uva/backends/drm ./src/uva/backends/drm;  \
		$(CP) -af $(OPERA_HUMAX_IMPL_HOME)/include $(BUILD_SRC_DIR); \
	fi

.PHONY: port
port: premake
	$(SILENT) $(MAKE) -C ../port/

.PHONY: corsair
corsair: uva
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/build

.PHONY: jsplugins
jsplugins: corsair
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/hbbtv PROFILES=hbbtv.profile

ifeq ($(CONFIG_PRODUCT_NAME),"jcngs")
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.jlabs PROFILES=jlabs.profile
else ifeq ($(CONFIG_PRODUCT_NAME),"ysr2000")
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.humax PROFILES=ystdc.profile
else ifeq ($(CONFIG_PRODUCT_NAME),"ys1000")
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.humax PROFILES=ystdc.profile
else
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.humax PROFILES=humax.profile
endif
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/extension

.PHONY: uva
uva: port
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/uva/backends/hmp/
	$(SILENT) $(CP) -af $(BUILD_SRC_DIR)/uva/backends/hmp/*.so $(BUILD_SRC_DIR)/$(GOGI_SDK_PATH)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib/
	$(SILENT) $(MAKE) -C $(BUILD_SRC_DIR)/uva/backends/drm/
	$(SILENT) $(CP) -af $(BUILD_SRC_DIR)/uva/backends/drm/*.so $(BUILD_SRC_DIR)/$(GOGI_SDK_PATH)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib/drm/

.PHONY: clean
clean:
	@if [ -d $(BUILD_SRC_DIR) ]; then	\
		$(MAKE) -C $(NATIVE_PORT_PATH) clean;	\
		$(MAKE) -C $(BUILD_SRC_DIR)/build/ clean;	\
		$(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/hbbtv clean;	\
		$(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/extension clean;	\
		$(MAKE) -C $(BUILD_SRC_DIR)/uva/backends/hmp clean;	\
		$(MAKE) -C $(BUILD_SRC_DIR)/uva/backends/drm clean;	\
	fi
ifeq ($(CONFIG_PRODUCT_NAME),"jcngs")
	@if [ -d $(BUILD_SRC_DIR) ]; then	\
		$(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.jlabs clean;	\
	fi
else
	@if [ -d $(BUILD_SRC_DIR) ]; then	\
		$(MAKE) -C $(BUILD_SRC_DIR)/jsplugins/oipf -f Makefile.humax clean;	\
	fi
endif

.PHONY: distclean
distclean: clean
	$(RM) -rf $(BUILD_SRC_DIR)
	$(RM) -rf $(BUILD_OUT_DIR)

lib_list	+= $(shell find $(BUILD_SRC_DIR)/$(PLATFORM)/$(GCC_LIBC_STRING)/lib/ -name *.so*)
lib_list	+= $(shell find $(NATIVE_PORT_PATH)/src -name *.so)

opera_copy_list	+= $(OPERA_HUMAX_IMPL_HOME)/$(PLATFORM)/$(GCC_LIBC_STRING)/opera_home
opera_copy_list	+= $(OPERA_HUMAX_IMPL_HOME)/$(PLATFORM)/$(GCC_LIBC_STRING)/opera_dir

product_env_dir += $(OPERA_HUMAX_IMPL_HOME)/$(PRODUCT_ENV)
product_copy_list += $(OPERA_HUMAX_IMPL_HOME)/$(PRODUCT_ENV)/opera_home
product_copy_list += $(OPERA_HUMAX_IMPL_HOME)/$(PRODUCT_ENV)/opera_dir

plugin_list		+= $(shell find $(BUILD_SRC_DIR)/jsplugins/oipf/oipf_out/ -name *.so)
plugin_list		+= $(shell find $(BUILD_SRC_DIR)/jsplugins/hbbtv/hbbtv_out/ -name *.so)
plugin_list		+= $(shell find $(BUILD_SRC_DIR)/jsplugins/extension/ -name *.so)

.PHONY: package
package: jsplugins
	@if [ -d $(BUILD_OUT_DIR) ]; then	\
		$(RM) -rf $(BUILD_OUT_DIR);		\
	fi	\

	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)
	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)/lib
	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)/runtime
	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)/runtime/opera_home
	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)/runtime/opera_dir
	$(SILENT) $(MKDIR) -p $(BUILD_OUT_DIR)/runtime/lib
	$(SILENT) $(CP) -af $(lib_list) $(BUILD_OUT_DIR)/lib/

	@if [ ! -d $(product_env_dir) ]; then	\
		$(CP) -af $(opera_copy_list) $(BUILD_OUT_DIR)/runtime/;	\
	else	\
		$(CP) -af $(product_copy_list) $(BUILD_OUT_DIR)/runtime/;	\
	fi	\

	$(SILENT) $(CP) -af $(plugin_list) $(BUILD_OUT_DIR)/runtime/opera_dir/jsplugins/
	$(SILENT) $(FIND) $(BUILD_OUT_DIR) -name *.so -exec $(CHMOD) 0774 {} \;
	$(SILENT) $(FIND) $(BUILD_OUT_DIR) -name .svn -type d -print | xargs rm -rf

	$(SILENT) objdump -x $(BUILD_SRC_DIR)/build/out/corsair > $(BUILD_SRC_DIR)/build/out/corsair.map

ifneq ($(CONFIG_DEBUG),y)
	$(SILENT) $(CROSS)strip --strip-debug --strip-unneeded $(BUILD_SRC_DIR)/build/out/corsair
	$(SILENT) $(FIND) $(BUILD_OUT_DIR) -name "*.so" | xargs $(CROSS)strip --strip-debug --strip-unneeded
endif
