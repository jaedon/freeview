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
# $brcm_Workfile: bapi_usercall.pm $
# $brcm_Revision: 18 $
# $brcm_Date: 7/9/12 11:29a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/bapi_usercall.pm $
# 
# 18   7/9/12 11:29a erickson
# SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
# 
# 17   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 16   11/1/11 9:57a erickson
# SW7231-391: add typecast
# 
# 15   3/17/11 12:00p erickson
# SW7346-101: fix NEXUS_StopCallbacks for secondary destructors
#  (specifically, NEXUS_Playpump_ClosePidChannel)
#
# 14   2/8/11 10:44a erickson
# SW7420-1123: use separate #ifdef NEXUS_PROXY_THUNK_LAYER for proxy
#  thunk
#
# 13   2/3/11 6:37p vsilyaev
# SW7420-1441: Use local (non proxied) implementation for function that
#  flush CPU cache
#
# 12   1/19/11 3:42p erickson
# SW7420-1123: refactor kernelproxy to correspond to usermode changes
#
# 11   10/14/10 12:34p erickson
# SW7420-1148: add secondary_destructor to support
#  NEXUS_Playpump_ClosePidChannel, verify function attr in one place,
#  support multiple param verifications per ioctl
#
# 10   6/15/10 4:46p erickson
# SW7405-4011: add NEXUS_P_Proxy_StartCallbacks to proxy to clean up stop
#  callbacks list
#
# 9   6/10/10 4:56p vsilyaev
# SW7405-4011: Added StopCallbacks into the driver ioctl code
#
# 8   6/8/10 6:40p vsilyaev
# SW7405-4011: Added user level StopCallbacks call
#
# 7   1/30/08 6:18p vsilyaev
# PR 38682: Added support for callbacks that are passed directly as
#  function arguments
#
# 6   1/28/08 12:33p vsilyaev
# PR 38682: Added support for attribute tags applied to structure
#  members.
#
# 5   1/25/08 2:35p vsilyaev
# PR 38682: Added handling of 'attr' to the function arguments
#
# 4   1/24/08 4:38p vsilyaev
# PR 38682: Improved kernel/proxy environment
#
# 3   1/23/08 9:21p vobadm
# PR35457: update docs
#
# 2   1/23/08 3:14p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/2   1/11/08 4:27p vsilyaev
# PR 38682: Adding kernel support
#
# 5   1/5/07 11:52a erickson
# PR26790: for proxy-layer callback conversion, we must convert back on
# callback out params.
#
# 4   3/15/06 4:51p vsilyaev
# PR20221: NetIF DMA interface
#
# 3   9/12/05 12:35p mphillip
# PR16870: Fix kernel-mode macrovision support
#
# 2   9/8/05 6:23p mphillip
# PR16870: Support for the macrovision data in bdisplay_settings
#
# 1   2/7/05 6:56p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/10   1/26/05 11:04a erickson
# PR13837: convert buffer_base to user address
#
# Irvine_BSEAVSW_Devel/9   7/23/04 2:41p erickson
# PR11771: updated proxy layer for settop api changes
#
# Irvine_BSEAVSW_Devel/8   7/9/04 1:38p erickson
# PR11771: rebuilt proxy layer for settop api dataflow redesign, and
# handle all callbacks with new perl module
#
# Irvine_BSEAVSW_Devel/7   6/24/04 10:24a erickson
# PR11135: added "don't modify" headers
#
# Irvine_BSEAVSW_Devel/6   6/18/04 12:33p erickson
# PR11135: added some timing debug code which can be uncommented if
# needed
#
# Irvine_BSEAVSW_Devel/5   6/17/04 9:37a erickson
# PR11135: recpump callback enabled
#
# Irvine_BSEAVSW_Devel/4   6/16/04 5:16p erickson
# PR11135: added callback support
#
# Irvine_BSEAVSW_Devel/3   6/2/04 2:45p erickson
# PR11204: changed/removed deprecated settop api functions
#
# Irvine_BSEAVSW_Devel/2   6/1/04 11:28a erickson
# PR11135: added comment
#
# Irvine_BSEAVSW_Devel/1   5/17/04 11:14a erickson
# PR11135: added initial linux proxy impl
#
#############################################################################
use strict;
use bapi_common;
use bapi_classes;

package bapi_usercall;

sub generate
{
    my ($filename, $module, $structs, @funcs) = @_;
    my $func;
    my $destructors = bapi_classes::get_destructors \@funcs;
    my $module_lc = lc $module;
    open FILE, ">$filename";

    print FILE bapi_util::header $module;

    print FILE "#define NEXUS_PROXY_THUNK_LAYER\n";
    print FILE "#include \"proxy/nexus_proxy_prologue.h\"\n";
    print FILE "#include \"nexus_${module_lc}_module.h\"\n";
    print FILE "#include \"nexus_core_utils.h\"\n";
    print FILE "BDBG_MODULE(nexus_${module_lc}_proxy);\n";
    print FILE "\n\n";
    print FILE "#define nexus_proxy_module_init nexus_proxy_${module_lc}_init\n";
    print FILE "#define nexus_proxy_module_uninit nexus_proxy_${module_lc}_uninit\n";
    print FILE "#define nexus_proxy_module_state nexus_proxy_${module_lc}_state\n";
    print FILE "#include \"" . (bapi_common::ioctl_header $module) . "\"\n";
    print FILE "#define NEXUS_IOCTL_MODULE_INIT " .  (bapi_common::version_ioctl $module) . "\n";
    print FILE "#define NEXUS_PROXY_MODULE_VERSION " .  (bapi_common::version_define $module) . "\n";
    print FILE "#define NEXUS_PROXY_MODULE_NAME \"$module\"\n";
    print FILE "#include \"proxy/nexus_proxy_body.h\"\n";

    for $func (@funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my $ioctl = bapi_common::ioctl_name $module, $func;
        my $arg = "NULL";
        my $attr = bapi_common::process_function_attributes $func, $structs, \@funcs;

        next if(exists $func->{ATTR}->{'local'}); # local function compiled verbatim into the proxy code

        print FILE "$func->{PROTOTYPE}\n\{\n";
        print FILE "  int rc;\n";
        if ($func->{RETTYPE} eq "NEXUS_Error") {
            print FILE "  NEXUS_Error result=NEXUS_SUCCESS;\n";
        } elsif ($func->{RETTYPE} ne "void") {
            print FILE "  $func->{RETTYPE} result=($func->{RETTYPE})NULL;\n";
        }


        print FILE "  NEXUS_PROXY_ENTER($func->{FUNCNAME});\n";
        my $stopcallbacks_handle = bapi_classes::get_stopcallbacks_handle $func, $destructors;
        if (defined $stopcallbacks_handle) {
            print FILE "  NEXUS_StopCallbacks((void*)$stopcallbacks_handle);\n";
        }

        print FILE "  if(nexus_proxy_module_state.fd) {\n";
        # If no params, no ioctl
        unless ($func->{NOSTRUCT}) {
            my $struct = bapi_common::ioctl_struct $module, $func;

            print FILE "    $struct st;\n";
            bapi_util::print_code \*FILE, $attr->{proxy_vars}, "    ";
            print FILE "\n";

            bapi_util::print_code \*FILE, $attr->{proxy_pre_call}, "    ";

            for $param (@$params) {
                if ($param->{ISREF}) {
                    print FILE "    st.$param->{NAME} = $param->{NAME};\n";
                } else {
                    if($param->{ATTR}->{'memory'}) {
                    }
                    print FILE "    st.$param->{NAME} = $param->{NAME};\n";
                }
            }
            $arg = "(void *)&st";
        }
        print FILE "    rc = ioctl( nexus_proxy_module_state.fd, $ioctl, $arg);\n";
        if ($func->{RETTYPE} eq "NEXUS_Error") {
            print FILE "    if (rc!=0) {result=BERR_TRACE(NEXUS_OS_ERROR);goto done;}\n";
        } else {
            print FILE "    if (rc!=0) {rc=BERR_TRACE(NEXUS_OS_ERROR);goto done;}\n";
        }

        if ($func->{RETTYPE} ne "void") {
            print FILE "    result = st.__retval;\n";
        }

        if (defined $stopcallbacks_handle) {
            print FILE "  NEXUS_StartCallbacks((void*)$stopcallbacks_handle);\n";
        }

        if(exists $attr->{proxy_post_success})  {
            # if postprocessing is required test error codee, and produce postprocessing
            if ($func->{RETTYPE} eq "NEXUS_Error") {
                print FILE "    if(result==NEXUS_SUCCESS) {\n";
            } elsif ($func->{RETTYPE} ne "void") {
                print FILE "    if(result!=NULL) {\n";
            } else {
                print FILE "    {\n";
            }
            bapi_util::print_code \*FILE, $attr->{proxy_post_success}, "    ";
            print FILE "    }\n";
        }

        print FILE "  } else {\n";
        if ($func->{RETTYPE} eq "NEXUS_Error") {
            print FILE "    result=BERR_TRACE(NEXUS_OS_ERROR);goto done;\n";
        } else {
            print FILE "    rc=BERR_TRACE(NEXUS_OS_ERROR);goto done;\n";
        }
        print FILE "  }\n";


        print FILE "\n\n done:\n";

        print FILE "  NEXUS_PROXY_LEAVE($func->{FUNCNAME});\n";
        if ($func->{RETTYPE} ne "void") {
            print FILE "  return result;\n"
        } else {
            print FILE "  return ;\n"
        }
        print FILE "}\n\n\n";
    }
    close FILE;
}

1;

