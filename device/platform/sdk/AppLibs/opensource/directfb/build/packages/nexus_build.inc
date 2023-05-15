
.PHONY: nexus nexus-install nexus-uninstall

##
#
# nexus: ensure that Nexus has already been built.
#
nexus: nexus-client

ifeq ($(DIRECTFB_MASTER_LIB),y)
	@$(ECHO) "Building master Nexus library  (Nexus mode env: $(NEXUS_MODE))"; 
	@$(MAKE)  ${MAKE_OPTIONS} NEXUS_LIB_SUFFIX= -C ${NEXUS_TOP}/build

	@if [ ! -e "${NEXUS_BIN_DIR}/libnexus.so" -o ! -e "${NEXUS_BIN_DIR}/${NEXUS_DRIVER}" ]; then \
		if [ ! -e "${DIRECTFB_PRE_INSTALL_LIB_DIR}/${NEXUS_LIB}" -o ! -e "${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}/${NEXUS_DRIVER}" ]; then \
			$(ECHO) "ERROR: Nexus driver not built - please build it first!"; \
			$(FALSE); \
		fi; \
	fi
endif


##
#
# nexus-client: create client Nexus library for use as Nexus slave app.
#
nexus-client:
ifeq ($(DIRECTFB_CLIENT_LIB),y)
	@$(ECHO) "Building client Nexus library (Nexus mode env: $(NEXUS_MODE))";
	@$(MAKE) ${MAKE_OPTIONS} ${NEXUS_CLIENT_FLAGS} -C ${NEXUS_TOP}/build
	@if [ ! -e "${NEXUS_BIN_DIR}/libnexus${NEXUS_CLIENT_LIB_SUFFIX}.so" -o ! -e "${NEXUS_BIN_DIR}/${NEXUS_DRIVER}" ]; then \
		$(ECHO) "ERROR: Nexus driver not built - please build it first!"; \
		$(FALSE); \
	fi
endif

##
#
# nexus-install: install necessary nexus files into installation directory.
#
nexus-install: nexus nexus-uninstall
	@$(INSTALL) -m 775 -D ${NEXUS_BIN_DIR}/${NEXUS_DRIVER} ${DIRECTFB_INSTALL_LIB_MODULES_DIR}/${NEXUS_DRIVER}; 
ifeq  ($(DIRECTFB_MASTER_LIB),y)
	@$(INSTALL) -m 775 -D ${NEXUS_BIN_DIR}/libnexus.so ${DIRECTFB_INSTALL_LIB_DIR}/libnexus.so; \
	$(INSTALL) -m 775 -D ${DIRECTFB_INSTALL_LIB_DIR}/libnexus.so ${DIRECTFB_PRE_INSTALL_LIB_DIR}/${NEXUS_LIB};
endif
ifeq  ($(DIRECTFB_CLIENT_LIB),y)
	@$(INSTALL) -m 775 -D ${NEXUS_BIN_DIR}/libnexus${NEXUS_CLIENT_LIB_SUFFIX}.so ${DIRECTFB_INSTALL_LIB_DIR}/libnexus${NEXUS_CLIENT_LIB_SUFFIX}.so; \
	$(INSTALL) -m 775 -D ${DIRECTFB_INSTALL_LIB_MODULES_DIR}/${NEXUS_DRIVER} ${DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR}/${NEXUS_DRIVER}; 
endif


##
#
# nexus-uninstall: uninstall nexus files from the installation directory.
#
nexus-uninstall:

	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libnexus.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libnexus.so; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_DIR}/libnexus${NEXUS_CLIENT_LIB_SUFFIX}.so" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_DIR}/libnexus${NEXUS_CLIENT_LIB_SUFFIX}.so; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_MODULES_DIR}/nexus.ko" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_MODULES_DIR}/nexus.ko; \
	fi
	@if [ -e "${DIRECTFB_INSTALL_LIB_MODULES_DIR}/bcmdriver.ko" ]; then \
		$(RM) ${DIRECTFB_INSTALL_LIB_MODULES_DIR}/bcmdriver.ko; \
	fi
	@if [ -e "${NEXUS_FLAG}" ]; then \
		$(RM) ${NEXUS_FLAG}; \
	fi

##
#
# Dependency target not to be called directly.
#
${NEXUS_FLAG}: check
	@$(ECHO) -e "KERNELMODE=${KERNELMODE}\nNEXUS_MODE=${NEXUS_MODE}" > ${NEXUS_FLAG}.$$$$; \
	if [ -e "${NEXUS_FLAG}" ]; then \
		if ($(DIFF) ${NEXUS_FLAG} ${NEXUS_FLAG}.$$$$ | $(GREP) -q differ); then \
			$(CP) ${NEXUS_FLAG}.$$$$ ${NEXUS_FLAG}; \
		fi; \
	else \
		$(CP) ${NEXUS_FLAG}.$$$$ ${NEXUS_FLAG}; \
	fi; \
	$(RM) ${NEXUS_FLAG}.$$$$

