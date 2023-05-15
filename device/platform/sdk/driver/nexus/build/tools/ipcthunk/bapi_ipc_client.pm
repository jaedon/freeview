#!/usr/bin/perl
#     (c)2004-2012 Broadcom Corporation
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
# $brcm_Workfile: bapi_ipc_client.pm $
# $brcm_Revision: 14 $
# $brcm_Date: 8/31/12 2:54p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/ipcthunk/bapi_ipc_client.pm $
# 
# 14   8/31/12 2:54p erickson
# SW7425-3418: rework user mode callbacks: filter redundant callbacks on
#  server. sync NEXUS_StopCallbacks across all callback threads.
# 
# 13   7/9/12 11:30a erickson
# SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
# 
# 12   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 11   10/31/11 6:13p bandrews
# SW7231-391: casts to support parser band and timebase as enums
# 
# SW7420-2078/1   10/11/11 8:21p bandrews
# SW7231-391: added casts to support parser band and timebase dual
#  variants
# 
# 10   8/8/11 3:40p erickson
# SW7420-1123: if IPC fails, return failure return code
# 
# 9   7/29/11 12:10p erickson
# SW7420-1991: fix nelem,nelem_out attribute support, reorganize to be
#  closer to kernelproxy/bapi_common.pm layout
# 
# 8   4/12/11 4:25p erickson
# SW7420-1123: reduce size of inparams
#
# 7   3/17/11 11:59a erickson
# SW7346-101: fix NEXUS_StopCallbacks for secondary destructors
#  (specifically, NEXUS_Playpump_ClosePidChannel)
#
# 6   3/11/11 1:04p erickson
# SW7420-1123: add NEXUS_PROXY_THUNK_LAYER
#
# 5   2/16/11 2:17p erickson
# SW7420-1123: implement nelem/nelem_out for IPC thunk
#
# 4   2/15/11 2:28p erickson
# SW7420-1123: refactor IPC thunk to separate in/out params
#
# 3   2/9/11 11:43a erickson
# SW7420-1441: support attr{local=true}
#
# 2   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 1   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
#
#############################################################################
use strict;
use bapi_classes;

package bapi_ipc_client;

sub generate
{
    my ($filename, $module, $version, $structs, @funcs) = @_;
    my $func;
    my $destructors = bapi_classes::get_destructors \@funcs;
    my $module_lc = lc $module;

    open FILE, ">$filename";

    print FILE bapi_util::header $module;

    print FILE "#define NEXUS_PROXY_THUNK_LAYER\n";
    print FILE "#include \"client/nexus_client_prologue.h\"\n";
    print FILE "#include \"nexus_${module_lc}_module.h\"\n";
    print FILE "#include \"nexus_core_utils.h\"\n";
    print FILE "BDBG_MODULE(nexus_${module_lc}_ipc_client);\n";
    print FILE "\n\n";
    print FILE "#define nexus_client_module_init nexus_client_${module_lc}_init\n";
    print FILE "#define nexus_client_module_uninit nexus_client_${module_lc}_uninit\n";
    print FILE "#define nexus_client_module_state nexus_client_${module_lc}_state\n";
    print FILE "#include \"" . (bapi_common::ipc_header $module) . "\"\n";
    print FILE "#define NEXUS_IPC_MODULE_INIT " .  (bapi_common::version_ipc $module) . "\n";
    print FILE "#define NEXUS_CLIENT_MODULE_VERSION " .  (bapi_common::version_define $module) . "\n";
    print FILE "#define NEXUS_CLIENT_MODULE_NAME \"$module\"\n";
    print FILE "#include \"client/nexus_client_body.h\"\n";

    for $func (@funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my $attr = bapi_common::process_function_attributes $func, $structs, \@funcs;

        next if(exists $func->{ATTR}->{'local'}); # local function compiled verbatim into the proxy code

        print FILE "$func->{PROTOTYPE}\n\{\n";
        print FILE "    unsigned __data_size, __n;\n";
        print FILE "    unsigned __variable_in_offset = 0;\n";
        print FILE "    void *temp;\n";
        print FILE "    NEXUS_Error __rc;\n";
        if ($func->{RETTYPE} eq "NEXUS_Error") {
            print FILE "    NEXUS_Error __result=NEXUS_UNKNOWN;\n";
        } elsif ($func->{RETTYPE} ne "void") {
            print FILE "    $func->{RETTYPE} __result=($func->{RETTYPE})NULL;\n";
        }

        print FILE "    " . (bapi_common::ipc_block $module) . " *in_data, *out_data;\n";
        bapi_util::print_code \*FILE, $attr->{proxy_vars}, "    ";
        print FILE "\n";
        print FILE "    NEXUS_CLIENT_ENTER($func->{FUNCNAME});\n";
        print FILE "    BSTD_UNUSED(__variable_in_offset); /* may be unused */\n";
        
        print FILE "\n";
        print FILE "    /* coverity[unreachable]  dummy jump to prevent warning */\n";
        print FILE "    if(0) {goto err_alloc;}\n";
        print FILE "\n";
        my $stopcallbacks_handle = bapi_classes::get_stopcallbacks_handle $func, $destructors;
        if (defined $stopcallbacks_handle) {
            print FILE "    NEXUS_StopCallbacks((void *)$stopcallbacks_handle);\n";
        }
        print FILE "    __rc = NEXUS_P_Client_LockModule(nexus_client_module_state, &temp, &__data_size);\n";
        print FILE "    if (__rc) {__rc=BERR_TRACE(__rc);goto err_lock;}\n";
        # client uses same memory for in & out params, but retains separate in_data/out_data names for compatibility w/ server side auto-gen code
        print FILE "    out_data = in_data = temp;\n";

        print FILE "\n";
        print FILE "    in_data->header.function_id = " . (bapi_common::ipc_name $module, $func) . ";\n";
        print FILE "    in_data->header.version = " . (bapi_common::version_define $module) . ";\n";
        bapi_util::print_code \*FILE, $attr->{proxy_pre_call}, "    ";
        
        print FILE "    __n = sizeof(in_data->header)+sizeof(in_data->data.$func->{FUNCNAME}.in);\n";
        # subtract sizeof(void*) because __variable_in_offset double counts the size of in.variable_params. 
	# HUMAX_PLATFORM_BASE using server and client mode, occur i2c error because of under the code.
        print FILE "    if (__variable_in_offset >= sizeof(void*)) __n += __variable_in_offset - sizeof(void*);\n";
        print FILE "    __rc = NEXUS_P_Client_CallServer(nexus_client_module_state, in_data, __n, out_data, __data_size, &__n);\n";
        if ($func->{RETTYPE} eq "NEXUS_Error") {
            print FILE "    if (__rc!=NEXUS_SUCCESS) {__result=BERR_TRACE(__rc);goto err_call;}\n";
        }
        else {
            print FILE "    if (__rc!=NEXUS_SUCCESS) {__rc=BERR_TRACE(__rc);goto err_call;}\n";
        }
        print FILE "    if (__n<sizeof(out_data->header)+sizeof(out_data->data.$func->{FUNCNAME}.out)) {__rc=BERR_TRACE(-1);goto err_call;}\n";
        
        bapi_util::print_code \*FILE, $attr->{proxy_post_success}, "    ";
        
        if ($func->{RETTYPE} ne "void") {
            print FILE "    __result = out_data->data.$func->{FUNCNAME}.out.__retval;\n";
        }

        print FILE "\n";
        print FILE "err_call:\n";
        print FILE "    NEXUS_P_Client_UnlockModule(nexus_client_module_state);\n";
        print FILE "err_lock:\n";
        if (defined $stopcallbacks_handle) {
            print FILE "    NEXUS_StartCallbacks((void *)$stopcallbacks_handle);\n";
        }
        print FILE "err_alloc:\n";
        print FILE "    NEXUS_CLIENT_LEAVE($func->{FUNCNAME});\n";
        if ($func->{RETTYPE} ne "void") {
            print FILE "    return __result;\n"
        } else {
            print FILE "    return;\n"
        }

        print FILE "}\n\n";

    }
    close FILE;
}

1;

