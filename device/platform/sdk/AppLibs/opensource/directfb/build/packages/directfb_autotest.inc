
.PHONY: directfb-autotest-all directfb-autotest-check-config directfb-autotest-config
.PHONY: directfb-autotest directfb-autotest-compile directfb-autotest-install directfb-autotest-uninstall directfb-autotest-clean


############################################################
#
# directfb-autotest-all: force DirectFB autotest to be configured and compiled.
#
directfb-autotest-all: directfb-autotest-config directfb-autotest-compile directfb-autotest-install

##
#
# directfb-autotest-check-config: check to see whether the build environment or
#                                 build options have changed since the last
#                                 build.  If they have, then this will force a
#                                 clean and then a rebuild.
#
directfb-autotest-check-config: ${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}

##
#
# directfb-autotest-config: force DirectFB Autotest to be configured.
#
directfb-autotest-config: directfb-autotest-check-config
	@if [ -e "${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}; \
	fi
	@$(call fn-directfb-autotest-configure)

##
#
# directfb-autotest-compile: force DirectFB Autotest to be compiled (tracks changes to source code files)
#
directfb-autotest-compile: ${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}
	@if [ -e "${DIRECTFB_AUTOTEST_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_COMPILED_FLAG}; \
	fi
	@$(call fn-directfb-autotest-compile)

##
#
# directfb-autotest-install: force DirectFB Autotest to be installed (tracks changes to source code files)
#
directfb-autotest-install: ${DIRECTFB_AUTOTEST_COMPILED_FLAG}
	@if [ -e "${DIRECTFB_AUTOTEST_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}; \
	fi
	@$(call fn-directfb-autotest-install)

##
#
# directfb-autotest: force DirectFB Autotest to be compiled and installed in one pass (tracks changes to source code files)
#
directfb-autotest: directfb-autotest-install

##
#
# directfb-autotest-uninstall: force DirectFB Autotest to be uninstalled
#
directfb-autotest-uninstall:
	@if [ -e "${DIRECTFB_AUTOTEST_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_AUTOTEST_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_AUTOTEST_SRC_DIR} uninstall DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	fi

##
#
# directfb-autotest-clean: remove all intermediate object and dependency files
#
directfb-autotest-clean: directfb-autotest-uninstall
	@if [ -e "${DIRECTFB_AUTOTEST_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_COMPILED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_AUTOTEST_FLAG}" ]; then \
		$(RM) ${DIRECTFB_AUTOTEST_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_AUTOTEST_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_AUTOTEST_SRC_DIR} clean; \
	fi

##
#
# fn-directfb-autotest-configure: function to configure DirectFB Autotest
#
define fn-directfb-autotest-configure
	$(ECHO) "***************************************"; \
	$(ECHO) "* Configuring DirectFB-autotest-${DIRECTFB_AUTOTEST_VERSION} *"; \
	$(ECHO) "***************************************"; \
	$(CD) ${DIRECTFB_AUTOTEST_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS} -DBUILD_AUTOMATION"; \
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
	./configure $(DIRECTFB_AUTOTEST_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n************************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure DirectFB Autotest - Aborting! *"; \
		$(ECHO) "************************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-directfb-autotest-compile: function to compile DirectFB Autotest
#
define fn-directfb-autotest-compile
	$(ECHO) "************************************"; \
	$(ECHO) "* Building DirectFB-autotest-${DIRECTFB_AUTOTEST_VERSION} *"; \
	$(ECHO) "************************************"; \
	$(CD) ${DIRECTFB_AUTOTEST_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n********************************************************"; \
	    $(ECHO) "* ERROR: Failed to build DirectFB Autotest - Aborting! *"; \
		$(ECHO) "********************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIRECTFB_AUTOTEST_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-directfb-autotest-install: function to install DirectFB Autotest
#
define fn-directfb-autotest-install
	$(ECHO) "**************************************"; \
	$(ECHO) "* Installing DirectFB-autotest-${DIRECTFB_AUTOTEST_VERSION} *"; \
	$(ECHO) "**************************************"; \
	$(CD) ${DIRECTFB_AUTOTEST_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n**********************************************************"; \
	    $(ECHO) "* ERROR: Failed to install DirectFB Autotest - Aborting! *"; \
		$(ECHO) "**********************************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DIRECTFB_AUTOTEST_VERSION}"  > ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DIRECTFB_AUTOTEST_SRC_DIR}" >> ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}: ${DIRECTFB_AUTOTEST_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$; \
	if [ -e "${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG} ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} directfb-autotest-clean; \
			$(CP) ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$ ${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$ ${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_AUTOTEST_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}: ${DIRECTFB_FLAG} ${DIRECTFB_AUTOTEST_CONFIGURATION_FLAG}
	@$(call fn-directfb-autotest-configure)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_AUTOTEST_COMPILED_FLAG}: ${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}
	@$(call fn-directfb-autotest-compile)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_AUTOTEST_INSTALLED_FLAG}: ${DIRECTFB_AUTOTEST_COMPILED_FLAG}
	@$(call fn-directfb-autotest-install)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_AUTOTEST_FLAG}: ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}
	@$(ECHO) -e "${DIRECTFB_AUTOTEST_VERSION}\n${BRCM_CPPFLAGS}" > ${DIRECTFB_AUTOTEST_SRC_DIR}/.install.$$$$; \
	if [ -e "${DIRECTFB_AUTOTEST_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_AUTOTEST_FLAG} ${DIRECTFB_AUTOTEST_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_AUTOTEST_SRC_DIR}/.install.$$$$ ${DIRECTFB_AUTOTEST_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_AUTOTEST_SRC_DIR}/.install.$$$$ ${DIRECTFB_AUTOTEST_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_AUTOTEST_SRC_DIR}/.install.$$$$
