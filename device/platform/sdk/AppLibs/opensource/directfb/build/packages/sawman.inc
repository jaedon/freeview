
.PHONY: sawman-all sawman-source sawman-check-config sawman-autogen sawman-config sawman sawman-compile sawman-install
.PHONY: sawman-uninstall sawman-clean sawman-distclean sawman-mrproper

############################################################
#
# sawman-all: force SaWMan to be configured and compiled.
#
sawman-all: sawman-config sawman-compile sawman-install

##
#
# sawman-source: unpack the SaWMan source code.
#
sawman-source: check ${SAWMAN_UNPACKED_FLAG}

##
#
# sawman-check-config: check to see whether the build environment or build options have changed
#                      since the last build.  If they have, then this will force a clean and
#                      then a rebuild of SaWMan.
#
sawman-check-config: sawman-source ${SAWMAN_CONFIGURATION_FLAG}

##
#
# sawman-autogen: force SaWMan configure and autoconf .in files to be regenerated.
#
sawman-autogen: check-autogen-tools sawman-check-config
	@$(ECHO) "*********************************************"; \
	$(ECHO)  "* Autogenerating SaWMan-${SAWMAN_VERSION} Configuration *"; \
	$(ECHO)  "*********************************************"; \
	if [ -e "${SAWMAN_CONFIGURED_FLAG}" ]; then \
		$(RM) ${SAWMAN_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${SAWMAN_SRC_DIR}; \
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
	./autogen.sh $(SAWMAN_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n****************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate SaWMan - Aborting! *"; \
		$(ECHO) "****************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${SAWMAN_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${SAWMAN_INSTALL_LIB_DIR};g" ${SAWMAN_SRC_DIR}/${SAWMAN_PKG_CONFIG}.pc; \
		$(TOUCH) ${SAWMAN_CONFIGURED_FLAG}; \
	fi

##
#
# sawman-config: force SaWMan to be configured.
#
sawman-config: sawman-check-config
	@if [ -e "${SAWMAN_CONFIGURED_FLAG}" ]; then \
		$(RM) ${SAWMAN_CONFIGURED_FLAG}; \
	fi
	@$(call fn-sawman-configure)

##
#
# sawman-compile: force SaWMan to be compiled (tracks changes to source code files)
#
sawman-compile: ${SAWMAN_CONFIGURED_FLAG}
	@if [ -e "${SAWMAN_COMPILED_FLAG}" ]; then \
		$(RM) ${SAWMAN_COMPILED_FLAG}; \
	fi
	@$(call fn-sawman-compile)

##
#
# sawman-install: force SaWMan to be installed (tracks changes to source code files)
#
sawman-install: ${SAWMAN_COMPILED_FLAG}
	@if [ -e "${SAWMAN_INSTALLED_FLAG}" ]; then \
		$(RM) ${SAWMAN_INSTALLED_FLAG}; \
	fi
	@$(call fn-sawman-install)

##
#
# sawman: force SaWMan to be compiled and installed in one pass (tracks changes to source code files)
#
sawman: sawman-install

##
#
# sawman-uninstall: force SaWMan to be uninstalled
#
sawman-uninstall:
	@if [ -e "${SAWMAN_INSTALLED_FLAG}" ]; then \
		$(RM) ${SAWMAN_INSTALLED_FLAG}; \
	fi
	@if [ -e "${SAWMAN_INSTALL_BIN_SAW_VER_DIR}/runsaw.sh" ]; then \
		$(RM) ${SAWMAN_INSTALL_BIN_SAW_VER_DIR}/runsaw.sh; \
	fi
	@-if [ -e "${SAWMAN_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${SAWMAN_SRC_DIR} uninstall DESTDIR=${SAWMAN_INSTALL_DIR}; \
	fi

##
#
# sawman-clean: remove all intermediate object and dependency files
#
sawman-clean: sawman-uninstall
	@if [ -e "${SAWMAN_COMPILED_FLAG}" ]; then \
		$(RM) ${SAWMAN_COMPILED_FLAG}; \
	fi
	@if [ -e "${SAWMAN_FLAG}" ]; then \
		$(RM) ${SAWMAN_FLAG}; \
	fi
	@-if [ -e "${SAWMAN_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${SAWMAN_SRC_DIR} clean; \
	fi

##
#
# sawman-distclean: remove all SaWMan files with the option to remove source.
#
sawman-distclean: sawman-clean
	@-$(call fn-sawman-distclean)
	@if [ -d "${SAWMAN_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${SAWMAN_SRC_DIR} completely."; \
		$(ECHO) "Removing ${SAWMAN_SRC_DIR}..."; \
		$(RM) -r ${SAWMAN_SRC_DIR}; \
	fi

##
#
# sawman-mrproper: remove all SaWMan files including the source.
#
sawman-mrproper: sawman-clean
	@-$(call fn-sawman-distclean)
	@if [ -d "${SAWMAN_SRC_DIR}" ]; then \
		$(RM) -r ${SAWMAN_SRC_DIR}; \
	fi

##
#
# fn-sawman-configure: function to configure SaWMan
#
define fn-sawman-configure
	$(ECHO) "****************************"; \
	$(ECHO) "* Configuring SaWMan-${SAWMAN_VERSION} *"; \
	$(ECHO) "****************************"; \
	$(CD) ${SAWMAN_SRC_DIR}; \
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
	./configure $(SAWMAN_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure SaWMan - Aborting! *"; \
		$(ECHO) "*************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${prefix};${SAWMAN_INSTALL_ROOT_DIR};g;\
		           /Libs/s;\$${libdir};${SAWMAN_INSTALL_LIB_DIR};g" ${SAWMAN_SRC_DIR}/${SAWMAN_PKG_CONFIG}.pc; \
		$(TOUCH) ${SAWMAN_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-sawman-compile: function to compile SaWMan
#
define fn-sawman-compile
	$(ECHO) "*************************"; \
	$(ECHO) "* Building SaWMan-${SAWMAN_VERSION} *"; \
	$(ECHO) "*************************"; \
	$(CD) ${SAWMAN_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${SAWMAN_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*********************************************"; \
	    $(ECHO) "* ERROR: Failed to build SaWMan - Aborting! *"; \
		$(ECHO) "*********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${SAWMAN_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-sawman-install: function to install SaWMan
#
define fn-sawman-install
	$(ECHO) "***************************"; \
	$(ECHO) "* Installing SaWMan-${SAWMAN_VERSION} *"; \
	$(ECHO) "***************************"; \
	$(CD) ${SAWMAN_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${SAWMAN_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***********************************************"; \
	    $(ECHO) "* ERROR: Failed to install SaWMan - Aborting! *"; \
		$(ECHO) "***********************************************"; \
	    $(FALSE); \
	else \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh ${SAWMAN_INSTALL_BIN_SAW_VER_DIR}/runsaw.sh; \
		if [ "${DIRECTFB_CLIENT_LIB}" = "y" -a "${DIRECTFB_MASTER_LIB}" = "y" ]; then \
			$(MKDIR) -p ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}; \
			$(CD) ${DIRECTFB_INSTALL_LIB_DFB_DIR}; \
			$(TAR) c * --exclude \*nexus\*  | $(TAR) x -C ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}; \
		fi; \
		$(CD) ${SAWMAN_INSTALL_BIN_SAW_VER_DIR}; \
		$(LN) ${DIRECTFB_TARGET_BIN_DFB_VER_DIR}/df_andi; \
		$(LN) ${DIRECTFB_TARGET_BIN_DFB_VER_DIR}/df_window; \
		$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${SAWMAN_INSTALL_ROOT_DIR};g" ${SAWMAN_INSTALL_LIB_DIR}/libsawman.la; \
		$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${SAWMAN_INSTALL_ROOT_DIR};g" ${SAWMAN_INSTALL_LIB_DIR}/libsawman.la; \
		$(ECHO) "Ver: ${SAWMAN_VERSION}"  > ${SAWMAN_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${SAWMAN_SRC_DIR}" >> ${SAWMAN_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-sawman-distclean: function to clean out all files.
#
define fn-sawman-distclean
	if [ -e "${SAWMAN_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${SAWMAN_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${SAWMAN_CONFIGURED_FLAG}" ]; then \
		$(RM) ${SAWMAN_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${SAWMAN_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${SAWMAN_SRC_DIR} distclean; \
	fi; \
	if [ -d "${SAWMAN_INSTALL_INCLUDE_DIR}/sawman" ]; then \
		$(RM) -r ${SAWMAN_INSTALL_INCLUDE_DIR}/sawman; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${SAWMAN_UNPACKED_FLAG}:
	@if ! [ -e "${SAWMAN_SRC_DIR}" ]; then \
		${MKDIR} "${SAWMAN_SRC_DIR}";\
	fi
	@if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${SAWMAN_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${SAWMAN_SOURCE_TARBALL}'!";\
		$(FALSE);\
	else \
		if [ `$(STAT) -c "%s" ${DIRECTFB_SRC_DFB_TAR_DIR}/${SAWMAN_SOURCE_TARBALL}` -eq 0 ]; then \
			$(ECHO) "ERROR: Size of '${DIRECTFB_SRC_DFB_TAR_DIR}/${SAWMAN_SOURCE_TARBALL}' is 0!";\
			$(FALSE);\
		else \
			$(ECHO) "********************************"; \
			$(ECHO) "* Creating SaWMan-${SAWMAN_VERSION} Source *"; \
			$(ECHO) "********************************"; \
			$(TAR) xzkfm $(DIRECTFB_SRC_DFB_TAR_DIR)/${SAWMAN_SOURCE_TARBALL} -C $(DIRECTFB_SRC_DIR); \
			if [ -d "${SAWMAN_PUB_SAW_VER2_DIR}" ]; then \
                              $(FIND) ${SAWMAN_PUB_SAW_VER2_DIR} -type d | $(SED) 's|${SAWMAN_PUB_SAW_VER2_DIR}||' | $(XARGS) -i $(MKDIR) ${SAWMAN_SRC_DIR}{}; \
                              $(FIND) ${SAWMAN_PUB_SAW_VER2_DIR} -type f | $(SED) 's|${SAWMAN_PUB_SAW_VER2_DIR}||' | $(XARGS) -i $(CP) ${SAWMAN_PUB_SAW_VER2_DIR}{} ${SAWMAN_SRC_DIR}{}; \
			fi; \
			if [ -d "${SAWMAN_PRI_SAW_VER2_DIR}" ]; then \
                              $(FIND) ${SAWMAN_PRI_SAW_VER2_DIR} -type d | $(SED) 's|${SAWMAN_PRI_SAW_VER2_DIR}||' | $(XARGS) -i $(MKDIR) ${SAWMAN_SRC_DIR}{}; \
                              $(FIND) ${SAWMAN_PRI_SAW_VER2_DIR} -type f | $(SED) 's|${SAWMAN_PRI_SAW_VER2_DIR}||' | $(XARGS) -i $(CP) ${SAWMAN_PRI_SAW_VER2_DIR}{} ${SAWMAN_SRC_DIR}{}; \
			fi; \
			$(TOUCH) ${SAWMAN_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${SAWMAN_CONFIGURATION_FLAG}: ${SAWMAN_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${SAWMAN_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_MULTI=${DIRECTFB_MULTI}"       >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_FUSION=${BUILD_FUSION}"           >> ${SAWMAN_SRC_DIR}/.config.$$$$; \
	if [ -e "${SAWMAN_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${SAWMAN_CONFIGURATION_FLAG} ${SAWMAN_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} sawman-clean; \
			$(CP) ${SAWMAN_SRC_DIR}/.config.$$$$ ${SAWMAN_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${SAWMAN_SRC_DIR}/.config.$$$$ ${SAWMAN_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${SAWMAN_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${SAWMAN_CONFIGURED_FLAG}: ${DIRECTFB_FLAG} ${SAWMAN_CONFIGURATION_FLAG}
	@$(call fn-sawman-configure)

##
#
# Dependency target not to be called directly.
#
${SAWMAN_COMPILED_FLAG}: ${SAWMAN_CONFIGURED_FLAG}
	@$(call fn-sawman-compile)

##
#
# Dependency target not to be called directly.
#
${SAWMAN_INSTALLED_FLAG}: ${SAWMAN_COMPILED_FLAG}
	@$(call fn-sawman-install)

##
#
# Dependency target not to be called directly.
#
${SAWMAN_FLAG}: ${SAWMAN_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${SAWMAN_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${SAWMAN_PKG_CONFIG} > ${SAWMAN_SRC_DIR}/.install.$$$$; \
	if [ -e "${SAWMAN_FLAG}" ]; then \
		if ($(DIFF) ${SAWMAN_FLAG} ${SAWMAN_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${SAWMAN_SRC_DIR}/.install.$$$$ ${SAWMAN_FLAG}; \
		fi; \
	else \
		$(CP) ${SAWMAN_SRC_DIR}/.install.$$$$ ${SAWMAN_FLAG}; \
	fi; \
	$(RM) ${SAWMAN_SRC_DIR}/.install.$$$$

