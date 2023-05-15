#!/usr/bin/perl
#     (c)2003-2012 Broadcom Corporation
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
# $brcm_Workfile: bapi_driver_ioctl.pm $
# $brcm_Revision: 61 $
# $brcm_Date: 10/11/12 12:15p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/bapi_driver_ioctl.pm $
# 
# 61   10/11/12 12:15p vsilyaev
# SW7420-2324: Don't verify twice and use faster path for local objects
# 
# 60   10/5/12 4:58p bandrews
# SW7425-4040: do not convert return value between eInvalid and NULL
# 
# 59   8/22/12 1:23p erickson
# SW7231-958: remove function restriction
# 
# 58   7/23/12 11:35a vsilyaev
# SW7420-2328: Use per-thread storage to keep information about the
#  current client, remove per-module client information
# 
# 57   7/18/12 5:48p vsilyaev
# SW7420-2324: Properly handle case for timebase and parser bands
# 
# 56   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 55   6/19/12 12:41p vsilyaev
# SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
# 
# 54   3/21/12 2:32p erickson
# SW7340-296: remove unused secondary_destructor attribute
# 
# 53   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 52   11/7/11 3:59p bandrews
# SW7231-446: merge to main
# 
# SW7231-446/2   11/4/11 9:44p bandrews
# SW7231-446: fix based on review
# 
# SW7231-446/1   11/3/11 9:27p bandrews
# SW7231-446: implemented
# 
# 51   11/1/11 9:57a erickson
# SW7231-391: add typecast
# 
# 50   11/1/11 9:29a erickson
# SW7231-391: add typecast for nexus_driver_verify_<handletype> calls
# 
# 49   10/17/11 1:09p erickson
# SW7418-40: fix size of copy_from_user for main ioctl struct
# 
# 48   9/20/11 11:02a erickson
# SW7420-1148: check attr{null_allowed} for NEXUS_HeapHandle function
#  param
# 
# 47   9/13/11 1:23p erickson
# SW7420-1148: change client modes to unprotected/protected/untrusted
# 
# 46   9/13/11 10:56a erickson
# SW7420-1689: NEXUS_HeapHandle is validated, even though it is not a
#  class
# 
# 45   9/2/11 12:39p erickson
# SW7420-1148: refactor class_list.pl to generate per-module "get current
#  client" functions
# 
# 44   9/2/11 10:03a erickson
# SW7420-1148: allow nexus_register/unregister to objdb from nexus
#  module. enables more complex resource management.
# 
# 43   8/31/11 12:18p erickson
# SW7231-357: don't generate server-side code for attr{local=true}
#  functions
# 
# 42   8/23/11 4:13p erickson
# SW7420-1992: rename to surface_compositor
# 
# 41   8/22/11 2:14p erickson
# SW7420-1689: back out last change. will require more work.
# 
# 40   8/11/11 11:31a erickson
# SW7420-1689: must do class_verification before driver_pre_call to avoid
#  leaks if not verified
# 
# 39   8/8/11 5:10p erickson
# SW7420-1689: add untrusted client api enforcement
# 
# 38   8/3/11 9:57a erickson
# SW7420-1992: add per-display framebuffer heap to
#  NEXUS_SurfaceCmpSettings
# 
# 37   8/2/11 4:25p erickson
# SW7420-1979: autogen nexus_driver_shutdown_close, used by class table
#  and ioctl, to process SHUTDOWN attribute prior to close
# 
# 36   7/29/11 12:13p erickson
# SW7420-1991: reorganize to allow ipcthunk/bapi_common.pm layout to be
#  closer
# 
# 35   7/12/11 1:26p erickson
# SW7420-1979: add support for new shutdown attribute
# 
# 34   5/16/11 4:14p erickson
# SW7420-1679: NEXUS_Memory_Free must check pointer with objdb before
#  freeing
# 
# 33   5/13/11 12:17p erickson
# SW7420-1873: remove shutdown attribute
# 
# 32   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 31   4/14/11 11:59a erickson
# SW7420-1679: clean up NEXUS_Memory_Allocate using
#  attr{dealloc=NEXUS_Memory_Free}
#
# 30   4/7/11 5:23p erickson
# SW7420-1671: use BERR_TRACE in proxy
#
# 29   4/1/11 9:59a erickson
# SW7420-1671: refactor object database for multiprocess stress
#
# 28   3/3/11 11:57a erickson
# SWDTV-5533: support up to 512 ioctl's per nexus module
#
# 27   2/15/11 2:16p erickson
# SW7420-1148: support NEXUS_HeapHandle as func param, add error line
#  numbers
#
# 26   1/28/11 9:03a erickson
# SW7420-1440: refactor secondary_destructor
#
# 25   1/19/11 3:42p erickson
# SW7420-1123: refactor kernelproxy to correspond to usermode changes
#
# 24   1/19/11 2:02p erickson
# SW7420-1123: socket-based usermode IPC
#
# 23   12/14/10 4:37p erickson
# SW7420-1285: null_allowed attribute is now required
#
# 22   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 21   10/14/10 5:05p erickson
# SW7420-1148: remove leftover debug
#
# 20   10/14/10 12:34p erickson
# SW7420-1148: add secondary_destructor to support
#  NEXUS_Playpump_ClosePidChannel, verify function attr in one place,
#  support multiple param verifications per ioctl
#
# 19   10/12/10 4:29p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 18   10/7/10 11:22a erickson
# SW7420-1148: add client_id
#
# 17   6/15/10 4:46p erickson
# SW7405-4011: remove NEXUS_StopCallbacks in driver that can deadlock
#
# 16   6/10/10 4:56p vsilyaev
# SW7405-4011: Added StopCallbacks into the driver ioctl code
#
# 15   5/13/10 3:59p jtna
# SW7125-306: Coverity Defect ID:20562 USE_AFTER_FREE
#
# 14   2/5/10 4:02p jgarrett
# SW7125-205: Fixing loop generating CREATE vs. USE code.  It was
#  generating CREATE entries for everything and causing memory leaks on
#  GetConnector.
#
# 13   7/24/08 6:09p vsilyaev
# PR 45137: Modified INIT ioctl to pass application type
#
# 12   2/7/08 1:01p vsilyaev
# PR 38682: Fixed check for destructor type
#
# 11   2/4/08 2:39p vsilyaev
# PR 38682: Added support for pointers with variable size
#
# 10   2/1/08 5:02p vsilyaev
# PR 38682: Added tracking of open/close calls
#
# 9   1/31/08 5:02p vsilyaev
# PR 38682: Added function level attributes
#
# 8   1/30/08 6:18p vsilyaev
# PR 38682: Added support for callbacks that are passed directly as
#  function arguments
#
# 7   1/30/08 11:49a vsilyaev
# PR 38682: Added routing of callbacks into the user space
#
# 6   1/28/08 12:51p vsilyaev
# PR 38682: Fixed redundant mapping of addreses inside structures
#
# 5   1/28/08 12:33p vsilyaev
# PR 38682: Added support for attribute tags applied to structure
#  members.
#
# 4   1/25/08 4:24p vsilyaev
# PR 38682: Reduce code size of the generated code
#
# 3   1/25/08 2:35p vsilyaev
# PR 38682: Added handling of 'attr' to the function arguments
#
# 2   1/24/08 4:30p vsilyaev
# PR 38682: Improved kernel/proxy environment
#
# 1   1/18/08 2:14p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/3   1/18/08 1:13p vsilyaev
# PR 38682: Updated nexus kernel environment
#
#############################################################################
use strict;

package bapi_driver_ioctl;

sub generate_class_code_post
{
    my ($classes, $func, $args) = @_;
    CLASS: for (@$classes) {
        my $class = $_;
        my $id=$class->{CLASS_NAME};
        for (@{$class->{CONSTRUCTORS}}) {
            if($_ == $func) {
                if($class->{DESTRUCTOR_TYPE} eq 'destructor') {
                    return ["NEXUS_DRIVER_CREATE_OBJECT($id, $args.ioctl.__retval);"];
                }
                last CLASS;
            }
        }
        if($class->{ACQUIRE} == $func) {
            my $param =  @{$func->{PARAMS}}[0];
            return ["NEXUS_DRIVER_ACQUIRE_OBJECT($id, $args.ioctl.__retval);"];
        }
    }
    return undef;
}

sub generate_class_code_pre
{
    my ($classes, $func, $args) = @_;
    CLASS: for (@$classes) {
        my $class = $_;
        my $id=$class->{CLASS_NAME};
        my $bypass=0;
        my $shutdown = "";
    
        if($class->{DESTRUCTOR} == $func) {
            if(exists $class->{SHUTDOWN}) {
                # for destructors with shutdown attributes, we must call the auto-generated shutdown_close function
                # instead of directly calling the normal close function.
                my $classtype = $class->{CLASS_TYPE};
                my $params = $func->{PARAMS};
                my $handle = "$args.ioctl.$$params[0]->{NAME}";
                $bypass = 1;
                $shutdown = "nexus_driver_shutdown_close_$classtype($handle);";
            }
            my $param =  @{$func->{PARAMS}}[0];
            return ($bypass,["NEXUS_DRIVER_DESTROY_OBJECT($id, $args.ioctl.$param->{NAME});",$shutdown]);
        }
        if($class->{RELEASE} == $func) {
            my $param =  @{$func->{PARAMS}}[0];
            return ($bypass,["NEXUS_DRIVER_RELEASE_OBJECT($id, $args.ioctl.$param->{NAME});"]);
        }
    }
    return (0,undef);
}

# check if any function parameter or struct member is a class handle.
# if so, generate code to verify the handle matches the instance and client.
# also, check NEXUS_HeapHandle as a special case
sub generate_class_verification
{
    my ($classes, $structs, $func, $args, $class_handles, $untrusted_api) = @_;
    my @verify_lines;
    my @post_verify_lines;
    my @undo_lines;

    # if untrusted, verify that this function can be called
    if (!bapi_classes::callable_by_untrusted($func, $classes, $untrusted_api)) {
        push @verify_lines, "if (CLIENT_ID->limited_api) {BERR_TRACE(-1); goto err_fault;}";
    }

    for (@{$func->{PARAMS}}) {
        my $param = $_;
        my $handletype;
        my $param_field = "$args.ioctl.$param->{NAME}";

        # check each class that was defined in this module
        for (@$classes) {
            my $class = $_;
            $handletype = $class->{CLASS_TYPE};

            # check if param is class handle
            if ($param->{TYPE} eq $handletype) {
                my $class_name = $class->{CLASS_NAME};
                my $typecast = ($class_name eq $handletype) ? "(void *)" : "";
                push @verify_lines, "if (NEXUS_OBJECT_VERIFY($class_name,$typecast ${param_field})) {BERR_TRACE(-1); goto err_fault;}";
            }
        }

        foreach $handletype (@$class_handles) {
            #print "generate_class_verification $handletype\n";
            # check is param is a struct which has a class handle field
            if ($param->{ISREF} && $param->{INPARAM} && exists $structs->{$param->{BASETYPE}}) {
                my $struct_field;
                for $struct_field (@{$structs->{$param->{BASETYPE}}}) {
                    if ($struct_field->{TYPE} eq $handletype) {
                        my $class_name = bapi_classes::get_class_name($handletype);
                        my $struct_param = "$args.$param->{NAME}.$struct_field->{NAME}";
                        my $typecast = ($class_name eq $handletype) ? "(void *)" : "";
                        my $local_class = 0;
                        for (@$classes) {
                            if($handletype eq $_->{CLASS_TYPE}) {
                                $local_class = 1; last;
                            }
                        }
                        # verify non-NULL params
                        push @verify_lines, "if(${param_field} && $struct_param) {";
                        if($local_class) {
                            push @verify_lines, "  if (NEXUS_OBJECT_VERIFY($class_name,$typecast $struct_param)) {BERR_TRACE(-1); goto err_fault;}";
                        } else {
                            push @verify_lines, "  if (NEXUS_OBJECT_VERIFY_AND_ACQUIRE($class_name, $typecast $struct_param)) {BERR_TRACE(-1); goto err_fault;}";
                        }
                        push @verify_lines, "}";
                        if(!$local_class) {
                            push @post_verify_lines, "if(${param_field} && $struct_param) {";
                            push @post_verify_lines, "  NEXUS_OBJECT_RELEASE_AFTER_VERIFY($class_name, $typecast $struct_param);";
                            push @post_verify_lines, "}";
                        }
                    }
                }
             # second, check if param is a class handle field
            } elsif (!$param->{ISREF} && $param->{INPARAM}) {
                if ($param->{TYPE} eq $handletype) {
                    my $local_class = 0;
                    for (@$classes) {
                        if($handletype eq $_->{CLASS_TYPE}) {
                            $local_class = 1; last;
                        }
                    }
                    if(!$local_class) {
                        my $class_name = bapi_classes::get_class_name($handletype);
                        my $typecast = ($class_name eq $handletype) ? "(void *)" : "";
                        # verify non-NULL params
                        #
                        push @verify_lines, "if(${param_field}) {";
                        push @verify_lines, "  if (NEXUS_OBJECT_VERIFY_AND_ACQUIRE($class_name, $typecast ${param_field})) {BERR_TRACE(-1); goto err_fault;}";
                        push @verify_lines, "}";
                        push @post_verify_lines, "if(${param_field}) {";
                        push @post_verify_lines, "  NEXUS_OBJECT_RELEASE_AFTER_VERIFY($class_name, $typecast ${param_field});";
                        push @post_verify_lines, "}";
                    }
                }
            }
        }

        # check for timebase or parser band as struct member
        # this code only checks input params
        # output params post call are checked in bapi_common.pm
        # TODO: doesn't support members as pointers
        if ($param->{ISREF} && $param->{INPARAM} && exists $structs->{$param->{BASETYPE}}) {
            my $struct_field;
            for $struct_field (@{$structs->{$param->{BASETYPE}}}) {
                if ($struct_field->{TYPE} eq 'NEXUS_Timebase' || $struct_field->{TYPE} eq 'NEXUS_ParserBand') {
                    my $struct_param = "$args.$param->{NAME}.$struct_field->{NAME}";
                    push @verify_lines, "if(nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @verify_lines, "  if(${param_field}) {"; #ioctl pointer check replaces _isnull check
                    push @verify_lines, "    if ((void *)$struct_param == NULL) {";
                    push @verify_lines, "      $struct_param = $struct_field->{TYPE}_eInvalid;";
                    push @verify_lines, "    }";
                    push @verify_lines, "  }";
                    push @verify_lines, "}";

                    push @undo_lines, "if(nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @undo_lines, "  if(${param_field}) {"; #ioctl pointer check replaces _isnull check
                    push @undo_lines, "    if ($struct_param == $struct_field->{TYPE}_eInvalid) {";
                    push @undo_lines, "      $struct_param = 0 /* NULL */;";
                    push @undo_lines, "    }";
                    push @undo_lines, "  }";
                    push @undo_lines, "}";
                }
            }
        }

        # check for timebase or parser band as function param
        # this code only checks input params
        # output params post call are checked in bapi_common.pm
        if ($param->{ISREF})
        {
            if ($param->{BASETYPE} eq 'NEXUS_Timebase' || $param->{BASETYPE} eq 'NEXUS_ParserBand')
            {
                if ($param->{INPARAM}) 
                {
                    push @verify_lines, "if (nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @verify_lines, "  if (${param_field} != NULL) {"; # check pointer
                    push @verify_lines, "    if ((void *)*${param_field} == NULL) {"; # check pointed to value
                    push @verify_lines, "      *${param_field} = $param->{TYPE}_eInvalid;";
                    push @verify_lines, "    }";
                    push @verify_lines, "  }";
                    push @verify_lines, "}";

                    push @undo_lines, "if (nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @undo_lines, "  if (${param_field} != NULL) {"; # check pointer
                    push @undo_lines, "    if (*${param_field} == $param->{TYPE}_eInvalid) {"; # check pointed to value
                    push @undo_lines, "      *${param_field} = 0 /* NULL */;";
                    push @undo_lines, "    }";
                    push @undo_lines, "  }";
                    push @undo_lines, "}";
                }
            }
        }
        else # pass by value is input implicitly
        {
            if ($param->{TYPE} eq 'NEXUS_Timebase' || $param->{TYPE} eq 'NEXUS_ParserBand') {
                push @verify_lines, "if(nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                push @verify_lines, "  if((void *)${param_field} == NULL) {";
                push @verify_lines, "    ${param_field} = $param->{TYPE}_eInvalid;";
                push @verify_lines, "  }";
                push @verify_lines, "}";

                push @undo_lines, "if(nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                push @undo_lines, "  if(${param_field} == $param->{TYPE}_eInvalid) {";
                push @undo_lines, "    ${param_field} = 0 /* NULL */;";
                push @undo_lines, "  }";
                push @undo_lines, "}";
            }
        }

        # check for heap handle as struct member
        if ($param->{ISREF} && $param->{INPARAM} && exists $structs->{$param->{BASETYPE}}) {
            my $struct_field;
            for $struct_field (@{$structs->{$param->{BASETYPE}}}) {
                if ($struct_field->{TYPE} eq 'NEXUS_HeapHandle') {
                    my $struct_param = "$args.$param->{NAME}.$struct_field->{NAME}";
                    # verify all heap handles, even if NULL
                    push @verify_lines, "if(${param_field} ) {";
                    push @verify_lines, "  if (NEXUS_DRIVER_VERIFY_HEAP($struct_param)) {BERR_TRACE(-1); goto err_fault;}";
                    push @verify_lines, "}";
                }
            }
        }

        # check for heap handle as function param
        if ($param->{TYPE} eq 'NEXUS_HeapHandle') {
            # verify all heap handles, but honor null_allowed
            my $null_allowed = $param->{ATTR}->{'null_allowed'};
            if (!defined $null_allowed || $null_allowed eq "unprotected" || $null_allowed eq "trusted") {
                if ($null_allowed eq "unprotected" || $null_allowed eq "trusted") {
                    # allowed for unprotected code
                    push @verify_lines, "if (!${param_field} && CLIENT_ID->verify != b_objdb_verify_none) {";
                }
                else {
                    # never allowed
                    push @verify_lines, "if (!${param_field}) {";
                }
                push @verify_lines, "  BDBG_ERR((\"NULL not allowed for $param->{NAME} param in $func->{FUNCNAME}: %d\", __LINE__));";
                push @verify_lines, "  goto err_fault;";
                push @verify_lines, "}";
            }
            push @verify_lines, "if (NEXUS_DRIVER_VERIFY_HEAP(${param_field})) {BERR_TRACE(-1); goto err_fault;}";
        }
    }
    @post_verify_lines = (@undo_lines, @post_verify_lines);
    return \@verify_lines,\@post_verify_lines;
}

sub generate_union
{
    my ($file, $module, $funcs, $structs) = @_;
    my $func;
    my $module_lc = lc $module;


    print $file "/* union to store temporary copy of all in and out parameters */\n";
    print $file "union nexus_driver_module_args {\n";
    print $file "  PROXY_NEXUS_ModuleInit init;\n";
    for $func (@$funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my @struct;
        my $attr = bapi_common::process_function_attributes $func, $structs, $funcs;
        if (!$func->{NOSTRUCT}) {
            my $struct = bapi_common::ioctl_struct $module, $func;
            print $file "  struct {\n";
            print $file "     $struct ioctl;\n";
            for $param (@$params) {
                if ($param->{ISREF} && $param->{BASETYPE} ne 'void' && not (exists $attr->{'driver_overwrite'}->{$param->{NAME}}) ) {
                    print $file "     $param->{BASETYPE} $param->{NAME};\n";
                }
            }
            bapi_util::print_code $file, $attr->{'driver_ioctl_data'}, "     ";
            print $file "  } $func->{FUNCNAME};  \n";
        }
    }
    print $file "};\n";
}

sub case_ioctl
{
    my $module = shift;
    my $ioctl = shift;
    return <<ENDCASE;
 case NEXUS_IOCTL_NUM($ioctl):
ENDCASE
}

sub generate_ioctl
{
    my ($file, $module, $version, $structs, $classes, $funcs, $class_handles, $untrusted_api) = @_;
    my $func;
    my $ioctl = bapi_common::version_ioctl $module;
    my $name;

    print $file <<SWITCH;
    /* body of ioctl dispatch, convert IOCTL's back to number, that shall help compile to convert switch into the jump table */
    if ((NEXUS_IOCTL_TYPE(cmd)) != NEXUS_IOCTL_$module\_ID) { BERR_TRACE(-1); goto err_invalid_ioctl;}
    switch(NEXUS_IOCTL_NUM(cmd)) {
    default:
        if(arg==0) {
            BERR_TRACE(-1);
            goto err_fault; /* just prevents warning */
        }
       BERR_TRACE(-1);
       goto err_invalid_ioctl;
SWITCH


    print $file case_ioctl($module,$ioctl);
    print $file "   NEXUS_IOCTL_ENTER(VERSION);\n";
    print $file "   if (" . bapi_util::call('copy_from_user_small', "&module->args.init", '(void*)arg', "sizeof(module->args.init)") . "!=0) {BERR_TRACE(-1); goto err_fault;}\n";
    print $file "   if ( module->args.init.version !=" . (bapi_common::version_value $version) . " ) {BERR_TRACE(-1);goto err_version;}\n";
    print $file "   NEXUS_IOCTL_LEAVE(VERSION);\n";
    print $file "   break;\n\n";

    for $func (@$funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my $ioctl = bapi_common::ioctl_name $module, $func;
        my $args = "module->args.$func->{FUNCNAME}";
        my $retval = "$args.ioctl.__retval";
        my $attr = bapi_common::process_function_attributes $func, $structs, $funcs;
        my ($bypass, $class_code_pre) = generate_class_code_pre $classes, $func, $args;
        my $class_code_post = generate_class_code_post $classes, $func, $args;
        my ($class_verification,$post_class_verification) = generate_class_verification $classes, $structs, $func, $args, $class_handles, $untrusted_api;
        
        next if(exists $func->{ATTR}->{'local'});

        print $file case_ioctl($module,$ioctl);
        print $file "   NEXUS_IOCTL_ENTER($func->{FUNCNAME});\n";
        if (!$func->{NOSTRUCT}) {
            # copy IOCTL parameters
            print $file "   if (" . bapi_util::call('copy_from_user_small', "&$args.ioctl", '(void*)arg', "sizeof($args.ioctl)") . "!=0) {BERR_TRACE(-1);goto err_fault;}\n";
            
            bapi_util::print_code $file, $attr->{driver_pre_call}, "   ";
            
            # TODO: must do class_verification before driver_pre_call to avoid leaks if not verified.
            # solution is to push generate_class_verification logic into driver_pre_call.
            bapi_util::print_code $file, $class_verification, "   ";
        }
        bapi_util::print_code $file, $class_code_pre, "     ";
        if (!$bypass) {
            # make a function call
            
            if ($func->{RETTYPE} ne "void") {
                print $file "   $retval = \n"
            }
            
            # capture arguments
            my @args;
            for $param (@$params) {
                $name = $param->{NAME};
    
                if (exists $attr->{'driver_overwrite'}->{$name}) {
                    push @args, "\n      $args.$name";
                } elsif ($param->{ISREF} && $param->{BASETYPE} ne 'void') {
                    push @args, "\n     ($args.ioctl.$name ? &$args.$name : NULL )";
                } else {
                    push @args, "\n      $args.ioctl.$name";
                }
            }
            
            print $file "   " . bapi_util::call($func->{FUNCNAME}, @args) . ";\n\n";
        }

        my @post;

        if(defined $attr->{driver_post_success}) {
            @post = @{$attr->{driver_post_success}};
        }

        for $param (@$params) {
            $name = $param->{NAME};
            # copy OUT parameters
            next if (exists $attr->{'driver_overwrite'}->{$name});
            if ($param->{ISREF} && !$param->{INPARAM} && $param->{BASETYPE} ne 'void' ) {
                # outparams may be NULL
                push @post, "if ($args.ioctl.$name) {";
                push @post, "  if (" . bapi_util::call('copy_to_user_small', "$args.ioctl.$name", "&$args.$name", "sizeof($args.$name)") . "!=0) {BERR_TRACE(-1);goto err_fault;}";
                push @post, "}";
            }
        }
        if( (scalar @post) || (defined $attr->{driver_post_error}) || (defined $class_code_post) || defined $post_class_verification) {
            bapi_util::print_code $file, $post_class_verification, "     ";
            if ($func->{RETTYPE} eq "NEXUS_Error") {
                print $file "  if ($retval == NEXUS_SUCCESS) {\n";
            } elsif ($func->{RETTYPE} ne "void") {
                print $file "  if ((void*)$retval != NULL) {\n";
            } else {
                print $file "  {\n";
            }
            bapi_util::print_code $file, \@post, "     ";
            bapi_util::print_code $file, $class_code_post, "     ";
            if (($func->{RETTYPE} ne "void") && (defined $attr->{driver_post_error}) ) {
                print $file "  } else { \n";
                bapi_util::print_code $file, $attr->{driver_post_error}, "     ";
            }
            print $file "  }\n";
        }
        if ($func->{RETTYPE} ne "void") {
            # copy only retval other members are left intact
           print $file "  if (" . bapi_util::call('copy_to_user_small', ('&(( struct ' . ((bapi_common::ioctl_struct $module, $func) . '*)arg)->__retval')),   "&$retval", "sizeof($retval)") . "!=0) {BERR_TRACE(-1);goto err_fault;}\n";
        }
        bapi_util::print_code  $file, $attr->{driver_post_always}, "  ";
        print $file "  NEXUS_IOCTL_LEAVE($func->{FUNCNAME});\n";
        print $file "  break;\n\n";
    }
    print $file " }; /* end of switch */ \n\n\n\n";

}


sub generate
{
    my ($filename, $module, $version, $structs, $funcs, $class_handles, $untrusted_api) = @_;
    my $module_lc = lc $module;
    open FILE, ">$filename";
    my $file = \*FILE;

    my $destructors = bapi_classes::get_destructors $funcs;
    my $classes = bapi_classes::get_classes $funcs, $destructors;
    print $file bapi_util::header $module;
    print $file "#define NEXUS_DRIVER_MODULE_NAME \"$module\"\n";
    print $file "#include \"nexus_${module_lc}_module.h\"\n";
    print $file "BDBG_MODULE(nexus_${module_lc}_driver);\n";
    print $file "#include \"nexus_core_utils.h\"\n";
    print $file "#include \"nexus_class_verification.h\"\n";
    print $file "\n\n";
    print $file "/* defines to make all module symbols uniques */\n";
    print $file "#define nexus_driver_module_ioctl nexus_driver_${module_lc}_ioctl\n";
    print $file "#define nexus_driver_module_open nexus_driver_${module_lc}_open\n";
    print $file "#define nexus_driver_module_close nexus_driver_${module_lc}_close\n";
    print $file "#define nexus_driver_module_state nexus_driver_${module_lc}_state\n";
    print $file "#define nexus_driver_module_args nexus_driver_${module_lc}_args\n";
    print $file "\n\n";
    if(scalar @$classes) {
        print $file "#define NEXUS_DRIVER_MODULE_CLASS_TABLE    nexus_driver_$module\_class_table\n";
    }
    print $file "\n\n";

    print $file "#include \"driver/nexus_driver_prologue.h\"\n";

    print $file "#include \"" . (bapi_common::ioctl_header $module) . "\"\n";
    print $file "\n\n\n";
    generate_union $file, $module, $funcs, $structs;
    print $file "\n\n\n";
    bapi_classes::generate_class_table $file, $classes;
    print $file "\n\n\n";
    print $file "#include \"driver/nexus_driver_body.h\"\n";
    print $file "\n\n\n";
    generate_ioctl $file, $module, $version, $structs, $classes, $funcs, $class_handles, $untrusted_api;
    print $file "\n\n\n";
    print $file "#include \"driver/nexus_driver_epilogue.h\"\n";
    print $file "\n\n\n";
    bapi_classes::generate_class_verify_functions $file, $classes, $module;
    print $file "\n\n\n";
    close FILE;
}

1;

