
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


.NOTPARALLEL : objlist build_sub_dir 

objlist: make_obj_list_file build_sub_dir FORCE

make_obj_list_file: FORCE
	$(HUMAX_ECHO) "HUMAX_OBJLIST := \\" > obj.lst

add_obj_list_file:
	$(HUMAX_ECHO) $(sub-obj-y)\\ >> obj.lst

build_sub_dir: $(sub-dir-y) add_obj_list_file FORCE


%/: FORCE
#	$(HUMAX_ECHO) makeobjlist $(abspath $@) 2>&1 | $(HUMAX_SED) -e 's/makeobjlist/$(COLOR_COMPILE)&$(COLOR_NONE)/g'
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak --no-print-directory HUMAX_OBJ= HUMAX_BUILD_DIR=$@ build_sub_dir

FORCE:

