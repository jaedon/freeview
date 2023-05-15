
.PHONY: freetype-all freetype-source freetype-check-config freetype-config freetype freetype-compile freetype-install
.PHONY: freetype-uninstallfreetype-clean freetype-distclean freetype-mrproper
.PHONY: jpeg-all jpeg-source jpeg-check-config jpeg-config jpeg jpeg-compile jpeg-install jpeg-uninstall jpeg-clean jpeg-distclean jpeg-mrproper
.PHONY: png-all png-source png-check-config png-config png png-compile png-install png-uninstall png-clean png-distclean png-mrproper
.PHONY: zlib-all zlib-source zlib-check-config zlib-config zlib zlib-compile zlib-install zlib-uninstall zlib-clean zlib-distclean zlib-mrproper

############################################################
#
# freetype-all: force freetype to be configured and compiled.
#
freetype-all: freetype-config freetype-compile freetype-install

##
#
# freetype-source: if using AppLibs version of freetype, then this will cause
#                  the freetype source tree to be created.
#
freetype-source: check ${DFB_FREETYPE_UNPACKED_FLAG}

##
#
# freetype-check-config: check to see whether the build environment or build options have changed
#                        since the last build.  If they have, then this will force a clean and
#                        then a rebuild of freetype.
#
freetype-check-config: freetype-source ${DFB_FREETYPE_CONFIGURATION_FLAG}

##
#
# freetype-config: force freetype to be configured.
#
freetype-config: freetype-check-config
	@if [ -e "${DFB_FREETYPE_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_CONFIGURED_FLAG}; \
	fi
	@$(call fn-freetype-configure)

##
#
# freetype-compile: compile freetype
#
freetype-compile: ${DFB_FREETYPE_CONFIGURED_FLAG}
	@if [ -e "${DFB_FREETYPE_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_COMPILED_FLAG}; \
	fi
	@$(call fn-freetype-compile)

##
#
# freetype-install: install freetype
#
freetype-install: check ${DFB_FREETYPE_COMPILED_FLAG}
	@if [ -e "${DFB_FREETYPE_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_INSTALLED_FLAG}; \
	fi
	@$(call fn-freetype-install)

##
#
# freetype: force freetype to be compiled and installed in one pass (tracks changes to source files).
#
freetype: freetype-install

##
#
# freetype-uninstall: uninstall freetype
#
freetype-uninstall:
	@if [ -e "${DFB_FREETYPE_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${DFB_FREETYPE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C $(DFB_FREETYPE_SRC_DIR) uninstall; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libfreetype.a" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libfreetype.*a; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libfreetype.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libfreetype.so*; \
	fi

##
#
# freetype-clean: remove all intermediate object and dependency files as well as installed libs.
#
freetype-clean: freetype-uninstall
	@if [ -e "${DFB_FREETYPE_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_COMPILED_FLAG}; \
	fi
	@if [ -e "${DFB_FREETYPE_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_FLAG}; \
	fi
	@-if [ -e "${DFB_FREETYPE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C $(DFB_FREETYPE_SRC_DIR) clean; \
	fi


##
#
# freetype-distclean: remove all freetype files including the source tree
#                     (only if using AppLibs version) with prompt.
#
freetype-distclean: freetype-clean
	@-$(call fn-freetype-distclean)
ifeq ($(DFB_FREETYPE_APPLIBS),y)
	@if [ -d "${DFB_FREETYPE_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DFB_FREETYPE_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DFB_FREETYPE_SRC_DIR}..."; \
		$(RM) -r ${DFB_FREETYPE_SRC_DIR}; \
	fi
endif

##
#
# freetype-mrproper: remove all freetype files including the source tree
#                     (only if using AppLibs version).
#
freetype-mrproper: freetype-clean
	@-$(call fn-freetype-distclean)
ifeq ($(DFB_FREETYPE_APPLIBS),y)
	@if [ -d "${DFB_FREETYPE_SRC_DIR}" ]; then \
		$(RM) -r ${DFB_FREETYPE_SRC_DIR}; \
	fi
endif

##
#
# fn-freetype-configure: function to configure freetype.
#
define fn-freetype-configure
	$(ECHO) "**************************************"; \
	$(ECHO) "* Configuring Freetype-${DFB_FREETYPE_VERSION} library *"; \
	$(ECHO) "**************************************"; \
	$(CD) $(DFB_FREETYPE_SRC_DIR); \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	[ -e "$(DFB_FREETYPE_SRC_DIR)/config.mk" ] && $(CHMOD) -f +w "$(DFB_FREETYPE_SRC_DIR)/config.mk"; \
	./configure ${DFB_FREETYPE_CONFIG_FLAGS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*********************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure Freetype-${DFB_FREETYPE_VERSION} - Aborting! *"; \
		$(ECHO) "*********************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_FREETYPE_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-freetype-compile: function to compile freetype.
#
define fn-freetype-compile
	$(ECHO) "***********************************"; \
	$(ECHO) "* Building Freetype-${DFB_FREETYPE_VERSION} library *"; \
	$(ECHO) "***********************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) MKDIR="mkdir -p"; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_FREETYPE_SRC_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*****************************************************"; \
	    $(ECHO) "* ERROR: Failed to build Freetype-${DFB_FREETYPE_VERSION} - Aborting! *"; \
		$(ECHO) "*****************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_FREETYPE_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-freetype-install: function to install freetype.
#
define fn-freetype-install
	$(ECHO) "*************************************"; \
	$(ECHO) "* Installing Freetype-${DFB_FREETYPE_VERSION} library *"; \
	$(ECHO) "*************************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) MKDIR="mkdir -p"; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_FREETYPE_SRC_DIR} install; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************************"; \
	    $(ECHO) "* ERROR: Failed to install Freetype-${DFB_FREETYPE_VERSION} - Aborting! *"; \
		$(ECHO) "*******************************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DFB_FREETYPE_VERSION}"  > ${DFB_FREETYPE_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_FREETYPE_SRC_DIR}" >> ${DFB_FREETYPE_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-freetype-distclean: function to cleanout all files
#
define fn-freetype-distclean
	if [ -e "${DFB_FREETYPE_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DFB_FREETYPE_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_FREETYPE_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DFB_FREETYPE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C $(DFB_FREETYPE_SRC_DIR) distclean; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_UNPACKED_FLAG}:
ifeq ($(DFB_FREETYPE_APPLIBS),y)
	@if ! [ -e "${DFB_FREETYPE_SRC_DIR}" ]; then \
	$(CD) ${DFB_FREETYPE_TOP}; ${MKDIR} "${DFB_FREETYPE_SRC_DIR}"; \
	fi
	@$(ECHO) "**********************************"; \
	$(ECHO) "* Creating Freetype-${DFB_FREETYPE_VERSION} Source *"; \
	$(ECHO) "**********************************"; \
	$(CD) ${DFB_FREETYPE_TOP}; \
	$(ECHO) ">>>>>>> Ignore following bogus tar errors <<<<<<<"; \
	$(TAR) xzkf ${DFB_FREETYPE_SOURCE_TARBALL}; \
	$(ECHO) ">>>>>>> Ignore preceeding bogus tar errors <<<<<<<"; \
	$(TOUCH) ${DFB_FREETYPE_UNPACKED_FLAG}
endif

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_CONFIGURATION_FLAG}: ${DFB_FREETYPE_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DFB_FREETYPE_SRC_DIR}/.config.$$$$; \
	if [ -e "${DFB_FREETYPE_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DFB_FREETYPE_CONFIGURATION_FLAG} ${DFB_FREETYPE_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} freetype-clean; \
			$(CP) ${DFB_FREETYPE_SRC_DIR}/.config.$$$$ ${DFB_FREETYPE_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_FREETYPE_SRC_DIR}/.config.$$$$ ${DFB_FREETYPE_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DFB_FREETYPE_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_CONFIGURED_FLAG}: ${DFB_FREETYPE_CONFIGURATION_FLAG}
	@$(call fn-freetype-configure)

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_COMPILED_FLAG}: ${DFB_FREETYPE_CONFIGURED_FLAG}
	@$(call fn-freetype-compile)

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_INSTALLED_FLAG}: ${DFB_FREETYPE_COMPILED_FLAG}
	@$(call fn-freetype-install)

##
#
# Dependency target not to be called directly.
#
${DFB_FREETYPE_FLAG}: ${DFB_FREETYPE_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${DFB_FREETYPE_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${DFB_FREETYPE_PKG_CONFIG} > \
					${DFB_FREETYPE_SRC_DIR}/.install.$$$$; \
	if [ -e "${DFB_FREETYPE_FLAG}" ]; then \
		if ($(DIFF) ${DFB_FREETYPE_FLAG} ${DFB_FREETYPE_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DFB_FREETYPE_SRC_DIR}/.install.$$$$ ${DFB_FREETYPE_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_FREETYPE_SRC_DIR}/.install.$$$$ ${DFB_FREETYPE_FLAG}; \
	fi; \
	$(RM) ${DFB_FREETYPE_SRC_DIR}/.install.$$$$


############################################################
#
# jpeg-all: force jpeg to be configured and compiled.
#
jpeg-all: jpeg-config jpeg-compile jpeg-install

##
#
# jpeg-source:  unpack the source jpeg source code (only for AppLibs version)
#
jpeg-source: check ${DFB_JPEG_UNPACKED_FLAG}

##
#
# jpeg-check-config: check to see whether the build environment or build options have changed
#                    since the last build.  If they have, then this will force a clean and
#                    then a rebuild of jpeg.
#
jpeg-check-config: jpeg-source ${DFB_JPEG_CONFIGURATION_FLAG}

##
#
# jpeg-config: force jpeg to be configured.
#
jpeg-config: jpeg-check-config
	@if [ -e "${DFB_JPEG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_CONFIGURED_FLAG}; \
	fi
	@$(call fn-jpeg-configure)
	@$(call fn-jpeg-libtool)

##
#
# jpeg-compile: force jpeg to be compiled (tracks changes to source code files)
#
jpeg-compile: ${DFB_JPEG_CONFIGURED_FLAG}
	@if [ -e "${DFB_JPEG_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_COMPILED_FLAG}; \
	fi
	@$(call fn-jpeg-compile)

##
#
# jpeg-install: force jpeg to be installed (tracks changes to source code files)
#
jpeg-install: check ${DFB_JPEG_COMPILED_FLAG}
	@if [ -e "${DFB_JPEG_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_INSTALLED_FLAG}; \
	fi
	@$(call fn-jpeg-install)

##
#
# jpeg: force jpeg to be compiled and installed in one pass (tracks changes to source code files)
#
jpeg: jpeg-install

##
#
# jpeg-uninstall: force jpeg to be uninstalled
#
jpeg-uninstall:
	@if [ -e "${DFB_JPEG_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_INSTALLED_FLAG}; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_INCLUDE_DIR}/jconfig.h" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_INCLUDE_DIR}/jconfig.h; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_INCLUDE_DIR}/jpeglib.h" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_INCLUDE_DIR}/jpeglib.h; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_INCLUDE_DIR}/jmorecfg.h" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_INCLUDE_DIR}/jmorecfg.h; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_INCLUDE_DIR}/jerror.h" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_INCLUDE_DIR}/jerror.h; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_LIB_DIR}/libjpeg.a" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_LIB_DIR}/libjpeg.*a; \
	fi
	@if [ -e "${DFB_JPEG_INSTALL_LIB_DIR}/libjpeg.so" ]; then \
		$(RM) ${DFB_JPEG_INSTALL_LIB_DIR}/libjpeg.so*; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libjpeg.a" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libjpeg.*a; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libjpeg.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libjpeg.so*; \
	fi

##
#
# jpeg-clean: remove all intermediate object and dependency files
#             as well as installed libs.
#
jpeg-clean: jpeg-uninstall
	@if [ -e "${DFB_JPEG_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_COMPILED_FLAG}; \
	fi
	@if [ -e "${DFB_JPEG_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_FLAG}; \
	fi
	@-if [ -e "${DFB_JPEG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DFB_JPEG_SRC_DIR} clean; \
	fi

##
#
# jpeg-distclean: remove all jpeg files including source tree
#                 (only if using AppLibs version) with prompt.
#
jpeg-distclean: jpeg-clean
	@-$(call fn-jpeg-distclean)
ifeq ($(DFB_JPEG_APPLIBS),y)
	@if [ -d "${DFB_JPEG_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DFB_JPEG_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DFB_JPEG_SRC_DIR}..."; \
		$(RM) -r ${DFB_JPEG_SRC_DIR}; \
	fi
endif

##
#
# jpeg-mrproper: remove all jpeg files including source tree
#                 (only if using AppLibs version)
#
jpeg-mrproper: jpeg-clean
	@-$(call fn-jpeg-distclean)
ifeq ($(DFB_JPEG_APPLIBS),y)
	@if [ -d "${DFB_JPEG_SRC_DIR}" ]; then \
		$(RM) -r ${DFB_JPEG_SRC_DIR}; \
	fi
endif

##
#
# fn-jpeg-configure: function to configure jpeg.
#
define fn-jpeg-configure
	$(ECHO) "*******************************"; \
	$(ECHO) "* Configuring JPEG-${DFB_JPEG_VERSION} library *"; \
	$(ECHO) "*******************************"; \
	$(CD) $(DFB_JPEG_SRC_DIR); \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	./configure ${DFB_JPEG_CONFIG_FLAGS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***********************************************"; \
	    $(ECHO) "* ERROR: Failed to configure jpeg - Aborting! *"; \
		$(ECHO) "***********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_JPEG_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-jpeg-libtool: function to create "libtool" for cross compiling.  This is only
#               required if building jpeg as a shared library.
#
ifeq ($(DIRECTFB_SHARED),y)

define fn-jpeg-libtool
	@$(CD) $(DFB_JPEG_SRC_DIR); \
	./ltconfig ltmain.sh $(B_REFSW_TOOLCHAIN_ARCH)
endef

else

define fn-jpeg-libtool
endef

endif

##
#
# fn-jpeg-compile: function to compile jpeg.
#
define fn-jpeg-compile
	$(ECHO) "****************************"; \
	$(ECHO) "* Building JPEG-${DFB_JPEG_VERSION} library *"; \
	$(ECHO) "****************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C $(DFB_JPEG_SRC_DIR); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************"; \
	    $(ECHO) "* ERROR: Failed to build jpeg - Aborting! *"; \
		$(ECHO) "*******************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_JPEG_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-jpeg-install: function to install jpeg.
#
# 	${MAKE} ${MAKE_OPTIONS} -C $(DFB_JPEG_SRC_DIR) install-lib; \

define fn-jpeg-install
	$(ECHO) "******************************"; \
	$(ECHO) "* Installing JPEG-${DFB_JPEG_VERSION} library *"; \
	$(ECHO) "******************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	if [ ${DFB_JPEG_VERSION} == "6b" ]; then \
		${MAKE} ${MAKE_OPTIONS} -C $(DFB_JPEG_SRC_DIR) install-lib; \
	else \
		${MAKE} ${MAKE_OPTIONS} -C $(DFB_JPEG_SRC_DIR) install; \
	fi
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*********************************************"; \
	    $(ECHO) "* ERROR: Failed to install jpeg - Aborting! *"; \
		$(ECHO) "*********************************************"; \
	    $(FALSE); \
	else \
		if [ -e "$(DFB_JPEG_SRC_DIR)/libjpeg.a" ]; then \
			$(CP) $(DFB_JPEG_SRC_DIR)/libjpeg.a ${DFB_JPEG_INSTALL_LIB_DIR}; \
		fi; \
		$(ECHO) "Ver: ${DFB_JPEG_VERSION}"  > ${DFB_JPEG_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_JPEG_SRC_DIR}" >> ${DFB_JPEG_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-jpeg-distclean: function to clean out all files.
#
define fn-jpeg-distclean
	if [ -e "${DFB_JPEG_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DFB_JPEG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_JPEG_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DFB_JPEG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DFB_JPEG_SRC_DIR} distclean; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_UNPACKED_FLAG}:
ifeq ($(DFB_JPEG_APPLIBS),y)
	@if ! [ -e "${DFB_JPEG_SRC_DIR}" ]; then \
	$(CD) ${DFB_JPEG_TOP}; ${MKDIR} "${DFB_JPEG_SRC_DIR}"; \
	fi
	@$(ECHO) "***************************"; \
	$(ECHO)  "* Creating JPEG-${DFB_JPEG_VERSION} Source *"; \
	$(ECHO) "***************************"; \
	$(CD) ${DFB_JPEG_TOP}; \
	$(ECHO) ">>>>>>> Ignore following bogus tar errors <<<<<<<"; \
	$(TAR) xzkf ${DFB_JPEG_SOURCE_TARBALL}; \
	$(ECHO) ">>>>>>> Ignore preceeding bogus tar errors <<<<<<<"; \
	$(TOUCH) ${DFB_JPEG_UNPACKED_FLAG}
endif

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_CONFIGURATION_FLAG}: ${DFB_JPEG_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion      >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine      >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DFB_JPEG_SRC_DIR}/.config.$$$$; \
	if [ -e "${DFB_JPEG_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DFB_JPEG_CONFIGURATION_FLAG} ${DFB_JPEG_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_DIR} jpeg-clean; \
			$(CP) ${DFB_JPEG_SRC_DIR}/.config.$$$$ ${DFB_JPEG_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_JPEG_SRC_DIR}/.config.$$$$ ${DFB_JPEG_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DFB_JPEG_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_CONFIGURED_FLAG}: ${DFB_JPEG_CONFIGURATION_FLAG}
	@$(call fn-jpeg-configure)
	@$(call fn-jpeg-libtool)

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_COMPILED_FLAG}: ${DFB_JPEG_CONFIGURED_FLAG}
	@$(call fn-jpeg-compile)

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_INSTALLED_FLAG}: ${DFB_JPEG_COMPILED_FLAG}
	@$(call fn-jpeg-install)

##
#
# Dependency target not to be called directly.
#
${DFB_JPEG_FLAG}: ${DFB_JPEG_INSTALLED_FLAG}
	@$(ECHO) -e "${DFB_JPEG_VERSION}\n${BRCM_CPPFLAGS}" > ${DFB_JPEG_SRC_DIR}/.install.$$$$; \
	if [ -e "${DFB_JPEG_FLAG}" ]; then \
		if ($(DIFF) ${DFB_JPEG_FLAG} ${DFB_JPEG_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DFB_JPEG_SRC_DIR}/.install.$$$$ ${DFB_JPEG_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_JPEG_SRC_DIR}/.install.$$$$ ${DFB_JPEG_FLAG}; \
	fi; \
	$(RM) ${DFB_JPEG_SRC_DIR}/.install.$$$$


############################################################
#
# png-all: force a configure and compile of png.
#
png-all: png-config png-compile png-install

##
#
# png-source: unpack the png source code (only for AppLibs version)
#
png-source: check ${DFB_PNG_UNPACKED_FLAG}

##
#
# png-check-config: check to see whether the build environment or build options have changed
#                   since the last build.  If they have, then this will force a clean and
#                   then a rebuild of png.
#
png-check-config: png-source ${DFB_PNG_CONFIGURATION_FLAG}

##
#
# png-config: force png to be configured.
#
png-config: png-check-config ${DFB_ZLIB_FLAG}
	@if [ -e "${DFB_PNG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_CONFIGURED_FLAG}; \
	fi
	@$(call fn-png-configure)

##
#
# png-compile: force png to be compiled (tracks changes to source code files).
#
png-compile: ${DFB_PNG_CONFIGURED_FLAG}
	@if [ -e "${DFB_PNG_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_COMPILED_FLAG}; \
	fi
	@$(call fn-png-compile)

##
#
# png-install: force png to be installed (tracks changes to source code files).
#
png-install: check ${DFB_PNG_COMPILED_FLAG}
	@if [ -e "${DFB_PNG_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_INSTALLED_FLAG}; \
	fi
	@$(call fn-png-install)

##
#
# png: force png to be compiled and installed in one pass (tracks changes to source code files).
#
png: png-install

##
#
# png-uninstall: force png to be uninstalled
#
png-uninstall:
	@if [ -e "${DFB_PNG_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_INSTALLED_FLAG}; \
	fi
	@if [ -d "${DFB_PNG_INSTALL_INCLUDE_DIR}/libpng" ]; then \
		$(RM) -r ${DFB_PNG_INSTALL_INCLUDE_DIR}/libpng; \
	fi
	@if [ -h "${DFB_PNG_INSTALL_INCLUDE_DIR}/png.h" ]; then \
		$(RM) ${DFB_PNG_INSTALL_INCLUDE_DIR}/png.h; \
	fi
	@if [ -h "${DFB_PNG_INSTALL_INCLUDE_DIR}/pngconf.h" ]; then \
		$(RM) ${DFB_PNG_INSTALL_INCLUDE_DIR}/pngconf.h; \
	fi
	@if [ -e "${DFB_PNG_INSTALL_LIB_DIR}/libpng.a" -o -e "${DFB_PNG_INSTALL_LIB_DIR}/libpng12.a" ]; then \
		$(RM) ${DFB_PNG_INSTALL_LIB_DIR}/libpng*.*a; \
	fi
	@if [ -e "${DFB_PNG_INSTALL_LIB_DIR}/libpng.so" -o -e "${DFB_PNG_INSTALL_LIB_DIR}/libpng12.so" ]; then \
		$(RM) ${DFB_PNG_INSTALL_LIB_DIR}/libpng*.so*; \
	fi
	@if [ -e "${DFB_PNG_INSTALL_PKG_CONFIG_DIR}/libpng.pc" -o -e "${DFB_PNG_INSTALL_PKG_CONFIG_DIR}/${DFB_PNG_PKG_CONFIG}.pc" ]; then \
		$(RM) ${DFB_PNG_INSTALL_PKG_CONFIG_DIR}/libpng*.pc; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libpng.a" -o -e "${DIRECTFB_INSTALL_LIB_DIR}/libpng12.a" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libpng*.*a; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libpng.so" -o -e "${DIRECTFB_INSTALL_LIB_DIR}/libpng12.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libpng*.so*; \
	fi

##
#
# png-clean: remove all intermediate object and dependency files
#            including installed libs.
#
png-clean: png-uninstall
	@if [ -e "${DFB_PNG_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_COMPILED_FLAG}; \
	fi
	@if [ -e "${DFB_PNG_FLAG}" ]; then \
		$(RM) ${DFB_PNG_FLAG}; \
	fi
	@-if [ -e "${DFB_PNG_SRC_DIR}/Makefile" -o -e "${DFB_PNG_SRC_DIR}/makefile" ]; then \
		${MAKE} -C ${DFB_PNG_SRC_DIR} clean; \
	fi
ifeq ($(APPLIBS_COMMON_INC),y)
	@-if [ -e "${DFB_PNG_TOP}/Makefile" ]; then \
		${MAKE} -C ${DFB_PNG_TOP} clean; \
	fi
endif

##
#
# png-distclean: remove all png files including source (only if AppLibs ver)
#                with prompt.
#
png-distclean: png-clean
	@-$(call fn-png-distclean)
ifeq ($(DFB_PNG_APPLIBS),y)
	@if [ -d "${DFB_PNG_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DFB_PNG_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DFB_PNG_SRC_DIR}..."; \
		$(RM) -r ${DFB_PNG_SRC_DIR}; \
	fi
endif

##
#
# png-mrproper: remove all png files including source (only if AppLibs ver)
#
png-mrproper: png-clean
	@-$(call fn-png-distclean)
ifeq ($(DFB_PNG_APPLIBS),y)
	@if [ -d "${DFB_PNG_SRC_DIR}" ]; then \
		$(RM) -r ${DFB_PNG_SRC_DIR}; \
	fi
endif

##
#
# fn-png-configure: function to configure png.
#
ifeq ($(DFB_PNG_APPLIBS),y)

define fn-png-configure
	$(ECHO) "********************************"; \
	$(ECHO) "* Configuring PNG-${DFB_PNG_VERSION} library"; \
	$(ECHO) "********************************"; \
	$(CD) ${DFB_PNG_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) LDFLAGS="${LDFLAGS} -L${DFB_ZLIB_INSTALL_LIB_DIR}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	./configure ${DFB_PNG_CONFIG_FLAGS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n**********************************************"; \
	    $(ECHO) "* ERROR: Failed to configure png - Aborting! *"; \
		$(ECHO) "**********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_PNG_CONFIGURED_FLAG}; \
	fi
endef

else

ifeq ($(DIRECTFB_SHARED),y)

define fn-png-configure
	$(ECHO) "********************************"; \
	$(ECHO) "* Configuring PNG-${DFB_PNG_VERSION} library"; \
	$(ECHO) "********************************"; \
	$(CD) ${DFB_PNG_SRC_DIR}; \
	$(CAT) makefile | $(SED) -e "/^install/s/$$/ libpng.so.${DFB_PNG_VERSION}/" > Makefile; \
	$(SED) -i -e "s/cp libpng.a/cp -a libpng.so*/" Makefile; \
	$(SED) -i -e "/chmod 644.*libpng.a$$/d" Makefile; \
	$(ECHO) -e "\nLDFLAGS = -L. -L${DFB_ZLIB_INSTALL_LIB_DIR} -lz -lm -shared -Wl,-soname,libpng.so.1" >> Makefile; \
	$(ECHO) -e "\nlibpng.so.${DFB_PNG_VERSION}: $$""(OBJS)" >> Makefile; \
	$(ECHO) -e "\t@$$""{CC} $$""{LDFLAGS} $$""(OBJS) -o $$""@; \\" >> Makefile; \
	$(ECHO) -e "\trm -f libpng.so.1 libpng.so; \\" >> Makefile; \
	$(ECHO) -e "\tln -s $$""@ libpng.so.1; \\" >> Makefile; \
	$(ECHO) -e "\tln -s $$""@ libpng.so" >> Makefile; \
	$(TOUCH) ${DFB_PNG_CONFIGURED_FLAG}
endef

else

define fn-png-configure
	$(ECHO) "********************************"; \
	$(ECHO) "* Configuring PNG-${DFB_PNG_VERSION} library"; \
	$(ECHO) "********************************"; \
	$(CD) ${DFB_PNG_SRC_DIR}; \
	$(INSTALL) -m 664 makefile Makefile; \
	$(TOUCH) ${DFB_PNG_CONFIGURED_FLAG}
endef

endif
endif

##
#
# fn-png-compile: function to compile the png library.
#
define fn-png-compile
	$(ECHO) "*****************************"; \
	$(ECHO) "* Building PNG-${DFB_PNG_VERSION} library"; \
	$(ECHO) "*****************************"; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_PNG_SRC_DIR} -f Makefile ${DFB_PNG_INSTALL_OPTIONS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n******************************************"; \
	    $(ECHO) "* ERROR: Failed to build png - Aborting! *"; \
		$(ECHO) "******************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_PNG_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-png-install: function to install the png library.
#
define fn-png-install
	$(ECHO) "*******************************"; \
	$(ECHO) "* Installing PNG-${DFB_PNG_VERSION} library"; \
	$(ECHO) "*******************************"; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_PNG_SRC_DIR} -f Makefile install ${DFB_PNG_INSTALL_OPTIONS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n********************************************"; \
	    $(ECHO) "* ERROR: Failed to install png - Aborting! *"; \
		$(ECHO) "********************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DFB_PNG_VERSION}"  > ${DFB_PNG_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_PNG_SRC_DIR}" >> ${DFB_PNG_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-png-distclean: function to clean out all files.
#
define fn-png-distclean
	if [ -e "${DFB_PNG_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DFB_PNG_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DFB_PNG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_PNG_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DFB_PNG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DFB_PNG_SRC_DIR} distclean; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_UNPACKED_FLAG}:
ifneq ($(APPLIBS_COMMON_INC),y)
ifeq ($(DFB_PNG_APPLIBS),y)
	@if ! [ -e "${DFB_PNG_SRC_DIR}" ]; then \
	$(CD) ${DFB_PNG_TOP}; ${MKDIR} "${DFB_PNG_SRC_DIR}"; \
	fi
	@$(ECHO) "*****************************"; \
	$(ECHO)  "* Creating PNG-${DFB_PNG_VERSION} Source"; \
	$(ECHO) "*****************************"; \
	$(CD) ${DFB_PNG_TOP}; \
	$(ECHO) ">>>>>>> Ignore following bogus tar errors <<<<<<<"; \
	$(TAR) xzkf ${DFB_PNG_SOURCE_TARBALL}; \
	$(ECHO) ">>>>>>> Ignore preceeding bogus tar errors <<<<<<<"; \
	$(TOUCH) ${DFB_PNG_UNPACKED_FLAG}
endif
endif

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_CONFIGURATION_FLAG}: ${DFB_PNG_UNPACKED_FLAG} ALWAYS
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(ECHO) -n "GCC_VERSION="                        > ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion      >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine      >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DFB_PNG_SRC_DIR}/.config.$$$$; \
	if [ -e "${DFB_PNG_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DFB_PNG_CONFIGURATION_FLAG} ${DFB_PNG_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_DIR} png-clean; \
			$(CP) ${DFB_PNG_SRC_DIR}/.config.$$$$ ${DFB_PNG_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_PNG_SRC_DIR}/.config.$$$$ ${DFB_PNG_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DFB_PNG_SRC_DIR}/.config.$$$$
endif

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_CONFIGURED_FLAG}: ${DFB_ZLIB_FLAG} ${DFB_PNG_CONFIGURATION_FLAG}
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(call fn-png-configure)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_COMPILED_FLAG}: ${DFB_PNG_CONFIGURED_FLAG}
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(call fn-png-compile)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_INSTALLED_FLAG}: ${DFB_PNG_COMPILED_FLAG}
ifeq ($(APPLIBS_COMMON_INC),y)
	@if [ -e "${DFB_PNG_TOP}/Makefile" ]; then \
		${MAKE} -C ${DFB_PNG_TOP} all; \
	fi; \
	$(CP) -r ${DFB_PNG_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/include ${DFB_PNG_INSTALL_DIR}; \
	$(CP) -r ${DFB_PNG_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/lib ${DFB_PNG_INSTALL_DIR}; \
	$(CP) -r ${DFB_PNG_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/share ${DFB_PNG_INSTALL_DIR};
else
	@$(call fn-png-install)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_PNG_FLAG}: ${DFB_PNG_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${DFB_PNG_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${DFB_PNG_PKG_CONFIG} > ${DFB_PNG_SRC_DIR}/.install.$$$$; \
	if [ -e "${DFB_PNG_FLAG}" ]; then \
		if ($(DIFF) ${DFB_PNG_FLAG} ${DFB_PNG_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DFB_PNG_SRC_DIR}/.install.$$$$ ${DFB_PNG_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_PNG_SRC_DIR}/.install.$$$$ ${DFB_PNG_FLAG}; \
	fi; \
	$(RM) ${DFB_PNG_SRC_DIR}/.install.$$$$


############################################################
#
# zlib-all: force zlib to be configured and compiled.
#
zlib-all: zlib-config zlib-compile zlib-install

##
#
# zlib-source: unpack the zlib source code (only for AppLibs version)
#
zlib-source: check ${DFB_ZLIB_UNPACKED_FLAG}

##
#
# zlib-check-config: check to see whether the build environment or build options have changed
#                    since the last build.  If they have, then this will force a clean and
#                    then a rebuild of zlib.
#
zlib-check-config: zlib-source ${DFB_ZLIB_CONFIGURATION_FLAG}

##
#
# zlib-config: force zlib to be configured.
#
zlib-config: zlib-check-config
	@if [ -e "${DFB_ZLIB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_CONFIGURED_FLAG}; \
	fi
	@$(call fn-zlib-configure)

##
#
# fn-zlib-compile: force zlib to be compiled (tracks changes to source code files)
#
zlib-compile: ${DFB_ZLIB_CONFIGURED_FLAG}
	@if [ -e "${DFB_ZLIB_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_COMPILED_FLAG}; \
	fi
	@$(call fn-zlib-compile)

##
#
# zlib-install: force zlib to be installed (tracks changes to source code files)
#
zlib-install: check ${DFB_ZLIB_COMPILED_FLAG}
	@if [ -e "${DFB_ZLIB_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_INSTALLED_FLAG}; \
	fi
	@$(call fn-zlib-install)

##
#
# zlib: force zlib to be compiled and installed in one pass (tracks changes to source code files)
#
zlib: zlib-install

##
#
# zlib-uninstall: force zlib to be uninstalled
#
zlib-uninstall:
	@if [ -e "${DFB_ZLIB_INSTALLED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_INSTALLED_FLAG}; \
	fi
	@if [ -e "${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zlib.h" ]; then \
		$(RM) ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zlib.h; \
	fi
	@if [ -e "${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zconf.h" ]; then \
		$(RM) ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zconf.h; \
	fi
	@if [ -e "${DFB_ZLIB_INSTALL_LIB_DIR}/libz.a" ]; then \
		$(RM) ${DFB_ZLIB_INSTALL_LIB_DIR}/libz.*a; \
	fi
	@if [ -e "${DFB_ZLIB_INSTALL_LIB_DIR}/libz.so" ]; then \
		$(RM) ${DFB_ZLIB_INSTALL_LIB_DIR}/libz.so*; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libz.a" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libz.*a; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libz.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libz.so*; \
	fi
	@$(ECHO) "${DIRECTFB_INSTALL_LIB_DIR}"

##
#
# zlib-clean: remove all intermediate object and dependency files
#             including installed libs.
#
zlib-clean: zlib-uninstall
	@if [ -e "${DFB_ZLIB_COMPILED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_COMPILED_FLAG}; \
	fi
	@if [ -e "${DFB_ZLIB_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_FLAG}; \
	fi
	@-if [ -e "${DFB_ZLIB_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DFB_ZLIB_SRC_DIR} clean; \
	fi
ifeq ($(APPLIBS_COMMON_INC),y)
	@-if [ -e "${DFB_ZLIB_TOP}/Makefile" ]; then \
		${MAKE} -C ${DFB_ZLIB_TOP} clean; \
	fi
endif

##
#
# zlib-distclean: remove all zlib files including source (AppLibs only)
#                 with user prompt.
#
zlib-distclean: zlib-clean
	@-$(call fn-zlib-distclean)
ifeq ($(DFB_ZLIB_APPLIBS),y)
	@if [ -d "${DFB_ZLIB_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DFB_ZLIB_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DFB_ZLIB_SRC_DIR}..."; \
		$(RM) -r ${DFB_ZLIB_SRC_DIR}; \
	fi
endif

##
#
# zlib-mrproper: remove all zlib files including source (AppLibs only)
#
zlib-mrproper: zlib-clean
	@-$(call fn-zlib-distclean)
ifeq ($(DFB_ZLIB_APPLIBS),y)
	@if [ -d "${DFB_ZLIB_SRC_DIR}" ]; then \
		$(RM) -r ${DFB_ZLIB_SRC_DIR}; \
	fi
endif

##
#
# fn-zlib-configure: function to configure zlib.
#
define fn-zlib-configure
	$(ECHO) "**********************************"; \
	$(ECHO) "* Configuring ZLIB-${DFB_ZLIB_VERSION} library *"; \
	$(ECHO) "**********************************"; \
	$(CD) $(DFB_ZLIB_SRC_DIR); \
	$(EXPORT) CFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR="${AR} cr"; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(CHMOD) -f u+w Makefile; \
	${DFB_ZLIB_CONFIG_OPTIONS} /bin/sh configure ${DFB_ZLIB_CONFIG_FLAGS}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***********************************************"; \
	    $(ECHO) "* ERROR: Failed to configure zlib - Aborting! *"; \
		$(ECHO) "***********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_ZLIB_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-zlib-compile: function to compile zlib.
#
define fn-zlib-compile
	$(ECHO) "*******************************"; \
	$(ECHO) "* Building ZLIB-${DFB_ZLIB_VERSION} library *"; \
	$(ECHO) "*******************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR="${AR} cr"; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_ZLIB_SRC_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************"; \
	    $(ECHO) "* ERROR: Failed to build zlib - Aborting! *"; \
		$(ECHO) "*******************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DFB_ZLIB_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-zlib-install: function to install header files and libs
#
ifneq ($(DFB_ZLIB_APPLIBS),y)
ifeq ($(DIRECTFB_SHARED),y)

define fn-zlib-install
	$(ECHO) "*********************************"; \
	$(ECHO) "* Installing ZLIB-${DFB_ZLIB_VERSION} library *"; \
	$(ECHO) "*********************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR="${AR} cr"; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_ZLIB_SRC_DIR} install; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************"; \
	    $(ECHO) "* ERROR: Failed to install zlib - Aborting! *"; \
		$(ECHO) "*******************************************"; \
	    $(FALSE); \
	else \
		${MAKE} -C ${DFB_ZLIB_SRC_DIR} example; \
		$(CP) -a ${DFB_ZLIB_SRC_DIR}/libz.so* ${DFB_ZLIB_INSTALL_LIB_DIR}; \
		$(CP) ${DFB_ZLIB_SRC_DIR}/zlib.h ${DFB_ZLIB_SRC_DIR}/zconf.h ${DFB_ZLIB_INSTALL_INCLUDE_DIR}; \
		$(CHMOD) -f 644 ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zlib.h ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zconf.h; \
		$(ECHO) "Ver: ${DFB_ZLIB_VERSION}"  > ${DFB_ZLIB_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_ZLIB_SRC_DIR}" >> ${DFB_ZLIB_INSTALLED_FLAG}; \
	fi
endef

else

define fn-zlib-install
	$(ECHO) "*********************************"; \
	$(ECHO) "* Installing ZLIB-${DFB_ZLIB_VERSION} library *"; \
	$(ECHO) "*********************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR="${AR} cr"; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_ZLIB_SRC_DIR} install; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************"; \
	    $(ECHO) "* ERROR: Failed to install zlib - Aborting! *"; \
		$(ECHO) "*******************************************"; \
	    $(FALSE); \
	else \
		$(CP) $(DFB_ZLIB_SRC_DIR)/libz.a ${DFB_ZLIB_INSTALL_LIB_DIR}; \
		$(CP) ${DFB_ZLIB_SRC_DIR}/zlib.h ${DFB_ZLIB_SRC_DIR}/zconf.h ${DFB_ZLIB_INSTALL_INCLUDE_DIR}; \
		$(CHMOD) -f 644 ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zlib.h ${DFB_ZLIB_INSTALL_INCLUDE_DIR}/zconf.h; \
		$(ECHO) "Ver: ${DFB_ZLIB_VERSION}"  > ${DFB_ZLIB_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_ZLIB_SRC_DIR}" >> ${DFB_ZLIB_INSTALLED_FLAG}; \
	fi
endef

endif

else

define fn-zlib-install
	$(ECHO) "*********************************"; \
	$(ECHO) "* Installing ZLIB-${DFB_ZLIB_VERSION} library *"; \
	$(ECHO) "*********************************"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR="${AR} cr"; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	${MAKE} ${MAKE_OPTIONS} -C ${DFB_ZLIB_SRC_DIR} install; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*******************************************"; \
	    $(ECHO) "* ERROR: Failed to install zlib - Aborting! *"; \
		$(ECHO) "*******************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DFB_ZLIB_VERSION}"  > ${DFB_ZLIB_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DFB_ZLIB_SRC_DIR}" >> ${DFB_ZLIB_INSTALLED_FLAG}; \
	fi
endef

endif

##
#
# fn-zlib-distclean: function to clean out all files.
#
define fn-zlib-distclean
	if [ -e "${DFB_ZLIB_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DFB_ZLIB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DFB_ZLIB_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DFB_ZLIB_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DFB_ZLIB_SRC_DIR} distclean; \
		$(CP) -p ${DFB_ZLIB_SRC_DIR}/Makefile.in ${DFB_ZLIB_SRC_DIR}/Makefile; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_UNPACKED_FLAG}:
ifneq ($(APPLIBS_COMMON_INC),y)
ifeq ($(DFB_ZLIB_APPLIBS),y)
	@if ! [ -e "${DFB_ZLIB_SRC_DIR}" ]; then \
	$(CD) ${DFB_ZLIB_TOP}; ${MKDIR} "${DFB_ZLIB_SRC_DIR}"; \
	fi
	@$(ECHO) "******************************"; \
	$(ECHO) "* Creating ZLIB-${DFB_ZLIB_VERSION} Source *"; \
	$(ECHO) "******************************"; \
	$(CD) ${DFB_ZLIB_TOP}; \
	$(ECHO) ">>>>>>> Ignore following bogus tar errors <<<<<<<"; \
	$(TAR) xzkf ${DFB_ZLIB_SOURCE_TARBALL}; \
	$(ECHO) ">>>>>>> Ignore preceeding bogus tar errors <<<<<<<"; \
	$(TOUCH) ${DFB_ZLIB_UNPACKED_FLAG}
endif
endif

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_CONFIGURATION_FLAG}: ${DFB_ZLIB_UNPACKED_FLAG} ALWAYS
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(ECHO) -n "GCC_VERSION="                        > ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion      >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine      >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DFB_ZLIB_SRC_DIR}/.config.$$$$; \
	if [ -e "${DFB_ZLIB_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DFB_ZLIB_CONFIGURATION_FLAG} ${DFB_ZLIB_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_DIR} zlib-clean; \
			$(CP) ${DFB_ZLIB_SRC_DIR}/.config.$$$$ ${DFB_ZLIB_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_ZLIB_SRC_DIR}/.config.$$$$ ${DFB_ZLIB_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DFB_ZLIB_SRC_DIR}/.config.$$$$
endif

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_CONFIGURED_FLAG}: ${DFB_ZLIB_CONFIGURATION_FLAG}
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(call fn-zlib-configure)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_COMPILED_FLAG}: ${DFB_ZLIB_CONFIGURED_FLAG}
ifneq ($(APPLIBS_COMMON_INC),y)
	@$(call fn-zlib-compile)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_INSTALLED_FLAG}: ${DFB_ZLIB_COMPILED_FLAG}
ifeq ($(APPLIBS_COMMON_INC),y)
	@if [ -e "${DFB_ZLIB_TOP}/Makefile" ]; then \
		${MAKE} -C ${DFB_ZLIB_TOP} all; \
	fi; \
	$(CP) -r ${DFB_ZLIB_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/include ${DFB_ZLIB_INSTALL_DIR}; \
	$(CP) -r ${DFB_ZLIB_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/lib ${DFB_ZLIB_INSTALL_DIR}; \
	$(CP) -r ${DFB_ZLIB_SRC_DIR}/${PLATFORM}${BCHP_VER_LOWER}/share ${DFB_ZLIB_INSTALL_DIR};
else
	@$(call fn-zlib-install)
endif

##
#
# Dependency target not to be called directly.
#
${DFB_ZLIB_FLAG}: ${DFB_ZLIB_INSTALLED_FLAG}
	@$(ECHO) -e "${DFB_ZLIB_VERSION}\n${BRCM_CPPFLAGS}" > ${DFB_ZLIB_SRC_DIR}/.install.$$$$; \
	if [ -e "${DFB_ZLIB_FLAG}" ]; then \
		if ($(DIFF) ${DFB_ZLIB_FLAG} ${DFB_ZLIB_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DFB_ZLIB_SRC_DIR}/.install.$$$$ ${DFB_ZLIB_FLAG}; \
		fi; \
	else \
		$(CP) ${DFB_ZLIB_SRC_DIR}/.install.$$$$ ${DFB_ZLIB_FLAG}; \
	fi; \
	$(RM) ${DFB_ZLIB_SRC_DIR}/.install.$$$$


