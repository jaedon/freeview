
.PHONY: directfb-brcm-all directfb-brcm-defines directfb-brcm-config directfb-brcm directfb-brcm-compile
.PHONY: directfb-brcm-install directfb-brcm-post-install directfb-brcm-uninstall directfb-brcm-clean directfb-brcm-distclean directfb-brcm-mrproper


directfb-brcm-all: directfb-brcm-config directfb-brcm-compile directfb-brcm-install directfb-brcm-post-install

directfb-brcm-config: 
	@if [ -e "${DIRECTFB_BRCM_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_CONFIGURED_FLAG}; \
	fi
	@$(call fn-directfb-brcm-configure)


directfb-brcm-compile: ${DIRECTFB_BRCM_CONFIGURED_FLAG}
	@if [ -e "${DIRECTFB_BRCM_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_COMPILED_FLAG}; \
	fi
	@$(call fn-directfb-brcm-compile)

directfb-brcm-install: ${DIRECTFB_BRCM_COMPILED_FLAG}
	@if [ -e "${DIRECTFB_BRCM_INSTALLED_FLAG}" ]; then \
	         $(RM) ${DIRECTFB_BRCM_INSTALLED_FLAG}; \
	fi; \
	@$(call fn-directfb-brcm-install)

directfb-brcm-post-install: ${DIRECTFB_BRCM_INSTALLED_FLAG}
	@$(INSTALL) -m 775 ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/${NEXUS_SCRIPT} ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.974xx; \
	$(SED) -i -e "s;MODULES_DIR;${DIRECTFB_TARGET_MODULES_DIR};g;s;BIN_DIR;${DIRECTFB_TARGET_BIN_DFB_VER_DIR};g" \
	${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.974xx; \
	$(INSTALL) -m 775 ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/nexus_dfb ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus_dfb; \
	$(SED) -i -e "s;LIB_DIR;${DIRECTFB_TARGET_LIB_DIR};g;s;BIN_DIR;${DIRECTFB_TARGET_BIN_DFB_VER_DIR};g;s;HOTPLUG_DIR;${DIRECTFB_PREFIX}/etc/hotplug.d;g" \
	${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus_dfb; \
	$(INSTALL) -m 775 ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/rundfb.sh ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}; \
	$(SED) -i -e "s;nexus;nexus_dfb;" ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh; \
	$(SED) -i -e "s;PREFIX;${DIRECTFB_PREFIX};g;s;BIN_DIR;${DIRECTFB_TARGET_BIN_DFB_VER_DIR};g" ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh; \
	$(INSTALL) -m 775 ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/killer.sh ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}; \
	$(INSTALL) -m 775 ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/nexus.install ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}; \
	$(INSTALL) -m 775 -D ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/dfbhotplug ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfbhotplug; \
	$(SED) -i -e "s;HOTPLUG_DIR;${DIRECTFB_PREFIX}/etc/hotplug.d;g" ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfbhotplug; \
	if [ -e ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfb_input_hotplug ]; then \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfb_input_hotplug ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}; \
	fi; \
        $(INSTALL) -m 664 ${DIRECTFB_BRCM_SRC_DFB_DIR}/dfb_platform.h ${DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR}; \
	if [ -e ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/directfbrc ]; then \
        	$(INSTALL) -m 664 -D ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/directfbrc ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
        fi; \
        $(ECHO) "res=720p" >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc;

ifeq ($(BUILD_SAWMAN),y)
	@if [ -e ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc ]; then \
		$(ECHO) "wm=sawman" >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
	fi
endif
ifeq ($(BUILD_FUSION),y)
	@$(ECHO) "if [ ! -e /dev/fusion0 ]" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
    $(ECHO)  "then" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	$(ECHO)  "  mknod -m a=rw /dev/fusion0 c 250 0" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	$(ECHO)  "fi" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	$(ECHO)  "if [ ! -e /dev/fusion1 ]" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
    $(ECHO)  "then" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	$(ECHO)  "  mknod -m a=rw /dev/fusion1 c 250 1" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	$(ECHO)  "fi" >> ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install
endif

ifeq ($(DIRECTFB_IR_INPUT),y)
	@$(ECHO) "bcmnexus-ir-protocol=${DIRECTFB_IR_PROTOCOL}" >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
	$(ECHO)  "bcmnexus-ir-keycodes=${DIRECTFB_IR_KEYCODES}" >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc
endif

ifeq ($(DIRECTFB_NSC_SUPPORT),y)
	@if [ -e ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc ]; then \
                $(ECHO) sw_picture_decode >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
        fi
endif

ifeq ($(DIRECTFB_NIR_SUPPORT),y)
	@if [ -e ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc ]; then \
		$(ECHO) disable-module=linux_input >> ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
	fi
endif

	@if [ ! -d "${DIRECTFB_PRE_INSTALL_DIR}" ]; then \
		$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_DIR}; \
	fi; \
	if [ ! -d "${DIRECTFB_PRE_INSTALL_DIR}" ]; then \
		$(ECHO) "ERROR: Could not create \"${DIRECTFB_PRE_INSTALL_DIR}\" directory!"; \
		$(FALSE); \
	else \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_ETC_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_ETC_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_INCLUDE_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_INCLUDE_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_DFB_GFX_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_DFB_GFX_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_DFB_SYS_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_DFB_SYS_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_DFB_INP_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_DFB_INP_DIR}; \
		fi; \
		if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_DFB_IMG_DIR}" ]; then \
			$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_DFB_IMG_DIR}; \
		fi; \
		if [ "${DIRECTFB_MASTER_LIB}" = "y" -a "${DIRECTFB_CLIENT_LIB}" = "y" ]; then \
			if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}; \
			fi; \
			if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_GFX_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_GFX_DIR}; \
			fi; \
			if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_SYS_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_SYS_DIR}; \
			fi; \
			if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_INP_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_INP_DIR}; \
			fi; \
			if [ ! -d "${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_IMG_DIR}" ]; then \
				$(MKDIR) -p ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_IMG_DIR}; \
			fi; \
		fi; \
		$(INSTALL) -m 775 ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.974xx ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}/${NEXUS_SCRIPT}; \
		$(CP) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/*.sh ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
		$(CP) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus_dfb ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
		$(CP) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
		if [ "${DIRECTFB_GLES_SUPPORT}" = "y" -a "${BUILD_TESTS}" = "y" ]; then \
			$(CP) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfbtest_egl* ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
			$(CP) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/dfbtest_gl* ${DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR}; \
		fi; \
		$(CP) -r ${DIRECTFB_INSTALL_ETC_DIR} ${DIRECTFB_PRE_INSTALL_DIR}; \
		$(CP) ${DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR}/dfb_platform.h ${DIRECTFB_PRE_INSTALL_INCLUDE_DIR}; \
		if [ "${DIRECTFB_MASTER_LIB}" = "y" ]; then \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_DFB_GFX_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_DFB_GFX_DIR}; \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_DFB_SYS_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_DFB_SYS_DIR}; \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_DFB_INP_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_DFB_INP_DIR}; \
			if [ "${DIRECTFB_SID}" = "y" ]; then \
				$(CP) -dpr ${DIRECTFB_INSTALL_LIB_DFB_IMG_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_DFB_IMG_DIR}; \
			fi; \
		fi; \
		if [ "${DIRECTFB_MASTER_LIB}" = "y" -a "${DIRECTFB_CLIENT_LIB}" = "y" ]; then \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_GFX_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_GFX_DIR}; \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_SYS_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_SYS_DIR}; \
			$(CP) -dpr ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_INP_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_INP_DIR}; \
			if [ "${DIRECTFB_SID}" = "y" ]; then \
				$(CP) -dpr ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_IMG_DIR}/*nexus* ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_IMG_DIR}; \
			fi; \
		fi; \
		if [ -e ${DIRECTFB_INSTALL_LIB_DIR}/libdbpl.so ]; then \
			$(CP) ${DIRECTFB_INSTALL_LIB_DIR}/libdbpl.so ${DIRECTFB_PRE_INSTALL_LIB_DIR}; \
		fi; \
		if [ -e ${DIRECTFB_INSTALL_LIB_DIR}/libv3ddriver.so ]; then \
			$(CP) ${DIRECTFB_INSTALL_LIB_DIR}/libv3ddriver.so ${DIRECTFB_PRE_INSTALL_LIB_DIR}; \
		fi; \
	fi



directfb-brcm-uninstall:
	@if [ -e "${DIRECTFB_BRCM_POST_INSTALLED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_POST_INSTALLED_FLAG}; \
	fi
	@-if [ -e "${DIRECTFB_BRCM_SRC_DFB_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_BRCM_SRC_DFB_DIR} uninstall DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install" ]; then \
		$(RM) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.install; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh" ]; then \
		$(RM) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/rundfb.sh; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/killer.sh" ]; then \
		$(RM) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/killer.sh; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus_dfb" ]; then \
		$(RM) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus_dfb; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.974xx" ]; then \
		$(RM) ${DIRECTFB_INSTALL_BIN_DFB_VER_DIR}/nexus.974xx; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_ETC_DIR}/directfbrc" ]; then \
		$(RM) ${DIRECTFB_INSTALL_ETC_DIR}/directfbrc; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfbhotplug" ]; then \
		$(RM) ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfbhotplug; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfb_input_hotplug" ]; then \
		$(RM) ${DIRECTFB_INSTALL_ETC_HOTPLUG_DIR}/dfb_input_hotplug; \
	fi

directfb-brcm-clean: directfb-brcm-uninstall
	@if [ -e "${DIRECTFB_BRCM_COMPILED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_COMPILED_FLAG}; \
	fi	
	@if [ -e "${DIRECTFB_BRCM_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_FLAG}; \
	fi	
	@-if [ -e "${DIRECTFB_BRCM_SRC_DFB_DIR}/Makefile" ]; then \
		${MAKE} -C ${DIRECTFB_BRCM_SRC_DFB_DIR} clean; \
	fi
	@-if [ -d "${DIRECTFB_BRCM_SRC_DFB_DIR}/tools/stubs" ]; then \
		$(RM) -r ${DIRECTFB_BRCM_SRC_DFB_DIR}/tools/stubs; \
	fi

directfb-brcm-mrproper: directfb-brcm-clean
	@if [ -e "${DIRECTFB_BRCM_SRC_DFB_DIR}/Makefile" ]; then \
                ${MAKE} -C ${DIRECTFB_BRCM_SRC_DFB_DIR} distclean; \
        fi
	@if [ -e "${DIRECTFB_BRCM_CONFIGURED_FLAG}" ]; then \
		$(RM) ${DIRECTFB_BRCM_CONFIGURED_FLAG}; \
	fi
	@if [ -e "${DIRECTFB_BRCM_DIR}/Makefile" ]; then \
		find ${DIRECTFB_BRCM_DIR} -name "Makefile" -exec rm {} \; ;\
	fi


define fn-directfb-brcm-configure
	$(ECHO) "*************************************"; \
	$(ECHO) "* Configuring BRCM DirectFB Package *"; \
	$(ECHO) "*************************************"; \
	$(CD) ${DIRECTFB_BRCM_SRC_DFB_DIR}; \
	$(EXPORT) CPPFLAGS="${BRCM_CPPFLAGS}"; \
	$(EXPORT) LDFLAGS="${LDFLAGS} ${BRCM_LDFLAGS}"; \
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
	$(EXPORT) CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) DFB_EXTRA_CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) DFB_EXTRA_LIBS="${BRCM_LIBS}"; \
	$(EXPORT) NEXUS_CLIENT_LIB_SUFFIX="${NEXUS_CLIENT_LIB_SUFFIX}"; \
	$(EXPORT) PKG_CONFIG_LIBDIR=${DFB_FREETYPE_INSTALL_LIB_DIR}:${PKG_CONFIG_LIBDIR}:${DIRECTFB_INSTALL_LIB_DIR}; \
	$(EXPORT) PKG_CONFIG_PATH=${DFB_FREETYPE_INSTALL_PKG_CONFIG_DIR}:${PKG_CONFIG_PATH}:${DIRECTFB_INSTALL_PKG_CONFIG_DIR}; \
	$(CHMOD) -f u+wx configure; \
	$(CHMOD) -f u+w include/*; \
	./configure $(DIRECTFB_BRCM_CONFIG_FLAGS); \
	if [ $$? -ne 0 ]; then \
		$(ECHO) -e "\n********************************************************"; \
	   	$(ECHO)      "* ERROR: Failed to configure BRCM DirectFB - Aborting! *"; \
		$(ECHO)      "********************************************************"; \
	    $(FALSE); \
	else \
	    if [ ! -d ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/.libs; \
		fi; \
	    if [ ! -d ${DIRECTFB_BRCM_SRC_DFB_DIR}/gfxdrivers/bcmnexus/.libs ]; then \
		    ${MKDIR} ${DIRECTFB_BRCM_SRC_DFB_DIR}/gfxdrivers/bcmnexus/.libs; \
            fi; \
            $(TOUCH) ${DIRECTFB_BRCM_CONFIGURED_FLAG}; \
	fi
endef


define fn-directfb-brcm-compile
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
	$(EXPORT) BRCM_CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) NEXUS_TOP="${NEXUS_TOP}"; \
	$(EXPORT) CPI_OBJECT_DIR="${DIRECTFB_OBJECT_DIR}"; \
	$(EXPORT) DIRECTFB_GFX_3D_DRIV_LIB=${DIRECTFB_GFX_3D_DRIV_LIB}; \
	$(EXPORT) DIRECTFB_PREFIX=${DIRECTFB_PREFIX}; \
	if [ "${DIRECTFB_NSC_SUPPORT}" = "y" ]; then \
        ${MKDIR} ${DIRECTFB_BRCM_SRC_DFB_DIR}/tools/stubs; \
        cd ${DIRECTFB_BRCM_SRC_DFB_DIR}/tools; \
        perl -I $(NEXUS_TOP)/build/tools/common $(NEXUS_TOP)/lib/ipc/build/bipc_build.pl stubs/ipc_stubs_server.h stubs/ipc_stubs_client.h $(NEXUS_TOP)/examples/multiprocess/brc_client.h > /dev/null; \
	fi; \
	if [ "${DIRECTFB_MULTI}" = "y" ]; then \
        ${MKDIR} ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/stubs; \
        cd ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform; \
        perl -I $(NEXUS_TOP)/build/tools/common $(NEXUS_TOP)/lib/ipc/build/bipc_build.pl stubs/ipc_stubs_dfb_server.h stubs/ipc_stubs_dfb_client.h ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/platform_ipc_client.h > /dev/null; \
	fi; \
	if [ -e ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/stubs/ipc_stubs_dfb_client.h ]; then \
		$(SED) -i -e "s;brc_client_destroy;brc_client_destroy_client;g;s;brc_client_create;brc_client_create_client;g;s;brc_client_get_settings;brc_client_get_settings_client;g;\
" ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/stubs/ipc_stubs_dfb_client.h; \
	fi;\
	$(ECHO) "**********************************"; \
	$(ECHO) "* Building BRCM DirectFB Package *"; \
	$(ECHO) "**********************************"; \
	$(CD) ${DIRECTFB_BRCM_SRC_DFB_DIR} && \
	${MAKE} ${MAKE_OPTIONS} DESTDIR=${DIRECTFB_INSTALL_DIR} && \
		if [ $$? -ne 0 ]; then \
			$(ECHO) "****************************************************"; \
			$(ECHO) "* ERROR: Failed to build BRCM DirectFB - Aborting! *"; \
			$(ECHO) "****************************************************"; \
			$(FALSE); \
		else \
			$(TOUCH) ${DIRECTFB_BRCM_COMPILED_FLAG}; \
	fi
endef


define fn-directfb-brcm-install
	$(ECHO) "*****************************"; \
	$(ECHO) "* Installing DirectFB-Broadcom *"; \
	$(ECHO) "*****************************"; \
	$(CD) ${DIRECTFB_BRCM_SRC_DFB_DIR}; \
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
	$(EXPORT) NEXUS_TOP="${NEXUS_TOP}"; \
	$(EXPORT) BRCM_CFLAGS="${BRCM_CFLAGS}"; \
	$(EXPORT) CPI_OBJECT_DIR="${DIRECTFB_OBJECT_DIR}"; \
	${MAKE} ${MAKE_OPTIONS} install DESTDIR=${DIRECTFB_INSTALL_DIR}; \
	if [ $$? -ne 0 ]; then \
		$(ECHO) "*************************************************"; \
	    $(ECHO) "* ERROR: Failed to install DirectFB Broadcom- Aborting! *"; \
		$(ECHO) "*************************************************"; \
	    $(FALSE); \
	else \
		$(ECHO) "Ver: ${DIRECTFB_VERSION}"      > ${DIRECTFB_BRCM_INSTALLED_FLAG}; \
		$(ECHO) "Src: ${DIRECTFB_BRCM_SRC_DFB_DIR}" >> ${DIRECTFB_BRCM_INSTALLED_FLAG}; \
	fi
endef

${DIRECTFB_BRCM_CONFIGURED_FLAG}: ${DIRECTFB_CONFIGURATION_FLAG} ${DIRECTFB_CONFIGURED_FLAG}
	@$(call fn-directfb-brcm-configure)


${DIRECTFB_BRCM_COMPILED_FLAG}: ${DIRECTFB_BRCM_CONFIGURED_FLAG}
	@$(call fn-directfb-brcm-compile)


${DIRECTFB_BRCM_INSTALLED_FLAG}: ${DIRECTFB_BRCM_COMPILED_FLAG}
	@$(call fn-directfb-brcm-install)


${DIRECTFB_BRCM_POST_INSTALLED_FLAG}:  ${DIRECTFB_POST_INSTALLED_FLAG} ${DIRECTFB_BRCM_INSTALLED_FLAG} ${NEXUS_FLAG} directfb-brcm-post-install
	@$(call fn-directfb-brcm-post-install)


${DIRECTFB_BRCM_FLAG}: ${DIRECTFB_BRCM_POST_INSTALLED_FLAG}
	@PKG_CONFIG_PATH=${DIRECTFB_INSTALL_PKG_CONFIG_DIR} pkg-config --modversion --cflags --libs ${DIRECTFB_BRCM_PKG_CONFIG} > \
					 ${DIRECTFB_BRCM_SRC_DFB_DIR}/.install.$$$$; \
	if [ -e "${DIRECTFB_BRCM_FLAG}" ]; then \
		if ($(DIFF) ${DIRECTFB_BRCM_FLAG} ${DIRECTFB_BRCM_SRC_DFB_DIR}/.install.$$$$ | $(GREP) -q differ); then \
			$(CP) ${DIRECTFB_BRCM_SRC_DFB_DIR}/.install.$$$$ ${DIRECTFB_BRCM_FLAG}; \
		fi; \
	else \
		$(CP) ${DIRECTFB_BRCM_SRC_DFB_DIR}/.install.$$$$ ${DIRECTFB_BRCM_FLAG}; \
	fi; \
	$(RM) ${DIRECTFB_BRCM_SRC_DFB_DIR}/.install.$$$$


