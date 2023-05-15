# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

ifneq "" "$(LINUX_BUILD)"

TARGETS = \
    lib   \

else
# Mac

pass0: check_nttree_env drmresults.h drmresults.c

drmresults.h: drmresults.mc
	@echo Making $@... 1>&2
	@perl convertmctoh_mac.pl drmresults.mc > drmresults.h

drmresults.c: drmresults.mc
	@echo Making $@... 1>&2
	@perl make_error_source.pl drmresults_c_template.txt drmresults.mc > drmresults.c

clean: check_env
	@if [ -e drmresults.h ]; then \
		echo Cleaning drmresults.h... 1>&2; \
		rm drmresults.h; \
	fi
	@if [ -e drmresults.c ]; then \
		echo Cleaning drmresults.c... 1>&2; \
		rm drmresults.c; \
	fi

check_nttree_env:
	@if test X"$(_NTTREE)" = "X"; \
	then \
		echo "ERROR: Set environment before running this."; \
		exit 1; \
	fi

endif

include $(PLAYREADY_DIR)/GNUmakefile.common

