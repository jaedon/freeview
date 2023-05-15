###############################################################################
#                                                                             #
#                             Humax Templte Makefile                          #
#                                                                             #
# Author: dklee (Plat1, STB division)                                         #
#                                                                             #
# Copyright (c) Humax co, Ltd., 2007                                          #
#                                                                             #
###############################################################################
# $Header:
# Add up global	header file	directories	for	Humax application
# Port this	file for each platfrom.
###############################################################################

###############################################################################
# Common Default Rules
###############################################################################
OBJLIST_FILE :=	$(addsuffix .lst, $(basename $(FACTORYSW_LIB)))
-include $(OBJLIST_FILE)

.PHONY: $(SUBDIRS_LIST)

###############################################################################
# Compile
###############################################################################
build_obj: $(SUBDIRS_LIST) $(OBJS_LIST)
ifneq ($(OBJS_LIST), )
	@$(FACTORYSW_ECHO) "========[ Compiling source files ]========"
ifeq ($(FACTORYSW_OS), WINDOWS)
	@$(FACTORYSW_ECHO) $(addprefix $(CURDIR)/,$(OBJS_LIST))\>> $(OBJLIST_FILE)
else
	@$(FACTORYSW_ECHO) "$(addprefix $(CURDIR)/,$(OBJS_LIST))\\">> $(OBJLIST_FILE)
endif
endif

$(SUBDIRS_LIST):
	$(FACTORYSW_MAKE) -C $@

%.o: %.c
	$(FACTORYSW_CC) $< $(FACTORYSW_CFLAGS) $(INCLUDES_LIST) -o $@

###############################################################################
# Libaray
###############################################################################
init_mklist:
	@$(FACTORYSW_ECHO) "========[ Initializing object file list ]========"
ifeq ($(FACTORYSW_OS), WINDOWS)
	@$(FACTORYSW_ECHO) TARGET_OBJLIST:= \> $(OBJLIST_FILE)
else
	@$(FACTORYSW_ECHO) "TARGET_OBJLIST:= \\"> $(OBJLIST_FILE)
endif

build_lib: $(FACTORYSW_LIB)
ifneq ($(FACTORYSW_INSTALL_DIR), )
	@$(FACTORYSW_CP) -rf $< $(FACTORYSW_INSTALL_DIR)/$(FACTORYSW_OUTPUT_NAME)
endif

clean_lib:
	$(FACTORYSW_RM) $(OBJLIST_FILE)
	$(FACTORYSW_RM) $(FACTORYSW_LIB)
	$(FACTORYSW_RM) $(FACTORYSW_INSTALL_DIR)/$(FACTORYSW_OUTPUT_NAME)

$(FACTORYSW_LIB): $(TARGET_OBJLIST)
	@$(FACTORYSW_ECHO) "========[ Making  Library ]========"
	@$(FACTORYSW_AR) $(FACTORYSW_ARFLAGS) $@ $(TARGET_OBJLIST)


###############################################################################
# Dependancy
###############################################################################
ifeq (,$(filter $(MAKECMDGOALS),clean cleanall))

DEPEND_FILES := $(patsubst %.o, %.d, $(OBJS_LIST))
ifneq ($(OBJS_LIST), )
-include $(DEPEND_FILES)
endif

.SUFFIXES: .d

MKDEP_FLAGS := $(INCLUDES_LIST) $(FACTORYSW_COMPILER_INCLUDE) -f- -o .o

%.d : %.c
	@$(FACTORYSW_ECHO) [Making dependency file : $(@F)]
	@$(FACTORYSW_MAKE_DIR)/bin/$(FACTORYSW_MAKEDEP) $(MKDEP_FLAGS) $<  > temp_$@
	@$(FACTORYSW_SED) -e "s/$*\.[oO]/$*.o $*.d/" -e "s/[cC]:\\[Pp]rogram [Ff]iles/C:\\Progra~1/" temp_$@ > $@ && $(FACTORYSW_RM) temp_$@
endif

###############################################################################
# Clean
###############################################################################
SUBCLEAN_CMD = $(addsuffix _clean,$(SUBDIRS_LIST))

$(SUBCLEAN_CMD):
	$(FACTORYSW_MAKE) -C $(subst _clean,,$@) clean

clean: $(SUBCLEAN_CMD)
ifneq ($(OBJS_LIST), )
	$(FACTORYSW_RM) $(OBJS_LIST)
	$(FACTORYSW_RM) *.d
endif




