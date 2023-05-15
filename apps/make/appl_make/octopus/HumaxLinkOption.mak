ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO :=
endif

INTROSPECT_DIR = $(APPLTOP_SRC_DIR)/octopus/appkit_server/introspect-interface
INCLUDE_DIR = $(APPLTOP_SRC_DIR)/octopus/appkit_server/include


introspect-interface:
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_media_airplay.xml   	  --adaptor=$(INCLUDE_DIR)/as_apk_media_airplay_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_builder.xml       --adaptor=$(INCLUDE_DIR)/as_apk_util_builder_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_cas_action.xml         --adaptor=$(INCLUDE_DIR)/as_apk_cas_action_adaptor.h        
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_cas_ci.xml         --adaptor=$(INCLUDE_DIR)/as_apk_cas_ci_adaptor.h 
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_cas_irdeto.xml         --adaptor=$(INCLUDE_DIR)/as_apk_cas_irdeto_adaptor.h        
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_cas_mmi.xml            --adaptor=$(INCLUDE_DIR)/as_apk_cas_mmi_adaptor.h           
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_cas_nagra.xml          --adaptor=$(INCLUDE_DIR)/as_apk_cas_nagra_adaptor.h         
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_datastreamer.xml  --adaptor=$(INCLUDE_DIR)/as_apk_util_datastreamer_adaptor.h 
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_db.xml            --adaptor=$(INCLUDE_DIR)/as_apk_util_db_adaptor.h           
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_dlna_util.xml          --adaptor=$(INCLUDE_DIR)/as_apk_dlna_util_adaptor.h         
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_drmagent.xml      --adaptor=$(INCLUDE_DIR)/as_apk_util_drmagent_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_dsmcc.xml         --adaptor=$(INCLUDE_DIR)/as_apk_util_dsmcc_adaptor.h        
	#./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_event.xml         --adaptor=$(INCLUDE_DIR)/as_apk_util_event_adaptor.h        
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_exe.xml           --adaptor=$(INCLUDE_DIR)/as_apk_util_exe_adaptor.h          
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_extdev.xml        --adaptor=$(INCLUDE_DIR)/as_apk_util_extdev_adaptor.h       
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_base.xml       --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_base_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_browse.xml     --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_browse_adaptor.h    
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_ftpserver.xml  --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_ftpserver_adaptor.h 
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_group.xml      --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_group_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_sambaclient.xml --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_sambaclient_adaptor.h
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_sambaserver.xml --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_sambaserver_adaptor.h
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_filemgr_storage.xml    --adaptor=$(INCLUDE_DIR)/as_apk_filemgr_storage_adaptor.h   
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_media_play.xml         --adaptor=$(INCLUDE_DIR)/as_apk_media_play_adaptor.h   
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_media_record.xml       --adaptor=$(INCLUDE_DIR)/as_apk_media_record_adaptor.h   
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_antenna.xml       --adaptor=$(INCLUDE_DIR)/as_apk_meta_antenna_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_mail.xml          --adaptor=$(INCLUDE_DIR)/as_apk_meta_mail_adaptor.h         
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_query.xml         --adaptor=$(INCLUDE_DIR)/as_apk_meta_query_adaptor.h        
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_record.xml        --adaptor=$(INCLUDE_DIR)/as_apk_meta_record_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_schedule.xml      --adaptor=$(INCLUDE_DIR)/as_apk_meta_schedule_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_search.xml        --adaptor=$(INCLUDE_DIR)/as_apk_meta_search_adaptor.h       
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_meta_service.xml       --adaptor=$(INCLUDE_DIR)/as_apk_meta_service_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_network.xml       --adaptor=$(INCLUDE_DIR)/as_apk_util_network_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_output_audio.xml       --adaptor=$(INCLUDE_DIR)/as_apk_output_audio_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_output_hdmi.xml        --adaptor=$(INCLUDE_DIR)/as_apk_output_hdmi_adaptor.h       
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_output_panel.xml       --adaptor=$(INCLUDE_DIR)/as_apk_output_panel_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_output_video.xml       --adaptor=$(INCLUDE_DIR)/as_apk_output_video_adaptor.h      
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_power.xml         --adaptor=$(INCLUDE_DIR)/as_apk_util_power_adaptor.h        
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_scenario_base.xml      --adaptor=$(INCLUDE_DIR)/as_apk_scenario_base_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_scenario_pincode.xml   --adaptor=$(INCLUDE_DIR)/as_apk_scenario_pincode_adaptor.h  
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_util_swupdate.xml      --adaptor=$(INCLUDE_DIR)/as_apk_util_swupdate_adaptor.h     
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_sysinfo_stb.xml   --adaptor=$(INCLUDE_DIR)/as_apk_sysinfo_stb_adaptor.h  
	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_sysinfo_tuner.xml --adaptor=$(INCLUDE_DIR)/as_apk_sysinfo_tuner_adaptor.h

	./dbusxx-xml2cpp $(INTROSPECT_DIR)/apk_module_submodule.xml --adaptor=$(INCLUDE_DIR)/as_apk_module_submodule_adaptor.h

octopus: introspect-interface
ifeq ($(PARALLEL_MAKE_APPS), yes)
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' objlist
endif
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' library
	$(HUMAX_MAKE_LINK) link
	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_DIR)/root/home/$@
	@$(HUMAX_SUDO) $(HUMAX_CP) -rdf $@ $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_CP) -rdf $@ $(HUMAX_APP_STAG_DIR)/root/home/$@
	$(HUMAX_RM) $@
clean:
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' clean-act

