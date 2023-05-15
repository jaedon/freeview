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
# $brcm_Workfile: generate_nexus_driver_modules.pl $
# $brcm_Revision: 3 $
# $brcm_Date: 6/17/11 3:55p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxkernel/generate_nexus_driver_modules.pl $
# 
# 3   6/17/11 3:55p erickson
# SW7125-969: only overwrite nexus_driver_modules.h if it changes
# 
# 2   5/3/11 5:09p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 1   11/10/09 11:08a erickson
# SW7405-3351: add generate_nexus_driver_modules.pl to auto-generate
#  nexus_driver_modules.h based on NEXUS_MODULES list
#
#############################################################################
use strict;

my $i;

my $targetfile = @ARGV[0];

# generate code into a .tmpfile alongside the desired targetfile at first
my $tmpfile = `dirname $targetfile`;
chop($tmpfile);
$tmpfile .= "/.tmpfile";
open(OUTFILE, ">$tmpfile") or die "Unable to open output file $tmpfile";

my @modules = @ARGV;

# remove BASE
@modules = grep(!/BASE/,@modules);

# put PLATFORM on the bottom
@modules = grep(!/PLATFORM/,@modules);
push @modules, "PLATFORM";

print OUTFILE "/****************************************************\n";
print OUTFILE "* This file is automatically generated.  Do not edit.\n";
print OUTFILE "****************************************************/\n";
print OUTFILE "\n";
print OUTFILE "/* This file is used in linux kernel mode for the proxy layer and for automatic closing on abnormal application termination. */\n";
print OUTFILE "/* The order of modules must be from the highest module (most dependencies) to the lowest (no dependencies). */\n";
print OUTFILE "/* Order is determined by the order listed in platform_modules.inc. */\n";
print OUTFILE "\n";

# reverse order
for ($i=$#modules;$i>=1;$i--) {
    my $moduleLower = lc @modules[$i];
    my $moduleUpper = @modules[$i];

    print OUTFILE "#if NEXUS_HAS\_$moduleUpper\n";
    print OUTFILE "NEXUS_PLATFORM_P_DRIVER_MODULE($moduleLower)\n";
    print OUTFILE "#endif\n";

}
close OUTFILE;

# only overwrite $targetfile if it's different. this avoids needless rebuilds.
system("(test -e $targetfile && diff $tmpfile $targetfile) || cp -f $tmpfile $targetfile; rm $tmpfile");