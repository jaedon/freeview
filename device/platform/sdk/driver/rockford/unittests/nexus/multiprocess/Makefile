############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/11 $
# $brcm_Date: 10/9/12 5:13p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/multiprocess/Makefile $
# 
# Hydra_Software_Devel/11   10/9/12 5:13p erickson
# SW7425-3260: rely on NEXUS_MODE=client building client and server in
# usermode
# 
# Hydra_Software_Devel/10   6/21/12 3:58p vsilyaev
# SW7420-2315: Added client version of the memory allocator benchmark
# 
# Hydra_Software_Devel/9   11/18/11 12:28p erickson
# SW7420-1148: build client for usermode or kernelmode multiprocess
# 
# Hydra_Software_Devel/8   9/7/11 5:30p erickson
# SW7420-1123: add simple_client
# 
# Hydra_Software_Devel/7   8/8/11 5:05p erickson
# SW7420-1689: add test of untrusted client restrictions
# 
# Hydra_Software_Devel/6   7/29/11 12:07p erickson
# SW7420-1991: add multiprocess tests for nelem and nelem_out attributes
# 
# Hydra_Software_Devel/5   5/18/11 11:00a erickson
# SW7420-1671: add README, bad_join is server app
# 
# Hydra_Software_Devel/4   4/14/11 3:08p erickson
# SW7420-1671: update tests
#
# Hydra_Software_Devel/3   4/11/11 12:06p erickson
# SW7420-1123: don't depend on NEXUS_SERVER_SUPPORT
#
# Hydra_Software_Devel/2   3/23/11 5:03p erickson
# SW7420-1671: split apps so they can run in kernel mode and user mode
#
# Hydra_Software_Devel/1   3/18/11 5:33p erickson
# SW7420-1671: add join_stress
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

SERVER_APPS = \
	stress_server \
	bad_join \
	simple_server

CLIENT_APPS = \
	join_stress_client \
	alloc_stress_client \
	ir_input_client \
	playpump_scatter_gather_client \
	simple_client \
	memory_client \
	untrusted_client

.PHONY: client server clean_client_server_apps 
OTHER_APP_CLEANS += clean_client_server_apps

CFLAGS += -I{NEXUS_TOP}/in
APPS = client server

include ../common/Makefile.inc

ifneq ($(NEXUS_MODE),proxy)
CLIENT_FLAGS = NEXUS_MODE=client
endif

server: nexus
	$(MAKE) $(SERVER_APPS)

client:
ifneq ($(NEXUS_MODE),proxy)
	$(MAKE) -C $(NEXUS_TOP)/build $(CLIENT_FLAGS)
endif
	$(MAKE) $(CLIENT_FLAGS) $(CLIENT_APPS)

clean_client_server_apps:
	-$(RM) $(SERVER_APPS) $(CLIENT_APPS) *.d *.o *.out
