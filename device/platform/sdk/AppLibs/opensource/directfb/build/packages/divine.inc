
.PHONY: divine-all divine-source divine-check-config divine-config divine divine-compile divine-install divine-uninstall
.PHONY: divine-clean divine-distclean divine-mrproper

############################################################
#
# divine-all: force DiVine to be configured and compiled.
#
divine-all: divine-config divine-compile divine-install

##
#
# divine-source: unpack the DiVine source code.
#
divine-source: check ${DIVINE_UNPACKED_FLAG}

##
#
# divine-check-config: check to see whether the build environment or build options have changed
#                     since the last build.  If they have, then this will force a clean and
#                     then a rebuild of DiVine.
#
divine-check-config: divine-source ${DIVINE_CONFIGURATION_FLAG}

##
#
# divine-autogen: force configure and autoconf .in files to be regenerated.
#
divine-autogen: check-autogen-tools divine-check-config
	@$(ECHO) "*********************************************"; \
	$(ECHO)  "* Autogenerating DiVine-${DIVINE_VERSION} Configuration *"; \
	$(ECHO)  "*********************************************"; \
	if [ -e "${DIVINE_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIVINE_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${DIVINE_SRC_DIR}; \
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
	./configure $(DIVINE_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n****************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate DiVine - Aborting! *"; \
		$(ECHO) "****************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${includedir};${DIVINE_INSTALL_INCLUDE_DIR};g;\
		           /Libs/s;\$${libdir};${DIVINE_INSTALL_LIB_DIR};g" ${DIVINE_SRC_DIR}/${DIVINE_PKG_CONFIG}.pc; \
		$(TOUCH) ${DIVINE_CONFIGURED_FLAG}; \
	fi

##
#
# divine-config: force DiVine to be configured.
#
divine-config: divine-check-config
	@if [ -e "${DIVINE_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIVINE_CONFIGURED_FLAG}; \
	fi
	@$(call fn-divine-configure)

##
#
# divine-compile: force DiVine to be compiled (tracks changes to source code files)
#
divine-compile: ${DIVINE_CONFIGURED_FLAG}
	@if [ -e "${DIVINE_COMPILED_FLAG}" ]; then \
		$(RM) ${DIVINE_COMPILED_FLAG}; \
	fi
	@$(call fn-divine-compile)

##
#
# divine-install: force DiVine to be installed (tracks changes to source code files)
#
divine-install: ${DIVINE_COMPILED_FLAG}
	@if [ -e "${DIVINE_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIVINE_INSTALLED_FLAG}; \
	fi
	@$(call fn-divine-install)

##
#
# divine: force DiVine to be compiled and installed in one pass (tracks changes to source code files)
#
divine: divine-install

##
#
# divine-uninstall: force DiVine to be uninstalled
#
divine-uninstall:
	@if [ -e "${DIVINE_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIVINE_INSTALLED_FLAG}; \
	fi
	@if [ -e "${DIVINE_INSTALL_BIN_DIV_VER_DIR}/rundiv.sh" ]; then \
		$(RM) ${DIVINE_INSTALL_BIN_DIV_VER_DIR}/rundiv.sh; \
	fi
	@-if [ -e "${DIVINE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIVINE_SRC_DIR} uninstall DESTDIR=${DIVINE_INSTALL_DIR}; \
	fi

##
#
# divine-clean: remove all intermediate object and dependency files.
#
divine-clean: divine-uninstall
	@if [ -e "${DIVINE_COMPILED_FLAG}" ]; then \
		$(RM) ${DIVINE_COMPILED_FLAG}; \
	fi
	@if [ -e "${DIVINE_FLAG}" ]; then \
		$(RM) ${DIVINE_FLAG}; \
	fi
	@-if [ -e "${DIVINE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIVINE_SRC_DIR} clean; \
	fi

##
#
# divine-distclean: remove all DiVine files including the option of
#                  removing the complete source tree.
#
divine-distclean: divine-clean
	@-$(call fn-divine-distclean)
	@if [ -d "${DIVINE_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${DIVINE_SRC_DIR} completely."; \
		$(ECHO) "Removing ${DIVINE_SRC_DIR}..."; \
		$(RM) -r ${DIVINE_SRC_DIR}; \
	fi

##
#
# divine-mrproper: remove all DiVine files including the
#                  complete source tree.
#
divine-mrproper: divine-clean
	@-$(call fn-divine-distclean)
	@if [ -d "${DIVINE_SRC_DIR}" ]; then \
		$(RM) -r ${DIVINE_SRC_DIR}; \
	fi

##
#
# fn-divine-configure: function to configure DiVine.
#
define fn-divine-configure
	$(ECHO) "****************************"; \
	$(ECHO) "* Configuring DiVine-${DIVINE_VERSION} *"; \
	$(ECHO) "****************************"; \
	$(CD) ${DIVINE_SRC_DIR}; \
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
	./configure $(DIVINE_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure DiVine - Aborting! *"; \
		$(ECHO) "*************************************************"; \
	    $(FALSE); \
	else \
		$(SED) -i -e "/Cflags:/s;\$${includedir};${DIVINE_INSTALL_INCLUDE_DIR};g;\
		           /Libs/s;\$${libdir};${DIVINE_INSTALL_LIB_DIR};g" ${DIVINE_SRC_DIR}/${DIVINE_PKG_CONFIG}.pc; \
		$(TOUCH) ${DIVINE_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-divine-compile: function to compile DiVine.
#
define fn-divine-compile
	$(ECHO) "*************************"; \
	$(ECHO) "* Building DiVine-${DIVINE_VERSION} *"; \
	$(ECHO) "*************************"; \
	$(CD) ${DIVINE_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${DIVINE_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n*********************************************"; \
	    $(ECHO) "* ERROR: Failed to build DiVine - Aborting! *"; \
		$(ECHO) "*********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${DIVINE_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-divine-install: function to install DiVine.
#
define fn-divine-install
	$(ECHO) "***************************"; \
	$(ECHO) "* Installing DiVine-${DIVINE_VERSION} *"; \
	$(ECHO) "***************************"; \
	$(CD) ${DIVINE_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${DIVINE_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***********************************************"; \
	    $(ECHO) "* ERROR: Failed to install DiVine - Aborting! *"; \
		$(ECHO) "***********************************************"; \
	    $(FALSE); \
	else \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh ${DIVINE_INSTALL_BIN_DIV_VER_DIR}/rundiv.sh; \
		$(SED) -i -e "s;'${DIRECTFB_PREFIX};'${DIVINE_INSTALL_ROOT_DIR};g" ${DIVINE_INSTALL_LIB_DIR}/libdivine.la; \
		$(SED) -i -e "s; ${DIRECTFB_PREFIX}; ${DIVINE_INSTALL_ROOT_DIR};g" ${DIVINE_INSTALL_LIB_DIR}/libdivine.la; \
		$(ECHO) "Ver: ${DIVINE_VERSION}"  > ${DIVINE_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DIVINE_SRC_DIR}" >> ${DIVINE_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-divine-distclean: function to clean out all files.
#
define fn-divine-distclean
	if [ -e "${DIVINE_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${DIVINE_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${DIVINE_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIVINE_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${DIVINE_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIVINE_SRC_DIR} distclean; \
	fi; \
	if [ -e "${DIVINE_INSTALL_INCLUDE_DIR}/divine" ]; then \
		$(RM) -r ${DIVINE_INSTALL_INCLUDE_DIR}/divine; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${DIVINE_UNPACKED_FLAG}:
	@if ! [ -e "${DIVINE_SRC_DIR}" ]; then \
		${MKDIR} "${DIVINE_SRC_DIR}";\
	fi
	@if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${DIVINE_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIVINE_SOURCE_TARBALL}'!";\
		$(FALSE);\
	else \
		if [ `$(STAT) -c "%s" ${DIRECTFB_SRC_DFB_TAR_DIR}/${DIVINE_SOURCE_TARBALL}` -eq 0 ]; then \
			$(ECHO) "ERROR: Size of '${DIRECTFB_SRC_DFB_TAR_DIR}/${DIVINE_SOURCE_TARBALL}' is 0!";\
			$(FALSE);\
		else \
			$(ECHO) "********************************"; \
			$(ECHO) "* Creating DiVine-${DIVINE_VERSION} Source *"; \
			$(ECHO) "********************************"; \
			$(TAR) xzkf $(DIRECTFB_SRC_DFB_TAR_DIR)/${DIVINE_SOURCE_TARBALL} -C $(DIRECTFB_SRC_DIR); \
			if [ -d "${DIVINE_PUB_DIV_VER1_DIR}" ]; then \
				$(CP) -r ${DIVINE_PUB_DIV_VER1_DIR}/* ${DIVINE_SRC_DIR}; \
			fi; \
			if [ -d "${DIVINE_PUB_DIV_VER2_DIR}" ]; then \
				$(CP) -r ${DIVINE_PUB_DIV_VER2_DIR}/* ${DIVINE_SRC_DIR}; \
			fi; \
			if [ -d "${DIVINE_PRI_DIV_VER1_DIR}" ]; then \
				$(CP) -r ${DIVINE_PRI_DIV_VER1_DIR}/* ${DIVINE_SRC_DIR}; \
			fi; \
			if [ -d "${DIVINE_PRI_DIV_VER2_DIR}" ]; then \
				$(CP) -r ${DIVINE_PRI_DIV_VER2_DIR}/* ${DIVINE_SRC_DIR}; \
			fi; \
			$(TOUCH) ${DIVINE_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${DIVINE_CONFIGURATION_FLAG}: ${DIVINE_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${DIVINE_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_VOODOO=${BUILD_VOODOO}"           >> ${DIVINE_SRC_DIR}/.config.$$$$; \
	if [ -e "${DIVINE_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${DIVINE_CONFIGURATION_FLAG} ${DIVINE_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} divine-clean; \
			$(CP) ${DIVINE_SRC_DIR}/.config.$$$$ ${DIVINE_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${DIVINE_SRC_DIR}/.config.$$$$ ${DIVINE_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${DIVINE_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${DIVINE_CONFIGURED_FLAG}: ${DIRECTFB_FLAG} ${DIVINE_CONFIGURATION_FLAG}
	@$(call fn-divine-configure)

##
#
# Dependency target not to be called directly.
#
${DIVINE_COMPILED_FLAG}: ${DIVINE_CONFIGURED_FLAG}
	@$(call fn-divine-compile)

##
#
# Dependency target not to be called directly.
#
${DIVINE_INSTALLED_FLAG}: ${DIVINE_COMPILED_FLAG}
	@$(call fn-divine-install)

##
#
# Dependency target not to be called directly.
#
${DIVINE_FLAG}: ${DIVINE_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${DIVINE_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${DIVINE_PKG_CONFIG} > ${DIVINE_SRC_DIR}/.install.$$$$; \
	if [ -e "${DIVINE_FLAG}" ]; then \
		if ($(DIFF) ${DIVINE_FLAG} ${DIVINE_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIVINE_SRC_DIR}/.install.$$$$ ${DIVINE_FLAG}; \
		fi; \
	else \
		$(CP) ${DIVINE_SRC_DIR}/.install.$$$$ ${DIVINE_FLAG}; \
	fi; \
	$(RM) ${DIVINE_SRC_DIR}/.install.$$$$

