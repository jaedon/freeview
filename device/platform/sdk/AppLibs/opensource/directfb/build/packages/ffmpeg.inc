

.PHONY: ffmpeg-all ffmpeg-source ffmpeg-check-config ffmpeg-config ffmpeg ffmpeg-compile ffmpeg-install ffmpeg-uninstall
.PHONY: ffmpeg-clean ffmpeg-distclean ffmpeg-mrproper

############################################################
#
# ffmpeg-all: force FFmpeg to be configured and compiled.
#
ffmpeg-all: ffmpeg-config ffmpeg-compile ffmpeg-install

##
#
# ffmpeg-source: unpack the FFmpeg source code.
#
ffmpeg-source: check ${FFMPEG_UNPACKED_FLAG}

##
#
# ffmpeg-check-config: check to see whether the build environment or build options have changed
#                     since the last build.  If they have, then this will force a clean and
#                     then a rebuild of FFmpeg.
#
ffmpeg-check-config: ffmpeg-source ${FFMPEG_CONFIGURATION_FLAG}

##
#
# ffmpeg-autogen: force configure and autoconf .in files to be regenerated.
#
ffmpeg-autogen: check-autogen-tools ffmpeg-check-config
	@$(ECHO) "********************************************"; \
	$(ECHO)  "* Autogenerating FFmpeg-${FFMPEG_VERSION} Configuration *"; \
	$(ECHO)  "********************************************"; \
	if [ -e "${FFMPEG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${FFMPEG_CONFIGURED_FLAG}; \
	fi; \
	$(CD) ${FFMPEG_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) CXXFLAGS="${BRCM_CPPFLAGS}"; \
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
	./configure $(FFMPEG_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n***************************************************"; \
	    $(ECHO) "* ERROR: Failed to autogenerate FFmpeg - Aborting! *"; \
		$(ECHO) "***************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${FFMPEG_CONFIGURED_FLAG}; \
	fi

##
#
# ffmpeg-config: force FFmpeg to be configured.
#
ffmpeg-config: ffmpeg-check-config
	@if [ -e "${FFMPEG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${FFMPEG_CONFIGURED_FLAG}; \
	fi
	@$(call fn-ffmpeg-configure)

##
#
# ffmpeg-compile: force FFmpeg to be compiled (tracks changes to source code files)
#
ffmpeg-compile: ${FFMPEG_CONFIGURED_FLAG}
	@if [ -e "${FFMPEG_COMPILED_FLAG}" ]; then \
		$(RM) ${FFMPEG_COMPILED_FLAG}; \
	fi
	@$(call fn-ffmpeg-compile)

##
#
# ffmpeg-install: force FFmpeg to be installed (tracks changes to source code files)
#
ffmpeg-install: check ${FFMPEG_COMPILED_FLAG}
	@if [ -e "${FFMPEG_INSTALLED_FLAG}" ]; then \
		$(RM) ${FFMPEG_INSTALLED_FLAG}; \
	fi
	@$(call fn-ffmpeg-install)

##
#
# ffmpeg: force FFmpeg to be compiled and installed in one pass (tracks changes to source code files)
#
ffmpeg: ffmpeg-install

##
#
# ffmpeg-uninstall: force FFmpeg to be uninstalled
#
ffmpeg-uninstall:
	@if [ -e "${FFMPEG_INSTALLED_FLAG}" ]; then \
		$(RM) ${FFMPEG_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${FFMPEG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${FFMPEG_SRC_DIR} uninstall; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavdevice.a" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavdevice.*a; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavdevice.so" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavdevice.so*; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavformat.a" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavformat.*a; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavformat.so" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavformat.so*; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavutil.a" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavutil.*a; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavutil.so" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavutil.so*; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libswscale.a" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libswscale.*a; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libswscale.so" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libswscale.so*; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavcodec.a" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavcodec.*a; \
	fi
	@if [ -e "${FFMPEG_INSTALL_LIB_DIR}/libavcodec.so" ]; then \
		$(RM) ${FFMPEG_INSTALL_LIB_DIR}/libavcodec.so*; \
	fi

##
#
# ffmpeg-clean: remove all intermediate object and dependency files.
#
ffmpeg-clean: ffmpeg-uninstall
	@if [ -e "${FFMPEG_COMPILED_FLAG}" ]; then \
		$(RM) ${FFMPEG_COMPILED_FLAG}; \
	fi
	@if [ -e "${FFMPEG_FLAG}" ]; then \
		$(RM) ${FFMPEG_FLAG}; \
	fi
	@-if [ -e "${FFMPEG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${FFMPEG_SRC_DIR} clean; \
	fi

##
#
# ffmpeg-distclean: remove all FFmpeg files including the option of
#                  removing the complete source tree.
#
ffmpeg-distclean: ffmpeg-clean
	@-$(call fn-ffmpeg-distclean)
	@if [ -d "${FFMPEG_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${FFMPEG_SRC_DIR} completely."; \
		$(ECHO) "Removing ${FFMPEG_SRC_DIR}..."; \
		$(RM) -r ${FFMPEG_SRC_DIR}; \
	fi

##
#
# ffmpeg-mrproper: remove all FFmpeg files including the
#                 complete source tree.
#
ffmpeg-mrproper: ffmpeg-clean
	@-$(call fn-ffmpeg-distclean)
	@if [ -d "${FFMPEG_SRC_DIR}" ]; then \
		$(RM) -r ${FFMPEG_SRC_DIR}; \
	fi

##
#
# fn-ffmpeg-configure: function to configure FFmpeg.
#
define fn-ffmpeg-configure
	$(ECHO) "***************************"; \
	$(ECHO) "* Configuring FFmpeg-${FFMPEG_VERSION} *"; \
	$(ECHO) "***************************"; \
	$(CD) ${FFMPEG_SRC_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) CXXFLAGS="${BRCM_CPPFLAGS}"; \
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
	./configure $(FFMPEG_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n************************************************"; \
	    $(ECHO) "* ERROR: Failed to configure FFmpeg - Aborting! *"; \
		$(ECHO) "************************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${FFMPEG_CONFIGURED_FLAG}; \
	fi
endef

##
#
# fn-ffmpeg-compile: function to compile FFmpeg.
#
define fn-ffmpeg-compile
	$(ECHO) "************************"; \
	$(ECHO) "* Building FFmpeg-${FFMPEG_VERSION} *"; \
	$(ECHO) "************************"; \
	$(CD) ${FFMPEG_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${FFMPEG_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n********************************************"; \
	    $(ECHO) "* ERROR: Failed to build FFmpeg - Aborting! *"; \
		$(ECHO) "********************************************"; \
	    $(FALSE); \
	else \
		$(TOUCH) ${FFMPEG_COMPILED_FLAG}; \
	fi
endef

##
#
# fn-ffmpeg-install: function to install FFmpeg.
#
define fn-ffmpeg-install
	$(ECHO) "**************************"; \
	$(ECHO) "* Installing FFmpeg-${FFMPEG_VERSION} *"; \
	$(ECHO) "**************************"; \
	$(CD) ${FFMPEG_SRC_DIR}; \
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
	${MAKE} ${MAKE_OPTIONS} install ; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n**********************************************"; \
	    $(ECHO) "* ERROR: Failed to install FFmpeg - Aborting! *"; \
		$(ECHO) "**********************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${FFMPEG_VERSION}"  > ${FFMPEG_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${FFMPEG_SRC_DIR}" >> ${FFMPEG_INSTALLED_FLAG}; \
	fi
endef

##
#
# fn-ffmpeg-distclean: function to clean out all files.
#
define fn-ffmpeg-distclean
	if [ -e "${FFMPEG_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${FFMPEG_CONFIGURATION_FLAG}; \
	fi; \
	if [ -e "${FFMPEG_CONFIGURED_FLAG}" ]; then \
		$(RM) ${FFMPEG_CONFIGURED_FLAG}; \
	fi; \
	if [ -e "${FFMPEG_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${FFMPEG_SRC_DIR} distclean; \
	fi; \
	if [ -e "${FFMPEG_INSTALL_INCLUDE_DIR}/ffmpeg" ]; then \
		$(RM) -r ${FFMPEG_INSTALL_INCLUDE_DIR}/ffmpeg; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${FFMPEG_UNPACKED_FLAG}:
	@if ! [ -e "${FFMPEG_SRC_DIR}" ]; then \
	$(CD) ${FFMPEG_TOP}; ${MKDIR} "${FFMPEG_SRC_DIR}"; \
	fi
	@if ! [ -e "${FFMPEG_SOURCE_TARBALL}" ]; then \
		$(ECHO) "ERROR: Cannot find '${FFMPEG_SOURCE_TARBALL}'!";\
		$(FALSE);\
	else \
		if [ `$(STAT) -c "%s" ${FFMPEG_SOURCE_TARBALL}` -eq 0 ]; then \
			$(ECHO) "ERROR: Size of '${FFMPEG_SOURCE_TARBALL}' is 0!";\
			$(FALSE);\
		else \
			$(ECHO) "*******************************"; \
			$(ECHO) "* Creating FFmpeg-${FFMPEG_VERSION} Source *"; \
			$(ECHO) "*******************************"; \
			$(CD) ${FFMPEG_TOP}; \
			$(TAR) xjkf ${FFMPEG_SOURCE_TARBALL}; \
			$(TOUCH) ${FFMPEG_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${FFMPEG_CONFIGURATION_FLAG}: ${FFMPEG_UNPACKED_FLAG} ALWAYS
	@$(ECHO) -n "GCC_VERSION="                        > ${FFMPEG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) -n "GCC_MACHINE="                        >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "B_REFSW_SHAREABLE=${B_REFSW_SHAREABLE}" >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "DIRECTFB_SHARED=${DIRECTFB_SHARED}"     >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_VOODOO=${BUILD_VOODOO}"           >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	$(ECHO) "BUILD_PPDFB=${BUILD_PPDFB}"             >> ${FFMPEG_SRC_DIR}/.config.$$$$; \
	if [ -e "${FFMPEG_CONFIGURATION_FLAG}" ]; then \
		if ($(DIFF) ${FFMPEG_CONFIGURATION_FLAG} ${FFMPEG_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
			${MAKE} -C ${DIRECTFB_BUILD_VER_DIR} ffmpeg-clean; \
			$(CP) ${FFMPEG_SRC_DIR}/.config.$$$$ ${FFMPEG_CONFIGURATION_FLAG}; \
		fi; \
	else \
		$(CP) ${FFMPEG_SRC_DIR}/.config.$$$$ ${FFMPEG_CONFIGURATION_FLAG}; \
	fi; \
	$(RM) ${FFMPEG_SRC_DIR}/.config.$$$$

##
#
# Dependency target not to be called directly.
#
${FFMPEG_CONFIGURED_FLAG}: ${FFMPEG_CONFIGURATION_FLAG}
	@$(call fn-ffmpeg-configure)

##
#
# Dependency target not to be called directly.
#
${FFMPEG_COMPILED_FLAG}: ${FFMPEG_CONFIGURED_FLAG}
	@$(call fn-ffmpeg-compile)

##
#
# Dependency target not to be called directly.
#
${FFMPEG_INSTALLED_FLAG}: ${FFMPEG_COMPILED_FLAG}
	@$(call fn-ffmpeg-install)

##
#
# Dependency target not to be called directly.
#
${FFMPEG_FLAG}: ${FFMPEG_INSTALLED_FLAG}
	@$(ECHO) -e "${FFMPEG_VERSION}\n${BRCM_CPPFLAGS}" > ${FFMPEG_SRC_DIR}/.install.$$$$; \
	if [ -e "${FFMPEG_FLAG}" ]; then \
		if ($(DIFF) ${FFMPEG_FLAG} ${FFMPEG_SRC_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${FFMPEG_SRC_DIR}/.install.$$$$ ${FFMPEG_FLAG}; \
		fi; \
	else \
		$(CP) ${FFMPEG_SRC_DIR}/.install.$$$$ ${FFMPEG_FLAG}; \
	fi; \
	$(RM) ${FFMPEG_SRC_DIR}/.install.$$$$

