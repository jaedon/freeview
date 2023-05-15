#############################################################################
#    (c)2008-2010 Broadcom Corporation
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
# $brcm_Revision: 3 $
# $brcm_Date: 9/15/10 3:44p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/vxworks/module_rules.pl $
# 
# 3   9/15/10 3:44p jtna
# SW7420-1004: add synthunk for CORE and PLATFORM
# 
# 2   6/27/08 8:59a erickson
# PR43846: convert to $(PERL)
#
# 1   5/22/08 4:29p rjlewis
# PR40352: Broke symlink.
#
# 3   1/31/08 12:45p vsilyaev
# PR 38682: Build proxy for the core module
#
# 2   1/23/08 3:17p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
#############################################################################
use strict;

my $i;
my $moduleLower;
my $moduleUpper;
my $thunked;
my $proxy;

open(OUTFILE, ">@ARGV[0]") or die "Unable to open output file @ARGV[0]";

print OUTFILE "#####################################################\n";
print OUTFILE "# This file is automatically generated.  Do not edit.\n";
print OUTFILE "#####################################################\n";

for ($i=1;$i<=$#ARGV;$i++) {
	    $moduleLower = lc @ARGV[$i];
	    $moduleUpper = uc @ARGV[$i];
        $thunked = 1;
        $proxy = 0;

        print OUTFILE "#\n# $moduleUpper\n#\n";

        if ( ($moduleUpper eq 'BASE') ) {
           $thunked = 0;
        }

        if ( ($moduleUpper eq "BASE") ) {
           $proxy = 0;
        }

        # Skip thunks for BASE -- it is an exception
        if ( $thunked == 1 ) {
           # Thunk Info
           print OUTFILE "NEXUS_$moduleUpper\_SYNCTHUNKS := \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h\n";
           print OUTFILE "\n";
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h\n";
           print OUTFILE "\n";
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_thunks.h: \$(wildcard \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES)))\n";
           print OUTFILE "\t\$(Q_)\$(MKDIR) \$(NEXUS_SYNCTHUNK_DIR) >& /dev/null\n";
           print OUTFILE "\t\@echo \"[Thunk..... $moduleLower]\"\n";
           print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/syncthunk && \$(PERL) bapi_build.pl $moduleUpper \$(NEXUS_SYNCTHUNK_DIR) \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))\n";
           print OUTFILE "\n";
        }
        else {
           print OUTFILE "# $moduleUpper does not have syncthunks\n\n";
        }

        if( $proxy == 1) {
           print OUTFILE "NEXUS_$moduleUpper\_DRIVER := \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_driver.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h\n";
           print OUTFILE "NEXUS_$moduleUpper\_PROXY := \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_proxy.c \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h\n";
           print OUTFILE "NEXUS_$moduleUpper\_OBJECTS += \$(NEXUS_OBJ_DIR)/$moduleUpper/nexus_$moduleLower\_driver.\$(NEXUS_OBJ_SUFFIX)\n";
           print OUTFILE "\n";
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_driver.c: \$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h\n";
           print OUTFILE "\n";
           print OUTFILE "\$(NEXUS_SYNCTHUNK_DIR)/nexus_$moduleLower\_ioctl.h: \$(wildcard \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES)))\n";
           print OUTFILE "\t\$(Q_)\$(MKDIR) \$(NEXUS_SYNCTHUNK_DIR) >& /dev/null\n";
           print OUTFILE "\t\@echo \"[Proxy..... $moduleLower]\"\n";
           print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/kernelproxy && \$(PERL) bapi_build.pl $moduleUpper \$(NEXUS_SYNCTHUNK_DIR) \$(addsuffix /*.h,\$(NEXUS_$moduleUpper\_PUBLIC_INCLUDES))\n";
           print OUTFILE "\n";
        } else {
           print OUTFILE "# $moduleUpper does not have proxy layer\n\n";
        }

        # Module Rules
        print OUTFILE "\$(NEXUS_$moduleUpper\_OBJECTS): \$(NEXUS_OBJ_DIR)/$moduleUpper/%.\$(NEXUS_OBJ_SUFFIX): %.c \$(NEXUS_$moduleUpper\_SYNCTHUNKS) \$(NEXUS_$moduleUpper\_DRIVER)  \n";
        print OUTFILE "\t\$(Q_)\$(MKDIR) \$(NEXUS_OBJ_DIR)/$moduleUpper >& /dev/null\n";
        print OUTFILE "\t\@echo \"[Compile... \$(notdir \$<) ($moduleLower)]\"\n";
        print OUTFILE "\t\$(Q_)\$(CC) \$(CDEP_FLAG) \$(NEXUS_CFLAGS) \$(NEXUS_$moduleUpper\_CFLAGS) -c \$< -o \$@\n";
        print OUTFILE "\n";

}
close OUTFILE;
