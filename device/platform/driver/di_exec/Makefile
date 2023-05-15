
#di_exec_target=${shell ls}
di_exec_target=

di_exec_target += probefs/
ifeq ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
di_exec_target += usbctrl/
di_exec_target += dicmd/
endif

di_exec:
	@$(foreach dir,$(di_exec_target),if [ -d $(dir) ]; then make -C $(dir)/ $(EXEC_COMPILE_FLAGS) all; fi;)
	
di_exec-clean:
	@$(foreach dir,$(di_exec_target),if [ -d $(dir) ]; then make -C $(dir)/ $(EXEC_COMPILE_FLAGS) clean; fi;)

di_exec-distclean: di_exec-clean
