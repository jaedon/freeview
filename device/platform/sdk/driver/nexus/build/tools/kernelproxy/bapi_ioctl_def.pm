#!/usr/bin/perl
#     (c)2003-2011 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: bapi_ioctl_def.pm $
# $brcm_Revision: 6 $
# $brcm_Date: 12/7/11 10:41a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/bapi_ioctl_def.pm $
# 
# 6   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 5   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 4   3/3/11 11:57a erickson
# SWDTV-5533: support up to 512 ioctl's per nexus module
#
# 3   7/24/08 6:09p vsilyaev
# PR 45137: Modified INIT ioctl to pass application type
#
# 2   1/31/08 12:45p vsilyaev
# PR 38682: Build proxy for the core module
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/3   1/18/08 1:13p vsilyaev
# PR 38682: Updated nexus kernel environment
#
# Nexus_Devel/2   1/11/08 4:27p vsilyaev
# PR 38682: Adding kernel support
#
# 1   2/7/05 6:56p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/3   6/24/04 10:24a erickson
# PR11135: added "don't modify" headers
#
# Irvine_BSEAVSW_Devel/2   6/1/04 11:28a erickson
# PR11135: added comment
#
# Irvine_BSEAVSW_Devel/1   5/17/04 11:13a erickson
# PR11135: added initial linux proxy impl
#
#############################################################################
use strict;
use lib ".";
use bapi_common;
use bapi_util;

package bapi_ioctl_def;

# this function takes an array of function data structures as input
# it prints out the ioctl definitions, both IOCTL numbers and data structures
sub generate
{
    my ($filename, $module, $id, $version, @funcs) = @_;
    my $func;
    open FILE, ">$filename";
    my $cnt = 0;

    print FILE bapi_util::header $module;

    print FILE "/* ioctl dispatch is faster if ioctl number doesn't include size of the structure */\n";
    print FILE "#define NEXUS_IOCTL_$module\_ID  $id\n";
    print FILE "\n\n";
    print FILE "#define " . (bapi_common::version_define $module) . (bapi_common::version_value $version) . "\n";
    print FILE "#define " . (bapi_common::version_ioctl $module) . " NEXUS_IOCTL($id, $cnt, PROXY_NEXUS_ModuleInit)\n\n\n";

    $cnt++;

    for $func (@funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my $ioctl = bapi_common::ioctl_name $module, $func;

        if ($func->{NOSTRUCT}) {
            print FILE "#define $ioctl NEXUS_IOCTL($id, $cnt, int)\n\n";
        } else {
            my $struct = bapi_common::ioctl_struct $module, $func;
            print FILE "typedef struct $struct \{\n";

            # If the RETTYPE is a reference, make it a value

            if ( $func->{RETTYPE} !~ /void\s*\*$/ && $func->{RETTYPE} =~ /\*$/) {
                my $rettype = $func->{RETTYPE};
                $rettype =~ s/const//;
                $rettype =~ s/\*//;
                print FILE "  $rettype __retval;\n";
            } elsif ($func->{RETTYPE} ne "void") {
                print FILE "  $func->{RETTYPE} __retval;\n";
            }

            for $param (@$params) {
                print FILE "  $param->{TYPE} $param->{NAME};\n";
            }
            print FILE "} $struct ;\n\n";
            print FILE "#define $ioctl NEXUS_IOCTL($id, $cnt, $struct)\n\n";
        }
        $cnt++;
    }
    close FILE;
}

1;
