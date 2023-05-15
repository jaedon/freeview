
.PHONY: ppdfb-all ppdfb-source ppdfb-check-config ppdfb-config ppdfb ppdfb-compile ppdfb-install ppdfb-uninstall ppdfb-clean
.PHONY: ppdfb-distclean ppdfb-mrproper

############################################################
#
# ppdfb-all: force ppDFB to be configured and compiled.
#
ppdfb-all: ppdfb-config ppdfb-compile ppdfb-install

##
#
# ppdfb-source: unpack the ppDFB source code.
#
ppdfb-source: check ${PPDFB_UNPACKED_FLAG}

##
#
# ppdfb-check-config: check to see whether the build environment or build options have changed
#                     since the last build.  If they have, then this will force a clean and
#                     then a rebuild of ppDFB.
#
ppdfb-check-config: ppdfb-source ${PPDFB_CONFIGURATION_FLAG}

##
#
# ppdfb-autogen: force configure and autoconf .in files to be regenerated.
#
ppdfb-autogen: check-autogen-tools ppdfb-check-config
	@$(ECHO) "********************************************"; \
	$(ECHO)  "* Autogenerating ++DFB-${PPDFB_VERSION} Configuration *"; \
	$(ECHO)  "********************************************"; \
	if [ -e "${PPDFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${PPDFB_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${PPDFB_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR}"; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) PKG_CONFIG_LIBDIR=${DIRECTFB_INSTALL_LIB_DIR}; \
	$(EXPORT) PKG_CONFIG_PATH=${DIRECTFB_INSTALL_PKG_CONFIG_DIR}; \
	autoreconf; \
	./configure $(PPDFB_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate ++DFB - Aborting! *"; \
		$(ECHO) "***************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${includedir};${PPDFB_INSTALL_INCLUDE_DIR};g;\
		           /Libs/s;\$${libdir};${PPDFB_INSTALL_LIB_DIR};g" ${PPDFB_SRC_DIR}/${PPDFB_PKG_CONFIG}.pc; \
		$(TOUCH) ${PPDFB_CONFIGURED_FLAG}; \
	fi

##
#
# ppdfb-config: force ppDFB to be configured.
#
ppdfb-config: ppdfb-check-config
	@if [ -e "${PPDFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${PPDFB_CONFIGURED_FLAG}; \
	fi
	@$(call fn-ppdfb-configure)

##
#
# ppdfb-compile: force ppDFB to be compiled (tracks changes to source code files)
#
ppdfb-compile: ${PPDFB_CONFIGURED_FLAG}
	@if [ -e "${PPDFB_COMPILED_FLAG}" ]; then \
		$(RM) ${PPDFB_COMPILED_FLAG}; \
	fi
	@$(call fn-ppdfb-compile)

##
#
# ppdfb-install: force ppDFB to be installed (tracks changes to source code files)
#
ppdfb-install: ${PPDFB_COMPILED_FLAG}
	@if [ -e "${PPDFB_INSTALLED_FLAG}" ]; then \
		$(RM) ${PPDFB_INSTALLED_FLAG}; \
	fi
	@$(call fn-ppdfb-install)

##
#
# ppdfb: force ppDFB to be compiled and installed in one pass (tracks changes to source code files)
#
ppdfb: ppdfb-install

##
#
# ppdfb-uninstall: force ppDFB to be uninstalled
#
ppdfb-uninstall:
	@if [ -e "${PPDFB_INSTALLED_FLAG}" ]; then \
		$(RM) ${PPDFB_INSTALLED_FLAG}; \
	fi
	@if [ -e "${PPDFB_INSTALL_BIN_PPD_VER_DIR}/runppd.sh" ]; then \
		$(RM) ${PPDFB_INSTALL_BIN_PPD_VER_DIR}/runppd.sh; \
	fi
	@-if [ -e "${PPDFB_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${PPDFB_SRC_DIR} uninstall DESTDIR=${PPDFB_INSTALL_DIR}; \
	fi

##
#
# ppdfb-clean: remove all intermediate object and dependency files.
#
ppdfb-clean: ppdfb-uninstall
	@if [ -e "${PPDFB_COMPILED_FLAG}" ]; then \
		$(RM) ${PPDFB_COMPILED_FLAG}; \
	fi
	@if [ -e "${PPDFB_FLAG}" ]; then \
		$(RM) ${PPDFB_FLAG}; \
	fi
	@-if [ -e "${PPDFB_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${PPDFB_SRC_DIR} clean; \
	fi

##
#
# ppdfb-distclean: remove all ppDFB files including the option of
#                  removing the complete source tree.
#
ppdfb-distclean: ppdfb-clean
	@-$(call fn-ppdfb-distclean)
	@if [ -d "${PPDFB_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${PPDFB_SRC_DIR} completely."; \
		$(ECHO) "Removing ${PPDFB_SRC_DIR}..."; \
		$(RM) -r ${PPDFB_SRC_DIR}; \
	fi

##
#
# ppdfb-mrproper: remove all ppDFB files including
#                 the complete source tree.
#
ppdfb-mrproper: ppdfb-clean
	@-$(call fn-ppdfb-distclean)
	@if [ -d "${PPDFB_SRC_DIR}" ]; then \
		$(RM) -r ${PPDFB_SRC_DIR}; \
	fi

##
#
# fn-ppdfb-configure: function to configure ppDFB.
#
define fn-ppdfb-configure
	$(ECHO) "***************************"; \
	$(ECHO) "* Configuring ++DFB-${PPDFB_VERSION} *"; \
	$(ECHO) "***************************"; \
	$(CD) ${PPDFB_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR}"; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) PKG_CONFIG_LIBDIR=${DIRECTFB_INSTALL_LIB_DIR}; \
	$(EXPORT) PKG_CONFIG_PATH=${DIRECTFB_INSTALL_PKG_CONFIG_DIR}; \
	./configure $(PPDFB_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure ++DFB - Aborting! *"; \
		$(ECHO) "************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${includedir};${PPDFB_INSTALL_INCLUDE_DIR};g;\
		           /Libs/s;\$${libdir};${PPDFB_INSTALL_LIB_DIR};g" ${PPDFB_SRC_DIR}/${PPDFB_PKG_CONFIG}.pc; \
		$(TOUCH) ${PPDFB_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-ppdfb-compile: function to compile ppDFB.
#
define fn-ppdfb-compile
	$(ECHO) "************************"; \
	$(ECHO) "* Building ++DFB-${PPDFB_VERSION} *"; \
	$(ECHO) "************************"; \
	$(CD) ${PPDFB_SRC_DIR}; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR}"; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) BUILD_DIR="${DIRECTFB_BUILD_VER_DIR}"; \
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${PPDFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n********************************************"; \
	    $(ECHO) "* ERROR: Failed to build ++DFB - Aborting! *"; \
		$(ECHO) "********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${PPDFB_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-ppdfb-install: function to install ppDFB.
#
define fn-ppdfb-install
	$(ECHO) "**************************"; \
	$(ECHO) "* Installing ++DFB-${PPDFB_VERSION} *"; \
	$(ECHO) "**************************"; \
	$(CD) ${PPDFB_SRC_DIR}; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR}"; \
	$(EXPORT) CC="${CCACHE} ${CC}"; \
	$(EXPORT) CXX="${CCACHE} ${CXX}"; \
	$(EXPORT) AR=${AR}; \
	$(EXPORT) NM=${NM}; \
	$(EXPORT) STRIP=${STRIP}; \
	$(EXPORT) OBJCOPY=${OBJCOPY}; \
	$(EXPORT) OBJDUMP=${OBJDUMP}; \
	$(EXPORT) RANLIB=${RANLIB}; \
	$(EXPORT) BUILD_DIR="${DIRECTFB_BUILD_VER_DIR}"; \
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${PPDFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n**********************************************"; \
	    $(ECHO) "* ERROR: Failed to install ++DFB - Aborting! *"; \
		$(ECHO) "**********************************************"; \
	    $(FALSE); \
	else \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh ${PPDFB_INSTALL_BIN_PPD_VER_DIR}/runppd.sh; \
		$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${PPDFB_INSTALL_ROOT_DIR};g" ${PPDFB_INSTALL_LIB_DIR}/lib++dfb.la; \
		$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${PPDFB_INSTALL_ROOT_DIR};g" ${PPDFB_INSTALL_LIB_DIR}/lib++dfb.la; \
		$(ECHO) "Ver: ${PPDFB_VERSION}"  > ${PPDFB_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${PPDFB_SRC_DIR}" >> ${PPDFB_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-ppdfb-distclean: function to clean out all files.
#
define fn-ppdfb-distclean
	if [ -e "${PPDFB_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${PPDFB_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${PPDFB_CONFIGURED_FLAG}" ]; then \
		$(RM) ${PPDFB_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${PPDFB_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${PPDFB_SRC_DIR} distclean; \
	fi; \
	if [ -e "${PPDFB_INSTALL_INCLUDE_DIR}/++dfb" ]; then \
		$(RM) -r ${PPDFB_INSTALL_INCLUDE_DIR}/++dfb; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${PPDFB_UNPACKED_FLAG}:
	@if ! [ -e "${PPDFB_SRC_DIR}" ]; then \
		${MKDIR} "${PPDFB_SRC_DIR}";\
	fi
	@if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${PPDFB_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${PPDFB_SOURCE_TARBALL}'!";\
		$(FALSE);\
	else \
		if [ `$(STAT) -c "%s" ${DIRECTFB_SRC_DFB_TAR_DIR}/${PPDFB_SOURCE_TARBALL}` -eq 0 ]; then \
			$(ECHO) "ERROR: Size of '${DIRECTFB_SRC_DFB_TAR_DIR}/${PPDFB_SOURCE_TARBALL}' is 0!";\
			$(FALSE);\
		else \
			$(ECHO) "*******************************"; \
			$(ECHO) "* Creating ++DFB-${PPDFB_VERSION} Source *"; \
			$(ECHO) "*******************************"; \
			$(TAR) xzkf $(DIRECTFB_SRC_DFB_TAR_DIR)/${PPDFB_SOURCE_TARBALL} -C $(DIRECTFB_SRC_DIR); \
			if [ -d "${PPDFB_PUB_PPD_VER1_DIR}" ]; then \
				$(CP) -r ${PPDFB_PUB_PPD_VER1_DIR}/* ${PPDFB_SRC_DIR}; \
			fi; \
			if [ -d "${PPDFB_PUB_PPD_VER2_DIR}" ]; then \
				$(CP) -r ${PPDFB_PUB_PPD_VER2_DIR}/* ${PPDFB_SRC_DIR}; \
			fi; \
			if [ -d "${PPDFB_PRI_PPD_VER1_DIR}" ]; then \
				$(CP) -r ${PPDFB_PRI_PPD_VER1_DIR}/* ${PPDFB_SRC_DIR}; \
			fi; \
			if [ -d "${PPDFB_PRI_PPD_VER2_DIR}" ]; then \
				$(CP) -r ${PPDFB_PRI_PPD_VER2_DIR}/* ${PPDFB_SRC_DIR}; \
			fi; \
			$(TOUCH) ${PPDFB_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${PPDFB_CONFIGURATION_FLAG}: ${PPDFB_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${PPDFB_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${PPDFB_SRC_DIR}/.config.$$$$; \
	if [ -e "${PPDFB_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${PPDFB_CONFIGURATION_FLAG} ${PPDFB_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} ppdfb-clean; \
			$(CP) ${PPDFB_SRC_DIR}/.config.$$$$ ${PPDFB_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${PPDFB_SRC_DIR}/.config.$$$$ ${PPDFB_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${PPDFB_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${PPDFB_CONFIGURED_FLAG}: ${DIRECTFB_FLAG} ${PPDFB_CONFIGURATION_FLAG}
	@$(call fn-ppdfb-configure)

##
#
# Dependency target not to be called directly.
#
${PPDFB_COMPILED_FLAG}: ${PPDFB_CONFIGURED_FLAG}
	@$(call fn-ppdfb-compile)

##
#
# Dependency target not to be called directly.
#
${PPDFB_INSTALLED_FLAG}: ${PPDFB_COMPILED_FLAG}
	@$(call fn-ppdfb-install)

##
#
# Dependency target not to be called directly.
#
${PPDFB_FLAG}: ${PPDFB_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${PPDFB_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${PPDFB_PKG_CONFIG} > ${PPDFB_SRC_DIR}/.install.$$$$; \
	if [ -e "${PPDFB_FLAG}" ]; then \
		if ($(DIFF) ${PPDFB_FLAG} ${PPDFB_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${PPDFB_SRC_DIR}/.install.$$$$ ${PPDFB_FLAG}; \
		fi; \
	else \
		$(CP) ${PPDFB_SRC_DIR}/.install.$$$$ ${PPDFB_FLAG}; \
	fi; \
	$(RM) ${PPDFB_SRC_DIR}/.install.$$$$

