#
# Copyright (C) 2006 HUMAX Co., Ltd. All rights reserved.
#

###############################################################
emudi:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/emudi $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
emudi-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/emudi $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/emudi_compile.log
emudi-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/emudi $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: emudi emudi-clean emudi-distclean

memory:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/debug $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/null $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
memory-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/debug $(APP_CONFIG) clean  2>&1 || exit -1;
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/null $(APP_CONFIG) clean  2>&1 || exit -1;
memory-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/debug $(APP_CONFIG) distclean 2>&1 || exit -1;
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/memory/null $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: memory memory-clean memory-distclean

hlib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hlib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
hlib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hlib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/hlib-compile.log
hlib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hlib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: hlib hlib-clean hlib-distclean

dlib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/dlib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
dlib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/dlib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/dlib-compile.log
dlib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/dlib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: dlib dlib-clean dlib-distclean

silib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/silib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
silib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/silib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/silib-compile.log
silib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/silib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: silib silib-clean silib-distclean

sitextlib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/sitextlib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
sitextlib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/sitextlib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/sitextlib-compile.log
sitextlib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/sitextlib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: sitextlib sitextlib-clean sitextlib-distclean

clib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/clib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
clib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/clib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/clib-compile.log
clib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/clib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: clib clib-clean clib-distclean

rlib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/rlib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
rlib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/rlib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/rlib-compile.log
rlib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/rlib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: rlib rlib-clean rlib-distclean

cutest:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/cutest $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
cutest-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/cutest $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/cutest-compile.log
cutest-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/cutest $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: cutest cutest-clean cutest-distclean

hcrlib:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hcrlib $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
hcrlib-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hcrlib $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/hcrlib-compile.log
hcrlib-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/hcrlib $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: hcrlib hcrlib-clean hcrlib-distclean

ondk:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/ondk $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
ondk-clean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/ondk $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/ondk-compile.log
ondk-distclean:
	@$(MAKE) -C $(OCTOLIB_MAKE_DIR)/ondk $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: ondk ondk-clean ondk-distclean


