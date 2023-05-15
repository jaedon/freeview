V =
ifeq ($(strip $(V)), )
	Q=@
else
	Q=
endif

ifneq ($(HUMAX_CREATE_LIB),)
-include obj.lst
-include so.lst
endif

-include $(HUMAX_WORK_DIR)/make/.config
-include $(HUMAX_CONFIG_DIR)/.config
include $(HUMAX_WORK_DIR)/platform/sdk/driver/bcm_environment.config

ifneq ($(HUMAX_OBJ),)
obj-y := $(HUMAX_OBJ)
so-y := $(HUMAX_OBJ)
endif

ifneq ($(HUMAX_BUILD_DIR),)
include $(HUMAX_BUILD_DIR)Makefile
endif

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak

HUMAX_MAKEFLAGS="$(HUMAX_MULTI_JOBS) $(MAKEFLAGS)"

sub-dir-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %/, $(obj-y)))
sub-so-dir-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %/, $(so-y)))
sub-obj-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %.o, $(obj-y))) $(fullpath-obj-y)
sub-so-y := $(addprefix $(HUMAX_BUILD_DIR), $(filter %.o, $(so-y)))
sub-target-y := $(filter-out %.o, $(filter-out %/, $(obj-y)))
sub-so-target-y := $(filter-out %.o, $(filter-out %/, $(so-y)))

sub-extra-lib-y := $(addprefix $(HUMAX_BUILD_DIR), $(extra-lib-y))

obj-list += $(sub-obj-y)

ifneq ($(LIB_TYPE), SHARED)
build_sub_dir: $(sub-dir-y) $(sub-obj-y) $(sub-target-y) add_obj_list_file FORCE
	@echo $(sub-dir-y) $(sub-obj-y) $(sub-target-y)
else
build_sub_dir: $(sub-so-dir-y) $(sub-so-y) $(sub-so-target-y) add_so_list_file FORCE
	@echo $(sub-so-dir-y) $(sub-so-y) $(sub-so-target-y)
endif

static-library: make_obj_list_file build_sub_dir FORCE
	$(Q)$(HUMAX_MAKE) -f $(HUMAX_WORK_DIR)/make/HumaxDefaultRule.mak HUMAX_OBJ= HUMAX_BUILD_DIR= HUMAX_CREATE_LIB=y lib$(LIBNAME).a

shared-library: make_so_list_file build_sub_dir FORCE
	$(Q)$(HUMAX_MAKE) -f $(HUMAX_WORK_DIR)/make/HumaxDefaultRule.mak HUMAX_OBJ= HUMAX_BUILD_DIR= HUMAX_CREATE_LIB=y lib$(LIBNAME).so

make_obj_list_file: FORCE
	$(Q)$(HUMAX_ECHO) "HUMAX_OBJLIST := \\" > obj.lst

make_so_list_file: FORCE
	$(Q)$(HUMAX_ECHO) "HUMAX_SOLIST := \\" > so.lst

add_obj_list_file:
	$(Q)$(HUMAX_ECHO) $(sub-obj-y)\\ >> obj.lst
	$(Q)$(HUMAX_ECHO) $(sub-extra-lib-y)\\ >> obj.lst

add_so_list_file:
	$(Q)$(HUMAX_ECHO) $(sub-so-y)\\ >> so.lst
	$(Q)$(HUMAX_ECHO) $(sub-extra-lib-y)\\ >> so.lst

clean_sub_dir: $(sub-dir-y) FORCE
	$(Q)$(HUMAX_RM) $(wildcard $(HUMAX_BUILD_DIR)*.o $(HUMAX_BUILD_DIR)*.d)

clean_so_sub_dir: $(sub-so-dir-y) FORCE
	$(Q)$(HUMAX_RM) $(wildcard $(HUMAX_BUILD_DIR)*.o $(HUMAX_BUILD_DIR)*.d)

clean: clean_sub_dir FORCE
	$(Q)$(HUMAX_RM) obj.lst
	$(Q)$(HUMAX_RM) $(notdir $(CURDIR)).a

so_clean: clean_so_sub_dir FORCE
	$(Q)$(HUMAX_RM) so.lst
	$(Q)$(HUMAX_RM) $(notdir $(CURDIR)).so

%/: FORCE
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
	$(Q)$(HUMAX_MAKE) -f $(HUMAX_WORK_DIR)/make/HumaxDefaultRule.mak LIB_TYPE=$(LIB_TYPE) MAKEFLAGS=$(HUMAX_MAKEFLAGS) HUMAX_OBJ= HUMAX_BUILD_DIR=$@ build_sub_dir
else
ifneq ($(LIB_TYPE), SHARED)
	$(Q)$(HUMAX_MAKE) -f $(HUMAX_WORK_DIR)/make/HumaxDefaultRule.mak LIB_TYPE=$(LIB_TYPE) HUMAX_OBJ= HUMAX_BUILD_DIR=$@ clean_sub_dir
else
	$(Q)$(HUMAX_MAKE) -f $(HUMAX_WORK_DIR)/make/HumaxDefaultRule.mak LIB_TYPE=$(LIB_TYPE) HUMAX_OBJ= HUMAX_BUILD_DIR=$@ clean_so_sub_dir
endif
endif

include $(HUMAX_PORTING_MAKE_DIR)/HumaxDependency.mak

COLOR_COMPILE ?= \x1b[33;01m
COLOR_WARNING ?= \x1b[32;01m
COLOR_ERROR   ?= \x1b[31;01m
COLOR_NONE    ?= \x1b[0m

%.o: %.c
	$(HUMAX_ECHO) [Compiling ---------- $@]
ifneq ($(LIB_TYPE), SHARED)
ifeq ($(CONFIG_OS_UCOS),y)
ifeq ($(CONFIG_ARM), y)
	$(Q)$(HUMAX_CC) $(HUMAX_CFLAG) $< -c -o $@ -MF $(@:%.o=%.d) -MT $@ 2>${filter %.o,${subst /, ,$@}}.log || touch ${filter %.o,${subst /, ,$@}}.err;
else
	$(Q)$(HUMAX_CC) $(HUMAX_CFLAG) -c -o $@ $< 2>${filter %.o,${subst /, ,$@}}.log || touch ${filter %.o,${subst /, ,$@}}.err;
endif
else
	$(Q)$(HUMAX_CC) $(HUMAX_CFLAG) $< -c -o $@ -MF $(@:%.o=%.d) -MT $@ 2>${filter %.o,${subst /, ,$@}}.log || touch ${filter %.o,${subst /, ,$@}}.err;
endif
else
	$(Q)$(HUMAX_CC) $(HUMAX_CFLAG) -DCONFIG_SHARED_COMPILE $< -c -o $@ -MF $(@:%.o=%.d) -MT $@ 2>${filter %.o,${subst /, ,$@}}.log || touch ${filter %.o,${subst /, ,$@}}.err;
endif
	@$(HUMAX_SED) -e 's/.*\//     /' -e 's/error/$(COLOR_ERROR)&$(COLOR_NONE)/g' -e 's/warning/$(COLOR_WARNING)&$(COLOR_NONE)/g' ${filter %.o,${subst /, ,$@}}.log
	@if test -e ${filter %.o,${subst /, ,$@}}.err; then $(HUMAX_RM) -f ${filter %.o,${subst /, ,$@}}.log ${filter %.o,${subst /, ,$@}}.err; exit 1; fi;
	@$(HUMAX_RM) -f ${filter %.o,${subst /, ,$@}}.log ${filter %.o,${subst /, ,$@}}.err
	
%.a: $(HUMAX_OBJLIST) FORCE
	$(Q)$(HUMAX_AR) $(HUMAX_AR_FLAGS) $@ $(HUMAX_OBJLIST)
	$(Q)$(HUMAX_CP) $@ $(HUMAX_OUTPUT_DIR)/lib

%.so: $(HUMAX_SOLIST) FORCE
	$(Q)$(HUMAX_CC) $(HUMAX_CFLAGS) -shared -o $@ $(HUMAX_SOLIST)
	$(Q)$(HUMAX_CP) $@ $(HUMAX_OUTPUT_DIR)/lib

FORCE:

