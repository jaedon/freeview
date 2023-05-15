
-include obj.lst
include $(HUMAX_TOP_DIR)/apps/make/$(HUMAX_CONFIG_MODE)

ifneq ($(HUMAX_OBJ),)
obj-y := $(HUMAX_OBJ)
endif

ifneq ($(HUMAX_BUILD_DIR),)
include $(HUMAX_BUILD_DIR)Makefile
endif

include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak
include $(HUMAX_MAKE_DIR)/global_make/HumaxGlobalColorScheme.mak

sub-dir-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %/, $(obj-y)))
sub-obj-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %.o, $(obj-y)))
sub-target-y := $(filter-out %.o, $(filter-out %/, $(obj-y)))

obj-list += $(sub-obj-y)

build_sub_dir: $(sub-dir-y) $(sub-obj-y) $(sub-target-y) add_obj_list_file FORCE

library: $(HUMAX_LIB_LIST_FILE) make_obj_list_file build_sub_dir FORCE
#	$(HUMAX_ECHO) =======================================
#	$(HUMAX_ECHO) sub-obj-y = $(sub-obj-y)
#	$(HUMAX_ECHO) sub-dir-y = $(sub-dir-y)
#	$(HUMAX_ECHO) sub-target-y = $(sub-target-y)
#	$(HUMAX_ECHO) obj-list = $(obj-list)
#	$(HUMAX_ECHO) =======================================
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ= HUMAX_BUILD_DIR= lib$(HUMAX_SO_NAME).so

make_obj_list_file: FORCE
	$(HUMAX_ECHO) "HUMAX_OBJLIST := \\" > obj.lst

add_obj_list_file:
	$(HUMAX_ECHO) $(sub-obj-y)\\ >> obj.lst

clean_sub_dir: $(sub-dir-y) FORCE
	@$(HUMAX_RM) $(wildcard $(HUMAX_BUILD_DIR)*.o $(HUMAX_BUILD_DIR)*.d)

clean-act: clean_sub_dir FORCE
	@$(HUMAX_RM) obj.lst
	@$(HUMAX_RM) $(notdir $(CURDIR)).a
	@$(HUMAX_RM) -rf $(CURDIR)/lib

%/: FORCE
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
	$(HUMAX_ECHO) compiling $(abspath $@) 2>&1 | $(HUMAX_SED) -e 's/compiling/$(COLOR_COMPILE)&$(COLOR_NONE)/g'
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ= HUMAX_BUILD_DIR=$@ build_sub_dir
else
	$(HUMAX_ECHO) cleaning $(abspath $@) 2>&1 | $(HUMAX_SED) -e 's/cleaning/$(COLOR_COMPILE)&$(COLOR_NONE)/g'
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ= HUMAX_BUILD_DIR=$@ clean_sub_dir
endif


include $(HUMAX_GLOBALENV_DIR)/HumaxDependency.mak

%.o: %.c
	$(HUMAX_CC) $(HUMAX_CFLAG) $< -c -o $@ 2>log.tmp || touch err.tmp; 
	@$(HUMAX_SED) -e 's/.*\//     /' -e 's/error/$(COLOR_ERROR)&$(COLOR_NONE)/g' -e 's/warning/$(COLOR_WARNING)&$(COLOR_NONE)/g' log.tmp
	@if test -e err.tmp; then $(HUMAX_RM) -f log.tmp err.tmp; exit 1; fi;
	@$(HUMAX_RM) -f log.tmp err.tmp

#	$(HUMAX_CC) $(HUMAX_CFLAG) $< -c -o $@ 2>&1 | $(HUMAX_SED) -e 's/.*\//     /' -e 's/error/$(COLOR_ERROR)&$(COLOR_NONE)/g' -e 's/warning/$(COLOR_WARNING)&$(COLOR_NONE)/g'

%.so: $(HUMAX_OBJLIST) FORCE
	$(HUMAX_CC) -shared -Wl,-soname,lib$(HUMAX_SO_NAME).so.0 -Wl,-whole-archive $(APP-LINK-y) -Wl,-no-whole-archive -o $@.0.0.1 $(HUMAX_OBJLIST)
#	$(HUMAX_CP) $@ $(HUMAX_OUTPUT_DIR)/lib
#	$(HUMAX_SED) s/'$(subst lib,-l,$(subst .a,,$@)) '// $(HUMAX_LIB_LIST_FILE) | $(HUMAX_SED) s/':='/':= $(subst lib,-l,$(subst .a,,$@))'/ > $(HUMAX_LIB_LIST_FILE)_tmp
#	$(HUMAX_MV) $(HUMAX_LIB_LIST_FILE)_tmp  $(HUMAX_LIB_LIST_FILE)

%.a: $(HUMAX_OBJLIST) FORCE
	$(HUMAX_AR) $(HUMAX_AR_FLAGS) $@ $(HUMAX_OBJLIST)
	$(HUMAX_CP) $@ $(HUMAX_OUTPUT_DIR)/lib
	$(HUMAX_SED) s/'$(subst lib,-l,$(subst .a,,$@)) '// $(HUMAX_LIB_LIST_FILE) | $(HUMAX_SED) s/':='/':= $(subst lib,-l,$(subst .a,,$@))'/ > $(HUMAX_LIB_LIST_FILE)_tmp
	$(HUMAX_MV) $(HUMAX_LIB_LIST_FILE)_tmp  $(HUMAX_LIB_LIST_FILE)

$(HUMAX_LIB_LIST_FILE):
	$(HUMAX_MKDIR) $(dir $(HUMAX_LIB_LIST_FILE))
	$(HUMAX_ECHO) "HUMAX_LIB_LIST := " > $@

FORCE:
