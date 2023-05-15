
.PHONY: directfb-examples-all directfb-examples-source directfb-examples-check-config directfb-examples-autogen directfb-examples-config
.PHONY: directfb-examples directfb-examples-compile directfb-examples-install directfb-examples-uninstall directfb-examples-clean
.PHONY: directfb-examples-distclean directfb-examples-mrproper


############################################################
#
# directfb-examples-all: force DirectFB examples to be configured and compiled.
#
directfb-examples-all: directfb-examples-config directfb-examples-compile directfb-examples-install

##
#
# directfb-examples-source: unpack the DirectFB-examples source code.
#
directfb-examples-source: check ${DIRECTFB_EXAMPLES_UNPACKED_FLAG}

##
#
# directfb-examples-check-config: check to see whether the build environment or
#                                 build options have changed since the last
#                                 build.  If they have, then this will force a
#                                 clean and then a rebuild.
#
directfb-examples-check-config: directfb-examples-source ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}

##
#
# directfb-examples-autogen: force DirectFB examples to be configured and autoconf .in files to be regenerated.
#
directfb-examples-autogen: check-autogen-tools directfb-examples-check-config
	@$(ECHO) "********************************************************"; \
	$(ECHO)  "* Autogenerating DirectFB-examples-${DIRECTFB_EXAMPLES_VERSION} Configuration *"; \
	$(ECHO)  "********************************************************"; \
	if [ -e "${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${DIRECTFB_EXAMPLES_SRC_DIR}; \
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
	./autogen.sh $(DIRECTFB_EXAMPLES_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***************************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate DirectFB Examples - Aborting! *"; \
		$(ECHO) "***************************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}; \
	fi

##
#
# directfb-examples-config: force DirectFB Examples to be configured.
#
directfb-examples-config: directfb-examples-check-config
	@if [ -e "${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}; \
	fi
	@$(call fn-directfb-examples-configure)

##
#
# directfb-examples-compile: force DirectFB Examples to be compiled (tracks changes to source code files)
#
directfb-examples-compile: ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}
	@if [ -e "${DIRECTFB_EXAMPLES_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_COMPILED_FLAG}; \
	fi
	@$(call fn-directfb-examples-compile)

##
#
# directfb-examples-install: force DirectFB Examples to be installed (tracks changes to source code files)
#
directfb-examples-install: ${DIRECTFB_EXAMPLES_COMPILED_FLAG}
	@if [ -e "${DIRECTFB_EXAMPLES_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}; \
	fi
	@$(call fn-directfb-examples-install)

##
#
# directfb-examples: force DirectFB Examples to be compiled and installed in one pass (tracks changes to source code files)
#
directfb-examples: directfb-examples-install

##
#
# directfb-examples-uninstall: force DirectFB Examples to be uninstalled
#
directfb-examples-uninstall:
	@if [ -e "${DIRECTFB_EXAMPLES_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_EXAMPLES_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_EXAMPLES_SRC_DIR} uninstall DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	fi

##
#
# directfb-examples-clean: remove all intermediate object and dependency files
#
directfb-examples-clean: directfb-examples-uninstall
	@if [ -e "${DIRECTFB_EXAMPLES_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_COMPILED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_EXAMPLES_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_EXAMPLES_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_EXAMPLES_SRC_DIR} clean; \
	fi

##
#
# directfb-examples-distclean: remove all DirectFB Example files with the option to remove source.
#
directfb-examples-distclean: directfb-examples-clean
	@-$(call fn-directfb-examples-distclean)
	@if [ -d "${DIRECTFB_EXAMPLES_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DIRECTFB_EXAMPLES_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DIRECTFB_EXAMPLES_SRC_DIR}..."; \
		$(RM) -r ${DIRECTFB_EXAMPLES_SRC_DIR}; \
	fi

##
#
# directfb-examples-mrproper: remove all DirectFB Example files including the source.
#
directfb-examples-mrproper: directfb-examples-clean
	@-$(call fn-directfb-examples-distclean)
	@if [ -d "${DIRECTFB_EXAMPLES_SRC_DIR}" ]; then \
		$(RM) -r ${DIRECTFB_EXAMPLES_SRC_DIR}; \
	fi

##
#
# fn-directfb-examples-configure: function to configure DirectFB Examples
#
define fn-directfb-examples-configure
	$(ECHO) "***************************************"; \
	$(ECHO) "* Configuring DirectFB-examples-${DIRECTFB_EXAMPLES_VERSION} *"; \
	$(ECHO) "***************************************"; \
	$(CD) ${DIRECTFB_EXAMPLES_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) AS=${AS}; \
	$(EXPORT) LD=${LD}; \
	if [ "${DIRECTFB_SHARED}" = "y" ]; then \
		$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR}"; \
	else \
		$(EXPORT) LDFLAGS="${LDFLAGS} -L${DIRECTFB_INSTALL_LIB_DIR} -L${DFB_ZLIB_INSTALL_LIB_DIR}"; \
	fi; \
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
	./configure $(DIRECTFB_EXAMPLES_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n************************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure DirectFB Examples - Aborting! *"; \
		$(ECHO) "************************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-directfb-examples-compile: function to compile DirectFB Examples
#
define fn-directfb-examples-compile
	$(ECHO) "************************************"; \
	$(ECHO) "* Building DirectFB-examples-${DIRECTFB_EXAMPLES_VERSION} *"; \
	$(ECHO) "************************************"; \
	$(CD) ${DIRECTFB_EXAMPLES_SRC_DIR}; \
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
	    $(ECHO) "* ERROR: Failed to build DirectFB Examples - Aborting! *"; \
		$(ECHO) "********************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIRECTFB_EXAMPLES_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-directfb-examples-install: function to install DirectFB Examples
#
define fn-directfb-examples-install
	$(ECHO) "**************************************"; \
	$(ECHO) "* Installing DirectFB-examples-${DIRECTFB_EXAMPLES_VERSION} *"; \
	$(ECHO) "**************************************"; \
	$(CD) ${DIRECTFB_EXAMPLES_SRC_DIR}; \
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
	    $(ECHO) "* ERROR: Failed to install DirectFB Examples - Aborting! *"; \
		$(ECHO) "**********************************************************"; \
	    $(FALSE); \
	else \
                ${MKDIR} -p ${DIRECTFB_INSTALL_SHARE_DIR}/fonts; \
                for font in ${DIRECTFB_INSTALL_SHARE_DIR}/directfb-${DIRECTFB_VERSION}/fonts/*.ttf; \
                do \
                        ${CP} $$font ${DIRECTFB_INSTALL_SHARE_DIR}/fonts; \
                done; \
		$(ECHO) "Ver: ${DIRECTFB_EXAMPLES_VERSION}"  > ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DIRECTFB_EXAMPLES_SRC_DIR}" >> ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}; \
	fi; \
	if [ -e ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfb_input_hotplug ]; then \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfb_input_hotplug ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}; \
	fi;

endef

##
#
# fn-directfb-examples-distclean: function to clean out all files.
#
define fn-directfb-examples-distclean
	if [ -e "${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DIRECTFB_EXAMPLES_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_EXAMPLES_SRC_DIR} distclean; \
	fi; \
	if [ -e "${DIRECTFB_EXAMPLES_INSTALL_SHARE_DIR}/directfb-examples" ]; then \
		$(RM) -r ${DIRECTFB_EXAMPLES_INSTALL_SHARE_DIR}/directfb-examples; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_UNPACKED_FLAG}:
	@if ! [ -e "${DIRECTFB_EXAMPLES_SRC_DIR}" ]; then \
		${MKDIR} "${DIRECTFB_EXAMPLES_SRC_DIR}";\
	fi
	@if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_EXAMPLES_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_EXAMPLES_SOURCE_TARBALL}'!";\
		$(FALSE);\
	else \
		if [ `$(STAT) -c "%s" ${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_EXAMPLES_SOURCE_TARBALL}` -eq 0 ]; then \
			$(ECHO) "ERROR: Size of '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIRECTFB_EXAMPLES_SOURCE_TARBALL}' is 0!";\
			$(FALSE);\
		else \
			$(ECHO) "*******************************************"; \
			$(ECHO) "* Creating DirectFB-examples-${DIRECTFB_EXAMPLES_VERSION} Source *"; \
			$(ECHO) "*******************************************"; \
			$(TAR) xzkf $(DIRECTFB_SRC_DFB_TAR_DIR)/${DIRECTFB_EXAMPLES_SOURCE_TARBALL} -C $(DIRECTFB_SRC_DIR); \
			if [ -d "${DIRECTFB_EXAMPLES_PUB_DFB_VER1_DIR}" ]; then \
				$(CP) -r ${DIRECTFB_EXAMPLES_PUB_DFB_VER1_DIR}/* ${DIRECTFB_EXAMPLES_SRC_DIR}; \
			fi; \
			if [ -d "${DIRECTFB_EXAMPLES_PUB_DFB_VER2_DIR}" ]; then \
				$(CP) -r ${DIRECTFB_EXAMPLES_PUB_DFB_VER2_DIR}/* ${DIRECTFB_EXAMPLES_SRC_DIR}; \
			fi; \
			if [ -d "${DIRECTFB_EXAMPLES_PRI_DFB_VER1_DIR}" ]; then \
				$(CP) -r ${DIRECTFB_EXAMPLES_PRI_DFB_VER1_DIR}/* ${DIRECTFB_EXAMPLES_SRC_DIR}; \
			fi; \
			if [ -d "${DIRECTFB_EXAMPLES_PRI_DFB_VER2_DIR}" ]; then \
				$(CP) -r ${DIRECTFB_EXAMPLES_PRI_DFB_VER2_DIR}/* ${DIRECTFB_EXAMPLES_SRC_DIR}; \
			fi; \
			$(TOUCH) ${DIRECTFB_EXAMPLES_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}: ${DIRECTFB_EXAMPLES_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$; \
	if [ -e "${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG} ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} directfb-examples-clean; \
			$(CP) ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$ ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$ ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_EXAMPLES_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}: ${DIRECTFB_FLAG} ${DIRECTFB_EXAMPLES_CONFIGURATION_FLAG}
	@$(call fn-directfb-examples-configure)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_COMPILED_FLAG}: ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}
	@$(call fn-directfb-examples-compile)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_INSTALLED_FLAG}: ${DIRECTFB_EXAMPLES_COMPILED_FLAG}
	@$(call fn-directfb-examples-install)

##
#
# Dependency target not to be called directly.
#
${DIRECTFB_EXAMPLES_FLAG}: ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}
	@$(ECHO) -e "${DIRECTFB_EXAMPLES_VERSION}\n${BRCM_CPPFLAGS}" > ${DIRECTFB_EXAMPLES_SRC_DIR}/.install.$$$$; \
	if [ -e "${DIRECTFB_EXAMPLES_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_EXAMPLES_FLAG} ${DIRECTFB_EXAMPLES_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_EXAMPLES_SRC_DIR}/.install.$$$$ ${DIRECTFB_EXAMPLES_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_EXAMPLES_SRC_DIR}/.install.$$$$ ${DIRECTFB_EXAMPLES_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_EXAMPLES_SRC_DIR}/.install.$$$$
