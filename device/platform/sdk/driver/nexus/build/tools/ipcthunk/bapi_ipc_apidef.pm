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
# $brcm_Workfile: bapi_ipc_apidef.pm $
# $brcm_Revision: 4 $
# $brcm_Date: 12/7/11 10:41a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/ipcthunk/bapi_ipc_apidef.pm $
# 
# 4   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 3   2/15/11 2:28p erickson
# SW7420-1123: refactor IPC thunk to separate in/out params
#
# 2   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 1   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
#
#############################################################################
use strict;
use bapi_common;
use bapi_util;

package bapi_ipc_apidef;

sub generate_ipc_block
{
    my ($file, $module, $funcs) = @_;
    my $func;
    my $block = bapi_common::ipc_block $module;
    my $module_lc = lc $module;


    print $file "/* structure to store copy of all in and out parameters */\n";
    print $file "typedef struct $block {\n";
    print $file "   NEXUS_Ipc_Header    header;\n";
    print $file "   union {\n";
    for $func (@$funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my @struct;
        #if (!$func->{NOSTRUCT})
        {
            my $struct = bapi_common::ipc_struct $module, $func;
            print $file "       $struct $func->{FUNCNAME};  \n";
        }
    }
    print $file "   } data; \n";
    print $file "} $block;\n";
}

# this function takes an array of function data structures as input
# it prints out structure definitions for IPC
sub generate
{
    my ($filename, $module, $version, @funcs) = @_;
    my $func;
    open FILE, ">$filename";
    my $cnt = 0;

    print FILE bapi_util::header $module;

    print FILE "\n\n";
    print FILE "#define " . (bapi_common::version_define $module) . (bapi_common::version_value $version) . "\n";

    $cnt++;

    for $func (@funcs) {
        my $funcname = $func->{FUNCNAME};
        my $params = $func->{PARAMS};
        my $param;
        my $ipc_name = bapi_common::ipc_name $module, $func;
        my $struct = bapi_common::ipc_struct $module, $func;
        my @members;

        # in params
        push @members, "struct {";
        for $param (@$params) {
            if ($param->{INPARAM}) {
                my $info = bapi_common::process_function_param $func, $param;
                bapi_util::append_code \@members, $info->{'ipc_inparams'}, "  ";
            }
        }
        push @members, "  uint8_t *variable_params;";
        push @members, "} in;";

        # out params, including retval
        push @members, "struct {";
        # If the RETTYPE is a reference, make it a value
        if ( $func->{RETTYPE} !~ /void\s*\*$/ && $func->{RETTYPE} =~ /\*$/) {
            my $rettype = $func->{RETTYPE};
            $rettype =~ s/const//;
            $rettype =~ s/\*//;
            push @members, "  $rettype __retval;";
        } elsif ($func->{RETTYPE} ne "void") {
            push @members, "  $func->{RETTYPE} __retval;";
        }

        for $param (@$params) {
            if (!$param->{INPARAM}) {
                my $info = bapi_common::process_function_param $func, $param;
                bapi_util::append_code \@members, $info->{'ipc_outparams'}, "  ";
            }
        }
        push @members, "  uint8_t *variable_params;";
        push @members, "} out;";

        print FILE "#define $ipc_name NEXUS_IPC_ID($module,$cnt)\n\n";
        print FILE "typedef union $struct \{\n";
        bapi_util::print_code \*FILE, \@members, "    ";
        print FILE "} $struct ;\n\n";

        $cnt++;
    }
    generate_ipc_block  \*FILE, $module, \@funcs;
    close FILE;
}

1;
