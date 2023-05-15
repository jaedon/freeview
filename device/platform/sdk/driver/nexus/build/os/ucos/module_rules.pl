#############################################################################
#    (c)2008-2011 Broadcom Corporation
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
# $brcm_Revision: 4 $
# $brcm_Date: 10/18/11 9:38a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/ucos/module_rules.pl $
# 
# 4   10/18/11 9:38a erickson
# SW7358-162: generate nexus_class_verification.h. sync up file with
#  linuxuser/module_rules.pl.
# 
# 3   9/15/10 3:43p jtna
# SW7420-1004: add synthunk for CORE and PLATFORM
# 
# 2   6/27/08 8:59a erickson
# PR43846: convert to $(PERL)
#
# 1   6/17/08 3:03p cnovak
# PR43697: Add uCOS to Nexus.
#
#############################################################################
use strict;

my $i;
my $moduleUpper;

my $file = shift @ARGV;
open(OUTFILE, ">$file") or die "Unable to open output file $file";

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
print OUTFILE "\$(CLASS_LIST) \$(CLASS_VERIFICATION): \n";
print OUTFILE "\t\$(Q_)cd \$(NEXUS_TOP)/build/tools/kernelproxy && \$(PERL) class_list.pl \$(CLASS_LIST) \$(CLASS_VERIFICATION) -headers \$(PUBLIC_INCLUDES) -modules \$(NEXUS_SERVER_MODULES)\n";
print OUTFILE "\n";

foreach $moduleUpper (@ARGV) {
    my $moduleLower = lc $moduleUpper;
    my $thunked = 1;
    my $proxy = 0;

        print OUTFILE "#\n# $moduleUpper\n#\n";
        print OUTFILE "\$(NEXUS_OBJ_DIR)/$moduleUpper/exists:\n";
        print OUTFILE "\t\$(Q_)\$(MKDIR) \$@\n";
        print OUTFILE "\n";

        # Skip thunks for BASE -- it is an exception
        if ( ($moduleUpper eq 'BASE') ) {
           $thunked = 0;
           $proxy = 0;
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
        print OUTFILE "\$(NEXUS_$moduleUpper\_OBJECTS): \$(NEXUS_OBJ_DIR)/$moduleUpper/%.\$(NEXUS_OBJ_SUFFIX): %.c \$(NEXUS_$moduleUpper\_SYNCTHUNKS) \$(NEXUS_$moduleUpper\_DRIVER)  \n";
        print OUTFILE "\t\$(Q_)\$(MKDIR) \$(NEXUS_OBJ_DIR)/$moduleUpper >& /dev/null\n";
        print OUTFILE "\t\@echo \"[Compile... \$(notdir \$<) ($moduleLower)]\"\n";
        print OUTFILE "\t\$(Q_)\$(CC) \$(CDEP_FLAG) \$(NEXUS_CFLAGS) \$(NEXUS_$moduleUpper\_CFLAGS) -c \$< -o \$@\n";
        print OUTFILE "\n";

}
close OUTFILE;
