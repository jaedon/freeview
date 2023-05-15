
.PHONY: fusion-all fusion-check-config fusion fusion-compile fusion-install fusion-uninstall fusion-clean fusion-distclean fusion-mrproper


############################################################
#
# fusion-all: force fusion to be built and installed.
#
fusion-all: fusion-check-config fusion-compile fusion-install

##
#
# fusion-source: unpack the linux-fusion source code.
#
fusion-source: check ${FUSION_UNPACKED_FLAG}

##
#
# fusion-check-config: check to see whether the build environment or build options have changed
#                      since the last build.  If they have, then this will force a clean and
#                      then a rebuild of fusion.
#
fusion-check-config: fusion-source ${FUSION_CONFIGURATION_FLAG}

##
#
# fusion: force fusion to be compiled
#
fusion-compile: ${FUSION_CONFIGURATION_FLAG}
	@if [ -e "${FUSION_COMPILED_FLAG}" ]; then \
		$(RM) ${FUSION_COMPILED_FLAG}; \
	fi
	@$(call fn-fusion-build)

##
#
# fusion-install: force an install of fusion files
#
fusion-install: ${FUSION_COMPILED_FLAG}
	@if [ -e "${FUSION_INSTALLED_FLAG}" ]; then \
		$(RM) ${FUSION_INSTALLED_FLAG}; \
	fi
	@$(call fn-fusion-install)

##
#
# fusion: force fusion to be compiled and installed in one pass
#
fusion: fusion-compile fusion-install

##
#
# fusion-uninstall: remove installation files
#
fusion-uninstall:
	@if [ -e "${FUSION_INSTALLED_FLAG}" ]; then \
		$(RM) ${FUSION_INSTALLED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_MODULES_DIR}/fusion.ko" -o -e "${DIRECTFB_INSTALL_LIB_MODULES_DIR}/fusion.o" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_MODULES_DIR}/fusion.?o; \
	fi

##
#
# fusion-clean: remove all intermediate object and dependency files as well as the destination dir.
#
fusion-clean: fusion-uninstall
	@if [ -e "${FUSION_COMPILED_FLAG}" ]; then \
		$(RM) ${FUSION_COMPILED_FLAG}; \
	fi
	@if [ -e "${FUSION_FLAG}" ]; then \
		$(RM) ${FUSION_FLAG}; \
	fi
	@-if [ -e "${FUSION_SRC_DIR}/Makefile" ]; then \
		${MAKE} -C ${FUSION_SRC_DIR} DESTDIR=${FUSION_INSTALL_DIR} clean; \
	fi

##
#
# fusion-distclean: do the same as for fusion-clean target
#                   with the user prompt.
#
fusion-distclean: fusion-clean
	@if [ -e "${FUSION_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${FUSION_CONFIGURATION_FLAG}; \
	fi
	@-if [ -d "${FUSION_SRC_DIR}" ]; then \
		$(ECHO) "WARNING: This will remove ${FUSION_SRC_DIR} completely."; \
		$(ECHO) "Removing ${FUSION_SRC_DIR}..."; \
		$(RM) -r ${FUSION_SRC_DIR}; \
		$(RMDIR) $(dir ${FUSION_SRC_DIR}); \
	fi

##
#
# fusion-mrproper: do the same as for fusion-clean target.
#
fusion-mrproper: fusion-clean
	@if [ -e "${FUSION_CONFIGURATION_FLAG}" ]; then \
		$(RM) ${FUSION_CONFIGURATION_FLAG}; \
	fi
	@-if [ -d "${FUSION_SRC_DIR}" ]; then \
		$(RM) -r ${FUSION_SRC_DIR}; \
		$(RMDIR) $(dir ${FUSION_SRC_DIR}); \
	fi


##
#
# fn-fusion-compile: compile fusion only if BUILD_FUSION=y
#
define fn-fusion-build
	if [ "${BUILD_FUSION}" = "y" ]; then \
		$(ECHO) "*********************************************"; \
		$(ECHO) "* Building linux-fusion-${FUSION_VERSION} kernel module *"; \
		$(ECHO) "*********************************************"; \
		if [ "${DIRECTFB_NETFLIX_SUPPORT}" = "y" ]; then \
		${MAKE} ${MAKE_OPTIONS} -C ${FUSION_SRC_DIR} ARCH=$(ARCH) FUSION_CPPFLAGS="-DCONFIG_NETFLIX_SUPPORT" KERNELDIR=${LINUX} INSTALL_MOD_PATH=${FUSION_INSTALL_DIR} -r install; \
		else \
		${MAKE} ${MAKE_OPTIONS} -C ${FUSION_SRC_DIR} ARCH=$(ARCH) KERNELDIR=${LINUX} INSTALL_MOD_PATH=${FUSION_INSTALL_DIR} -r install; \
		fi ;\
		if [ $$? -ne 0 ]; then \
			$(ECHO) "***********************************************************"; \
			$(ECHO) "* ERROR: Failed to build linux-fusion kernel module - Aborting! *"; \
			$(ECHO) "***********************************************************"; \
			$(FALSE); \
		else \
			$(TOUCH) ${FUSION_COMPILED_FLAG}; \
		fi; \
	fi
endef

define fn-fusion-install
	if [ "${BUILD_FUSION}" = "y" ]; then \
		$(ECHO) "***********************************************"; \
		$(ECHO) "* Installing linux-fusion-${FUSION_VERSION} kernel module *"; \
		$(ECHO) "***********************************************"; \
		if [ -e "${FUSION_INSTALL_DIR}/lib/modules/${KERNEL_VERSION}/drivers/char/fusion/fusion.ko" -o -e "${FUSION_INSTALL_DIR}/fusion.o" ]; then \
			if [ ! -d "${DIRECTFB_INSTALL_LIB_MODULES_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_INSTALL_LIB_MODULES_DIR}; \
			fi; \
			if [ ! -d "${DIRECTFB_INSTALL_LIB_MODULES_DIR}" ]; then \
				$(ECHO) "ERROR: Could not create \"${DIRECTFB_INSTALL_LIB_MODULES_DIR}\" directory!"; \
				$(FALSE); \
			else \
				if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}" ]; then \
					$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}; \
				fi; \
				if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}" ]; then \
					$(ECHO) "ERROR: Could not create \"${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}\" directory!"; \
					$(FALSE); \
				else \
					$(CP) ${FUSION_INSTALL_DIR}/lib/modules/${KERNEL_VERSION}/drivers/char/fusion/fusion.?o ${DIRECTFB_INSTALL_LIB_MODULES_DIR}; \
					$(CP) ${FUSION_INSTALL_DIR}/lib/modules/${KERNEL_VERSION}/drivers/char/fusion/fusion.?o ${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}; \
					if [ -d "${FUSION_INSTALL_INCLUDE_DIR}" ]; then \
						if [ ! -d "${DIRECTFB_PRE_INSTALL_DIR}" ]; then \
							$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_DIR}; \
						fi; \
						if [ ! -d "${DIRECTFB_PRE_INSTALL_DIR}" ]; then \
							$(ECHO) "ERROR: Could not create \"${DIRECTFB_PRE_INSTALL_DIR}\" directory!"; \
							$(FALSE); \
						else \
							$(CP) -r ${FUSION_INSTALL_INCLUDE_DIR} ${DIRECTFB_PRE_INSTALL_DIR}; \
						fi; \
						$(CP) -r ${FUSION_INSTALL_INCLUDE_DIR}/../usr/include/linux ${FUSION_INSTALL_INCLUDE_DIR}; \
					fi; \
					$(ECHO) "Ver: ${FUSION_VERSION}"  > ${FUSION_INSTALLED_FLAG}; \
					$(ECHO) "Src: ${FUSION_SRC_DIR}" >> ${FUSION_INSTALLED_FLAG}; \
				fi; \
			fi; \
		else \
			$(ECHO) "ERROR: Fusion kernel module not found in ${FUSION_INSTALL_DIR}!"; \
			$(FALSE); \
		fi; \
	fi
endef

##
#
# Dependency target not to be called directly.
#
${FUSION_UNPACKED_FLAG}:
	@if [ "${BUILD_FUSION}" = "y" ]; then \
		if ! [ -d "${FUSION_SRC_DIR}" ]; then \
			${MKDIR} -p "${FUSION_SRC_DIR}"; \
		fi; \
		if ! [ -e "${DIRECTFB_SRC_DFB_TAR_DIR}/${FUSION_SOURCE_TARBALL}" ]; then \
			$(ECHO) "ERROR: Cannot find '${DIRECTFB_SRC_DFB_TAR_DIR}/${FUSION_SOURCE_TARBALL}'!";\
			$(FALSE);\
		else \
			$(ECHO) "**************************************"; \
			if [ "${DIRECTFB_NETFLIX_SUPPORT}" = "y" ]; then \
			$(ECHO) "* Creating linux-fusion-${FUSION_VERSION} Source for netflix  with upsized FUSION_MAX_PACKET_SIZE (32768) by HUMAX*"; \
			else \
			$(ECHO) "* Creating linux-fusion-${FUSION_VERSION} Source*"; \
			fi; \
			$(ECHO) "**************************************"; \
			$(TAR) xzkf $(DIRECTFB_SRC_DFB_TAR_DIR)/${FUSION_SOURCE_TARBALL} -C $(dir ${FUSION_SRC_DIR}); \
			if [ -d "${FUSION_PUB_FUS_VER1_DIR}" ]; then \
				$(CP) -r ${FUSION_PUB_FUS_VER1_DIR}/* ${FUSION_SRC_DIR}; \
			fi; \
			if [ -d "${FUSION_PUB_FUS_VER2_DIR}" ]; then \
				$(CP) -r ${FUSION_PUB_FUS_VER2_DIR}/* ${FUSION_SRC_DIR}; \
			fi; \
			if [ -d "${FUSION_PRI_FUS_VER1_DIR}" ]; then \
				$(CP) -r ${FUSION_PRI_FUS_VER1_DIR}/* ${FUSION_SRC_DIR}; \
			fi; \
			if [ -d "${FUSION_PRI_FUS_VER2_DIR}" ]; then \
				$(CP) -r ${FUSION_PRI_FUS_VER2_DIR}/* ${FUSION_SRC_DIR}; \
			fi; \
			$(TOUCH) ${FUSION_UNPACKED_FLAG}; \
		fi; \
	fi

##
#
# Dependency target not to be called directly.
#
${FUSION_CONFIGURATION_FLAG}: ${FUSION_UNPACKED_FLAG} ALWAYS
	@if [ "${BUILD_FUSION}" = "y" ]; then \
		$(ECHO) -n "GCC_VERSION="                        > ${FUSION_SRC_DIR}/.config.$$$$; \
		${B_REFSW_CROSS_COMPILE}gcc -dumpversion         >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) -n "GCC_MACHINE="                        >> ${FUSION_SRC_DIR}/.config.$$$$; \
		${B_REFSW_CROSS_COMPILE}gcc -dumpmachine         >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "KERNEL_VERSION=${KERNEL_VERSION}"       >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "NEXUS_PLATFORM=${NEXUS_PLATFORM}"       >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "BCHP_VER=${BCHP_VER}"                   >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "SMP=${SMP}"                             >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "B_REFSW_DEBUG=${B_REFSW_DEBUG}"         >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "B_REFSW_ARCH=${B_REFSW_ARCH}"           >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "B_REFSW_VERBOSE=${B_REFSW_VERBOSE}"     >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "DIRECTFB_PREFIX=${DIRECTFB_PREFIX}"     >> ${FUSION_SRC_DIR}/.config.$$$$; \
		$(ECHO) "DIRECTFB_MULTI=${DIRECTFB_MULTI}"       >> ${FUSION_SRC_DIR}/.config.$$$$; \
		if [ -e "${FUSION_CONFIGURATION_FLAG}" ]; then \
			if ($(DIFF) ${FUSION_CONFIGURATION_FLAG} ${FUSION_SRC_DIR}/.config.$$$$ | $(GREP) -q differ); then \
				$(CP) ${FUSION_SRC_DIR}/.config.$$$$ ${FUSION_CONFIGURATION_FLAG}; \
			fi; \
		else \
			$(CP) ${FUSION_SRC_DIR}/.config.$$$$ ${FUSION_CONFIGURATION_FLAG}; \
		fi; \
		$(RM) ${FUSION_SRC_DIR}/.config.$$$$; \
	fi

##
#
# Dependency target not to be called directly.
#
${FUSION_COMPILED_FLAG}: ${FUSION_CONFIGURATION_FLAG}
	@$(call fn-fusion-build)

##
#
# Dependency target not to be called directly.
#
${FUSION_INSTALLED_FLAG}: ${FUSION_COMPILED_FLAG}
	@$(call fn-fusion-install)

##
#
# Dependency target not to be called directly.
#
${FUSION_FLAG}: ${FUSION_INSTALLED_FLAG}
	@if [ ! -d ${FUSION_INSTALL_DIR} ]; then \
		$(MKDIR) -p ${FUSION_INSTALL_DIR}; \
	fi; \
	$(ECHO) -e "${FUSION_VERSION}" > ${FUSION_FLAG}.$$$$; \
	if [ -e "${FUSION_FLAG}" ]; then \
		if ($(DIFF) ${FUSION_FLAG} ${FUSION_FLAG}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${FUSION_FLAG}.$$$$ ${FUSION_FLAG}; \
		fi; \
	else \
		$(CP) ${FUSION_FLAG}.$$$$ ${FUSION_FLAG}; \
	fi; \
	$(RM) ${FUSION_FLAG}.$$$$


