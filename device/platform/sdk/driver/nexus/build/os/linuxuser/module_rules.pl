#############################################################################
#    (c)2008-2012 Broadcom Corporation
#
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: module_rules.pl $
# $brcm_Revision: 29 $
# $brcm_Date: 8/22/12 1:25p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxuser/module_rules.pl $
# 
# 29   8/22/12 1:25p erickson
# SW7231-958: use .preload files to get struct definitions from other
#  modules
# 
# 28   7/17/12 8:46a erickson
# SW7420-1148: move -no_proxy test
# 
# 27   11/11/11 10:46a erickson
# SW7420-1148: fix usermode IPC for GL_SUPPORT=y
# 
# 26   11/9/11 3:52p erickson
# SW7420-1148: if GL_SUPPORT=y, don't generate or build proxy for
#  graphics3d module
# 
# 25   10/28/11 12:42p erickson
# SW7420-1148: proxy should wait for driver to build so that autogen is
#  done in one place
# 
# 24   9/14/11 12:41p erickson
# SW7346-488: nexus_class_verification.h must be built, even if there's
#  no proxy
# 
# 23   9/2/11 12:39p erickson
# SW7420-1148: refactor class_list.pl to generate per-module "get current
#  client" functions
# 
# 22   7/13/11 3:44p erickson
# SW7420-1979: #include nexus_class_verification.h is always required
#  because of automatic shutdown
# 
# 21   3/28/11 12:18p erickson
# SW7420-1671: don't thunk BASE
#
# 20   2/22/11 4:59p erickson
# SW7405-5105: fix dependencies for nexus_ipc_modules.h
#
# 19   2/15/11 4:00p erickson
# SW7420-1491: support local functions in ipc client
#
# 18   2/15/11 2:14p erickson
# SW7420-1123: allow ipc thunk to be built during MODE=client build
#
# 17   2/3/11 6:37p vsilyaev
# SW7420-1441: Use local (non proxied) implementation for function that
#  flush CPU cache
#
# 16   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 15   12/9/10 10:57a erickson
# SW7420-1148: fix race condition when building class_list.inc
#
# 14   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 13   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
#
# 12   9/15/10 3:43p jtna
# SW7420-1004: add synthunk for CORE and PLATFORM
#
# 11   11/17/09 2:02p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
#
# 10   4/8/09 4:54p vsilyaev
# PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
#  modules should include explicitly bkni_multi.h
#
#
# 8   3/30/09 3:50p vsilyaev
# PR 53611: Create the object directory only when needed
#
# 7   10/9/08 11:48a erickson
# PR47329: rename var to NEXUS_EXCLUDED_MODULES
#
# 6   6/27/08 8:58a erickson
# PR43846: convert to $(PERL)
#
# 5   4/8/08 10:40a vsilyaev
# PR 41083: Added bprofile support
#
# 4   1/31/08 12:45p vsilyaev
# PR 38682: Build proxy for the core module
#
# 3   1/24/08 4:31p vsilyaev
# PR 38682: Added proxy environment
#
# 2   1/23/08 3:15p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
#############################################################################
use strict;

my $i;
my $moduleUpper;

my $file = shift @ARGV;
open(OUTFILE, ">$file") or die "Unable to open output file $file";

my $no_proxy;
if ($ARGV[0] eq "-no_proxy") {
    shift @ARGV;
    # -no_proxy MODULE[,MODULE,...]
    $no_proxy = shift @ARGV;
}

print OUTFILE "#####################################################\n";
print OUTFILE "# This file is automatically generated.  Do not edit.\n";
print OUTFILE "#####################################################\n";

print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/exists:\n";
print OUTFILE "\t\$(Q_)\$(MKDIR) \$@\n";
print OUTFILE "\n";
print OUTFILE "CLASS_LIST=\$(NEXUS_SYNCTHUNK_DIR)/class_list.inc\n";
print OUTFILE "CLASS_VERIFICATION=\$(NEXUS_SYNCTHUNK_DIR)/nexus_class_verification.h\n";
print OUTFILE "PUBLIC_INCLUDES := ";
foreach $moduleUpper (@ARGV) {
    next if ($moduleUpper eq 'BASE');
    print OUTFILE "\$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES)) ";
}
print OUTFILE "\n";

# Generate nexus_class_verification.h using PERL
print OUTFILE "NEXUS_SERVER_MODULES=\$(filter-out \$(NEXUS_CLIENT_MODULES),\$(NEXUS_MODULES))\n";
print OUTFILE "ifeq (\$(NEXUS_MODE),proxy)\n";
print OUTFILE "\$(CLASS_LIST) \$(CLASS_VERIFICATION): \${EXTRA_ALL}\n";
print OUTFILE "else\n";
print OUTFILE "\$(CLASS_LIST) \$(CLASS_VERIFICATION): \n";
print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/kernelproxy && \$(PERL) class_list.pl \$(CLASS_LIST) \$(CLASS_VERIFICATION) -headers \$(PUBLIC_INCLUDES) -modules \$(NEXUS_SERVER_MODULES)\n";
print OUTFILE "endif\n";
print OUTFILE "\n";

# Generate nexus_ipc_modules.h using PERL
print OUTFILE "NEXUS_IPC_MODULES=\$(NEXUS_SYNCTHUNK_DIR)/nexus_ipc_modules.h\n";
print OUTFILE "\$(NEXUS_IPC_MODULES): \$(NEXUS_SYNCTHUNK_DIR)/exists\n";
print OUTFILE "\t\$(shell \$(PERL) \$(NEXUS_TOP)/build/os/\$(B_REFSW_OS)/generate_nexus_ipc_modules.pl \$(NEXUS_IPC_MODULES) \$(NEXUS_SERVER_MODULES))\n";
print OUTFILE "\n";

foreach $moduleUpper (@ARGV) {
    my $moduleLower = lc $moduleUpper;
    my $thunked = 1;
    my $proxy = 1;
    my $ipc = 1;

        print OUTFILE "#\n# $moduleUpper\n#\n";
        print OUTFILE "\$(NEXUS_OBJ_DIR)/$moduleUpper/exists:\n";
        print OUTFILE "\t\$(Q_)\$(MKDIR) \$@\n";
        print OUTFILE "\n";

        # Skip thunks for BASE -- it is an exception
        if ( ($moduleUpper eq 'BASE') ) {
           $thunked = 0;
           $proxy = 0;
           $ipc = 0;
        }
        if ( $no_proxy =~ /$moduleUpper/ ) {
           $proxy = 0;
        }

        # kernelmode proxy (usermode portion)
        if( $proxy == 1) {
            print OUTFILE "ifeq (\$(NEXUS_MODE),proxy)\n";
            print OUTFILE "ifeq (,\$(filter $moduleUpper,\$(NEXUS_CLIENT_MODULES)))\n";
            # for proxy, only compile the proxy object
            print OUTFILE "NEXUS_$moduleUpper\_OBJECTS := \$(NEXUS_OBJ_DIR)/$moduleUpper/nexus_$moduleLower\_proxy.\$(NEXUS_OBJ_SUFFIX) \n";
            print OUTFILE "NEXUS_$moduleUpper\_OBJECTS += \$(addprefix \$(NEXUS_OBJ_DIR)/$moduleUpper/,\$(patsubst %.c,%.\$(NEXUS_OBJ_SUFFIX),\$(notdir \$(NEXUS_$moduleUpper\_LOCAL_SOURCES))))\n";
            print OUTFILE "NEXUS_$moduleUpper\_CFLAGS += -DNEXUS_THUNK_LAYER \n";
            if ( $moduleUpper eq 'PLATFORM') {
                print OUTFILE 'NEXUS_PLATFORM_OBJECTS += $(addprefix $(NEXUS_OBJ_DIR)/PLATFORM/,$(patsubst %.c,%.$(NEXUS_OBJ_SUFFIX),$(notdir $(NEXUS_PLATFORM_PROXY_SOURCES))))';
                print OUTFILE "\n";
            }
            print OUTFILE "endif\n"; #NEXUS_CLIENT_MODULES
            print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_proxy.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h\n";
            print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h: \$(NEXUS_SYNCTHUNK_DIR)/exists \$(wildcard \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))) \$(CLASS_LIST)\n";
            print OUTFILE "endif\n"; #proxy
        }

        # usermode IPC
        if ( $ipc == 1) {
            print OUTFILE "ifeq (\${NEXUS_SERVER_SUPPORT},y)\n";
            print OUTFILE "ifneq (\${NEXUS_MODE},proxy)\n";
            print OUTFILE "ifeq (,\$(filter $moduleUpper,\$(NEXUS_CLIENT_MODULES)))\n";
            # for user mode IPC, client modules live in the server and the client
            print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_client.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_api.h\n";
            print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_server.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_api.h \$(CLASS_VERIFICATION)\n\n";
            print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_api.h: \$(NEXUS_SYNCTHUNK_DIR)/exists \$(wildcard \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))) \$(CLASS_LIST) \$(NEXUS_IPC_MODULES)\n";
            print OUTFILE "\t\@echo \"[Ipc ..... $moduleLower]\"\n";
            print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/ipcthunk && \$(PERL) bapi_build.pl $moduleUpper \$(NEXUS_SYNCTHUNK_DIR) \$(CLASS_LIST) \$(addsuffix /*.preload,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES)) \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))\n\n";

            # for client, only compile the client object + local sources
            print OUTFILE "ifeq (\${NEXUS_MODE},client)\n";
            print OUTFILE "  NEXUS_$moduleUpper\_CLIENT := \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_client.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_api.h\n";
            print OUTFILE "  NEXUS_$moduleUpper\_OBJECTS := \$(NEXUS_OBJ_DIR)/$moduleUpper/nexus_$moduleLower\_ipc_client.\$(NEXUS_OBJ_SUFFIX)\n";
            print OUTFILE "  NEXUS_$moduleUpper\_OBJECTS += \$(addprefix \$(NEXUS_OBJ_DIR)/$moduleUpper/,\$(patsubst %.c,%.\$(NEXUS_OBJ_SUFFIX),\$(notdir \$(NEXUS_$moduleUpper\_LOCAL_SOURCES))))\n";
            print OUTFILE "  NEXUS_$moduleUpper\_CFLAGS += -DNEXUS_THUNK_LAYER \n";
            if ( $moduleUpper eq 'PLATFORM') {
                print OUTFILE '  NEXUS_PLATFORM_OBJECTS += $(addprefix $(NEXUS_OBJ_DIR)/PLATFORM/,$(patsubst %.c,%.$(NEXUS_OBJ_SUFFIX),$(notdir $(NEXUS_PLATFORM_CLIENT_SOURCES))))' . "\n";
            }
            print OUTFILE "else\n";
            # for server, add the server object
            print OUTFILE "  NEXUS_$moduleUpper\_OBJECTS += \$(NEXUS_OBJ_DIR)/$moduleUpper/nexus_$moduleLower\_ipc_server.\$(NEXUS_OBJ_SUFFIX)\n";
            print OUTFILE "endif\n"; #client
            print OUTFILE "endif\n"; #NEXUS_CLIENT_MODULES
            print OUTFILE "endif\n"; #proxy
            print OUTFILE "endif\n"; #NEXUS_SERVER_SUPPORT

        }

        # syncthunk
        if ( $thunked == 1 ) {
           # Thunk Info
           print OUTFILE "NEXUS_$moduleUpper\_SYNCTHUNKS := \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h\n";
           print OUTFILE "\n";
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h\n";
           print OUTFILE "\n";
           # if there's no proxy, we still need nexus_class_verification.h for the stubs
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h: \$(wildcard \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))) \$(CLASS_LIST) \$(NEXUS_SYNCTHUNK_DIR)/exists \$(CLASS_VERIFICATION)\n";
           print OUTFILE "\t\@echo \"[Thunk..... $moduleLower]\"\n";
           print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/syncthunk && \$(PERL) bapi_build.pl $moduleUpper \$(NEXUS_SYNCTHUNK_DIR) \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))\n";
           print OUTFILE "\n";
        }

        # Module Rules
        print OUTFILE "ifdef NEXUS_$moduleUpper\_PRECOMPILED_HEADER\n";
        print OUTFILE "\$(NEXUS_OBJ_DIR)/$moduleUpper/nexus_$moduleLower\_precompiled.h.gch: \$(NEXUS_$moduleUpper\_PRECOMPILED_HEADER) \${NEXUS_OBJ_DIR}/$moduleUpper/exists \$(NEXUS_$moduleUpper\_SYNCTHUNKS)\n";
        print OUTFILE "\t\@echo \"[Compile... \$(notdir \$<) ($moduleLower)] \"\n";
        print OUTFILE "\t\$(Q_)\$(CC) \$(CDEP_FLAG) \$(NEXUS_CFLAGS) \$(NEXUS_CFLAGS_BPROFILE) \$(NEXUS_$moduleUpper\_CFLAGS) -c \$< -o \$@\n";
        print OUTFILE "\n";
        print OUTFILE "\$(NEXUS_$moduleUpper\_OBJECTS): \$(NEXUS_OBJ_DIR)/$moduleUpper/%.\$(NEXUS_OBJ_SUFFIX): %.c \${NEXUS_OBJ_DIR}/$moduleUpper/exists \${NEXUS_OBJ_DIR}/$moduleUpper/nexus_$moduleLower\_precompiled.h.gch \$(NEXUS_$moduleUpper\_SYNCTHUNKS) \${NEXUS_PRECOMPILED_HEADER_H}.gch \$(NEXUS_IPC_MODULES)\n";
        print OUTFILE "\t\@echo \"[Compile... \$(notdir \$<) ($moduleLower)]\"\n";
        print OUTFILE "\t\$(Q_)\$(CC) -include \$(if \$(filter \$<,\$(NEXUS_$moduleUpper\_SYNCTHUNKS) \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_proxy.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ipc_client.c),\${NEXUS_PRECOMPILED_HEADER_H},\${NEXUS_OBJ_DIR}/$moduleUpper/nexus_$moduleLower\_precompiled.h) \$(CDEP_FLAG) \$(NEXUS_CFLAGS) \$(NEXUS_CFLAGS_BPROFILE) \$(NEXUS_$moduleUpper\_CFLAGS) -c \$< -o \$@\n";
        print OUTFILE "\n";
        print OUTFILE "else\n";
        print OUTFILE "\$(NEXUS_$moduleUpper\_OBJECTS): \$(NEXUS_OBJ_DIR)/$moduleUpper/%.\$(NEXUS_OBJ_SUFFIX): %.c \${NEXUS_OBJ_DIR}/$moduleUpper/exists \$(NEXUS_$moduleUpper\_SYNCTHUNKS) \${NEXUS_PRECOMPILED_HEADER_H}.gch \$(NEXUS_IPC_MODULES)\n";
        print OUTFILE "\t\@echo \"[Compile... \$(notdir \$<) ($moduleLower)]\"\n";
        print OUTFILE "\t\$(Q_)\$(CC) \$(CDEP_FLAG) -include \${NEXUS_PRECOMPILED_HEADER_H} \$(NEXUS_CFLAGS) \$(NEXUS_CFLAGS_BPROFILE) \$(NEXUS_$moduleUpper\_CFLAGS) -c \$< -o \$@\n";
        print OUTFILE "\n";
        print OUTFILE "endif\n";

}
close OUTFILE;

