#!/usr/bin/perl
#     (c)2007-2012 Broadcom Corporation
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
# $brcm_Workfile: bapi_thunks.pm $
# $brcm_Revision: 23 $
# $brcm_Date: 10/18/12 10:45a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/syncthunk/bapi_thunks.pm $
# 
# 23   10/18/12 10:45a erickson
# SW7435-396: add temp exception to register keyslot with security module
#  objdb
# 
# 22   9/17/12 3:04p erickson
# SW7435-344: add NEXUS_OBJECT_REGISTER/UNREGISTER to syncthunk for
#  usermode server
# 
# 21   7/26/12 3:16p erickson
# SW7435-235: remove unnecessary symlink
# 
# 20   7/25/12 7:06p vsilyaev
# SW7435-267: Added API statistics for linux kernel
# 
# 19   7/25/12 4:47p vsilyaev
# SW7435-267: Added statistics gathering for API calls
# 
# 18   7/9/12 11:29a erickson
# SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
# 
# 17   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 16   10/31/11 6:13p bandrews
# SW7231-391: casts to support parser band and timebase as enums
# 
# SW7420-2078/1   10/11/11 8:22p bandrews
# SW7231-391: added casts to support dual variant usage of parser band
#  and timebase
# 
# 15   9/2/11 10:03a erickson
# SW7420-1148: allow nexus_register/unregister to objdb from nexus
#  module. enables more complex resource management.
# 
# 14   7/13/11 3:43p erickson
# SW7420-1979: #include nexus_class_verification.h is always required
#  because of automatic shutdown
# 
# 13   7/12/11 1:27p erickson
# SW7420-1979: add support for new shutdown attribute
# 
# 12   3/17/11 12:00p erickson
# SW7346-101: fix NEXUS_StopCallbacks for secondary destructors
#  (specifically, NEXUS_Playpump_ClosePidChannel)
#
# 11   1/27/11 3:05p erickson
# SW7208-152: fix and standardize get_destructor_hash
#
# 10   1/27/11 1:43p erickson
# SW7208-152: fix generation of NEXUS_StopCallbacks
#
# 9   1/19/11 2:02p erickson
# SW7420-1123: socket-based usermode IPC
#
# 8   9/22/09 1:54p gmohile
# SW7405-3060 : Export kernel symbols
#
# 7   3/28/08 10:47a vsilyaev
# PR 40863: Call NEXUS_StopCalbacks prior to the 'destructor' call, since
#  StopCallback is sticky it would prevent further callback to fire
#
# 6   3/24/08 6:58p vsilyaev
# PR 40863: Changed order of calls to NEXUS_StopCallbacks and
#  "destructors" in ordder to stop callbacks that were fired during call
#  to the destructor function
#
# 5   3/24/08 6:42p vsilyaev
# PR 40863: Added injection of NEXUS_StopCallbacks into all destructors
#
# 4   3/21/08 11:57a vsilyaev
# PR 38682: Fixed typo in description
#
# 3   1/25/08 4:24p vsilyaev
# PR 38682: Reduce code size of the generated code
#
# 2   1/24/08 4:34p vsilyaev
# PR 38682: Reduced size and made optional tracing of thunk layer
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/5   10/15/07 2:53p vsilyaev
# PR 35824: Fixed handling of (void) functions
#
# Nexus_Devel/4   10/12/07 11:49a erickson
# PR36066: added "or die" to file opens
#
# Nexus_Devel/3   10/8/07 3:21p erickson
# PR35395: added params and retval to trace
#
# Nexus_Devel/2   10/3/07 12:14p erickson
# PR35395: enable thunk trace
#
# Nexus_Devel/1   9/27/07 1:33p erickson
# PR35395: initial impl
#
#############################################################################
use strict;

package bapi_thunks;

use bapi_util;
use bapi_classes;

# for user-mode IPC, server-side does not pass through the IPC thunk. Therefore a minimal amount of
# hook is needed in the one thunk that is passed: the sync thunk.
sub generate_ipc_code
{
    my ($func, $destructors, $classes) = @_;
    my @server_pre_lock;
    my @server_post_lock;
    my @server_post_success;
    my $class;

    CLASS: for $class (@$classes) {
        if ($class->{DESTRUCTOR} == $func) {
            my $params = $func->{PARAMS};
            my $handle = $$params[0]->{NAME};
            for (@{$class->{SHUTDOWN}}) {
                my $shutdown_target = $_->{'shutdown_target'};
                my $shutdown_get_connector = $_->{'shutdown_get_connector'};
                push @server_pre_lock, "nexus_driver_shutdown_$shutdown_target($shutdown_get_connector($handle));";
            }
            if ($class->{CLASS_NAME} eq "NEXUS_KeySlot") {
                # TEMP: keyslot contructor in core, but objdb in security
                push @server_post_lock, "NEXUS_BaseObject_P_RegisterUnregister((void*)keyHandle, &NEXUS_OBJECT_DESCRIPTOR(NEXUS_KeySlot), NEXUS_Object_P_RegisterUnregister_eAutoDestroy, NEXUS_MODULE_SELF, NEXUS_MODULE_SELF);";
            }
            else {
                # NEXUS_OBJECT_UNREGISTER with eAutoDestroy
                push @server_post_lock, "$class->{CLASS_NAME}_BaseObject_P_RegisterUnregister((void*)$handle, NEXUS_Object_P_RegisterUnregister_eAutoDestroy, NEXUS_MODULE_SELF);";
            }
        }
        for (@{$class->{CONSTRUCTORS}}) {
            if($_ == $func) {
                if ($class->{CLASS_NAME} eq "NEXUS_KeySlot") {
                    # TEMP: keyslot contructor in core, but objdb in security
                    push @server_post_success, "NEXUS_BaseObject_P_RegisterUnregister((void*)result, &NEXUS_OBJECT_DESCRIPTOR(NEXUS_KeySlot), NEXUS_Object_P_RegisterUnregister_eAutoCreate, NEXUS_MODULE_SELF, NEXUS_MODULE_SELF);";
                }
                else {
                    # NEXUS_OBJECT_REGISTER with eAutoCreate
                    push @server_post_success, "$class->{CLASS_NAME}_BaseObject_P_RegisterUnregister((void*)result, NEXUS_Object_P_RegisterUnregister_eAutoCreate, NEXUS_MODULE_SELF);";
                }
            }
        }
    }
    
    my $stopcallbacks_handle = bapi_classes::get_stopcallbacks_handle $func, $destructors;
    if (defined $stopcallbacks_handle) {
        push @server_pre_lock, "NEXUS_StopCallbacks((void*)$stopcallbacks_handle);";
    }
    
    my %result;
    $result{'server_pre_lock'} = \@server_pre_lock;
    $result{'server_post_lock'} = \@server_post_lock;
    $result{'server_post_success'} = \@server_post_success;
    return \%result;
}

sub build_thunks
{
    my ($module, $filename, $funcs) = @_;
    my $func;
    my $destructors = bapi_classes::get_destructors $funcs;
    my $classes = bapi_classes::get_classes $funcs, $destructors;

    open FILE, ">$filename" or die;
    print FILE "/*********************************\n";
    print FILE "*\n";
    print FILE "* This file is autogenerated by the Nexus Platform makefile.\n";
    print FILE "*\n";
    print FILE "* This file acquires the module lock for every public API call into that module. The actual implementation of each function is remapped to _impl.\n";
    print FILE "*\n";
    print FILE "*********************************/\n";
    print FILE "#define NEXUS_THUNK_LAYER\n";
    print FILE "#include \"nexus_${module}_module.h\"\n";
    print FILE "#include \"nexus_core_utils.h\"\n";
    print FILE "#include \"nexus_class_verification.h\"\n";
    print FILE "#include \"nexus_base_statistics.h\"\n";
    print FILE "BDBG_MODULE(nexus_${module}_thunks);\n";
    print FILE "#define BDBG_MSG_TRACE(x) \n";
    print FILE "/* use local static function in order to reduce size of compiled code, size would reduce since NEXUS_XXXModule expanded to function call with three arguments (where at least one is a global symbol) */\n";
    print FILE "static void module_lock(void) { NEXUS_LockModule();}\n";
    print FILE "static void module_unlock(void) { NEXUS_UnlockModule();}\n";


    # generate prototypes for all the _impl functions
    for $func (@{$funcs}) {
        # NOTE: If there's an exception and a function does not belong in the thunk layer, add it here.
        next if(exists $func->{ATTR}->{'local'});

        my $impl = $func->{PROTOTYPE};
        $impl =~ s/$func->{FUNCNAME}/$func->{FUNCNAME}_impl/;
        print FILE "$impl;\n";
    }

    # generate the actual thunk layer functions which call the impl functions
    for $func (@{$funcs}) {
        # NOTE: If there's an exception and a function does not belong in the thunk layer, add it here.
        next if(exists $func->{ATTR}->{'local'});
        
        my $params = $func->{PARAMS};
        my $param;
        my $generated_code = generate_ipc_code $func, $destructors, $classes;

        print FILE "$func->{PROTOTYPE}\n\{\n";
        if ($func->{RETTYPE} eq "void") {
        }
        else {
            print FILE "  $func->{RETTYPE} result;\n";
        }
        print FILE "    NEXUS_P_API_STATS_STATE();\n";
        print FILE "    NEXUS_P_API_STATS_START(); /* this statistics count all API call overhead (including synchronization overhead) */\n";
        # enter MSG
        print FILE "  BDBG_MSG_TRACE((\">%s\(";
        for $param (@$params) {
            print FILE "%#lx";
            if ($param != $params->[-1]) { print FILE ", "; }
        }
        print FILE ")\" ";
        print FILE ", \"$func->{FUNCNAME}\"";
        for $param (@$params) {
            print FILE ", ";
            print FILE "(unsigned long)$param->{NAME}";
        }
        print FILE "));\n";

        if (scalar @{$generated_code->{'server_pre_lock'}}) {
            my $file = \*FILE;
            bapi_util::print_code $file, $generated_code->{'server_pre_lock'}, "  ";
        }

        # make call
        print FILE "  module_lock();\n";

        if (scalar @{$generated_code->{'server_post_lock'}}) {
            my $file = \*FILE;
            bapi_util::print_code $file, $generated_code->{'server_post_lock'}, "  ";
        }
        
        if ($func->{RETTYPE} eq "void") {
            print FILE "  $func->{FUNCNAME}_impl(";
        }
        else {
            print FILE "  result = $func->{FUNCNAME}_impl(";
        }
        for $param (@$params) {
            print FILE "$param->{NAME}";
            if ($param != $params->[-1]) { print FILE ", "; }
        }
        print FILE ");\n";

        # leave MSG and return value
        if ($func->{RETTYPE} eq "void") {
            print FILE "  module_unlock();\n";
            print FILE "  BDBG_MSG_TRACE((\"<%s\", \"$func->{FUNCNAME}\"));\n";
            print FILE "  NEXUS_P_API_STATS_STOP(\"$func->{FUNCNAME}\",NEXUS_MODULE_SELF); /* this statistics count all API call overhead */\n";
            print FILE "  return;\n";
        }
        else {
            if (scalar @{$generated_code->{'server_post_success'}}) {
                if ($func->{RETTYPE} eq "NEXUS_Error") {
                    print FILE "  if (!result) {\n";
                }
                else {
                    print FILE "  if (result) {\n";
                }
                my $file = \*FILE;
                bapi_util::print_code $file, $generated_code->{'server_post_success'}, "    ";
                print FILE "  }\n";
            }
            print FILE "  module_unlock();\n";
            print FILE "  BDBG_MSG_TRACE((\"<%s\=%#lx\", \"$func->{FUNCNAME}\", (unsigned long)result));\n";
            print FILE "  NEXUS_P_API_STATS_STOP(\"$func->{FUNCNAME}\", NEXUS_MODULE_SELF); /* this statistics count all API call overhead */\n";
            print FILE "  return result;\n";
        }
        print FILE "}\n\n";
    }
    close FILE;
}

sub build_remapping
{
    my ($filename, $funcs) = @_;
    my $func;
    open FILE, ">$filename" or die;

    print FILE "/*********************************\n";
    print FILE "*\n";
    print FILE "* This file is autogenerated by the Nexus Platform makefile.\n";
    print FILE "*\n";
    print FILE "* This file remaps every public API function to _impl. This allows the Nexus modules to call their own public API without reacquiring the module lock.\n";
    print FILE "*\n";
    print FILE "*********************************/\n";
    print FILE "#ifndef NEXUS_THUNK_LAYER\n";

    for $func (@{$funcs}) {
        # NOTE: If there's an exception and a function does not belong in the thunk layer, add it here.
        next if(exists $func->{ATTR}->{'local'});
        
        print FILE "#define $func->{FUNCNAME} $func->{FUNCNAME}_impl\n";
    }
    print FILE "#endif\n";
    print FILE "\n";
    close FILE;
}


1;
