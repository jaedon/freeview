# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

pass0: check_nttree_env $(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h

$(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h: $(BASEDIR)/PK/WMDRM-MD/DrmProfilePC.mk $(BASEDIR)/PK/WMDRM-MD/DrmProfileCompileTimeOptions.mk
	@echo Making $@... 1>&2
	@perl make_profile_header.pl $(BASEDIR)/PK/WMDRM-MD/DrmProfilePC.mk $(BASEDIR)/PK/WMDRM-MD/DrmProfileCompileTimeOptions.mk $(BASEDIR)/PK/WMDRM-MD/source/profile/drmprofile_h_template.txt > $(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h

clean: check_env
	@if [ -e $(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h ]; then \
		echo Cleaning $(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h... 1>&2; \
		rm $(BASEDIR)/PK/WMDRM-MD/source/inc/drmbuild_pc.h; \
	fi

check_nttree_env:
	@if test X"$(_NTTREE)" = "X"; \
	then \
		echo "ERROR: Set environment before running this."; \
		exit 1; \
	fi

include $(PLAYREADY_DIR)/GNUmakefile.common

