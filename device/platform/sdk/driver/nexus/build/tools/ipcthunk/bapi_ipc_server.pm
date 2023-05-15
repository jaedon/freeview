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
# $brcm_Workfile: bapi_ipc_server.pm $
# $brcm_Revision: 36 $
# $brcm_Date: 10/11/12 12:15p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/ipcthunk/bapi_ipc_server.pm $
# 
# 36   10/11/12 12:15p vsilyaev
# SW7420-2324: Don't verify twice and use faster path for local objects
# 
# 35   10/5/12 4:58p bandrews
# SW7425-4040: do not convert return value between eInvalid and NULL
# 
# 34   9/19/12 11:40a erickson
# SW7420-2324: add atomic verify and acquire for handle params
# 
# 33   8/22/12 1:23p erickson
# SW7231-958: remove function restriction
# 
# 32   7/18/12 5:31p vsilyaev
# SW7420-2324: Properly handle case for timebase and parser bands
# 
# 31   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 30   6/19/12 12:41p vsilyaev
# SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
# 
# 29   3/21/12 2:32p erickson
# SW7340-296: remove unused secondary_destructor attribute
# 
# 28   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 27   11/7/11 3:59p bandrews
# SW7231-446: merge to main
# 
# SW7231-446/3   11/4/11 10:15p bandrews
# SW7231-446: compile error
# 
# SW7231-446/2   11/4/11 9:44p bandrews
# SW7231-446: fix based on review
# 
# SW7231-446/1   11/3/11 7:55p bandrews
# SW7231-446: implemented
# 
# 26   10/31/11 6:13p bandrews
# SW7231-391: casts to support parser band and timebase as enums
# 
# SW7420-2078/3   10/18/11 7:00p bandrews
# SW7231-391: remove global_handle attribute, add comment about cross-
#  module handle verification
# 
# SW7420-2078/2   10/11/11 8:21p bandrews
# SW7231-391: added casts to support parser band and timebase dual
#  variants
# 
# SW7420-2078/1   9/30/11 8:35p bandrews
# SW7420-2078: added global_handle attribute
# 
# 24   9/29/11 1:57p erickson
# SW7420-1148: nexus_unregister cannot call NEXUS_ASSERT_MODULE
# 
# 23   9/20/11 11:02a erickson
# SW7420-1148: check attr{null_allowed} for NEXUS_HeapHandle function
#  param
# 
# 22   9/13/11 1:23p erickson
# SW7420-1148: change client modes to unprotected/protected/untrusted
# 
# 21   9/2/11 12:39p erickson
# SW7420-1148: refactor class_list.pl to generate per-module "get current
#  client" functions
# 
# 20   9/2/11 10:03a erickson
# SW7420-1148: allow nexus_register/unregister to objdb from nexus
#  module. enables more complex resource management.
# 
# 19   8/31/11 12:18p erickson
# SW7231-357: don't generate server-side code for attr{local=true}
#  functions
# 
# 18   8/23/11 4:13p erickson
# SW7420-1992: rename to surface_compositor
# 
# 17   8/22/11 1:01p erickson
# SW7420-1689: NEXUS_HeapHandle is validated, even though it is not a
#  class
# 
# 16   8/11/11 11:29a erickson
# SW7420-1689: clarify comment
# 
# 15   8/8/11 5:10p erickson
# SW7420-1689: add untrusted client api enforcement
#
# 14   8/3/11 9:57a erickson
# SW7420-1992: add per-display framebuffer heap to
#  NEXUS_SurfaceCmpSettings
#
# 13   7/29/11 12:10p erickson
# SW7420-1991: fix nelem,nelem_out attribute support, reorganize to be
#  closer to kernelproxy/bapi_common.pm layout
#
# 12   7/25/11 2:46p erickson
# SW7420-1992: allow class with secondary_destructor and release, no
#  destructor
#
# 11   5/17/11 12:25p erickson
# SW7420-1679: NEXUS_Memory_Free must check pointer with objdb before
#  freeing
#
# 10   5/13/11 12:18p erickson
# SW7420-1873: remove shutdown attribute
#
# 9   4/18/11 1:48p erickson
# SW7420-1123: use void* for intermediate device offset to avoid strict
#  aliasing warnings
#
# 8   4/14/11 11:59a erickson
# SW7420-1679: clean up NEXUS_Memory_Allocate using
#  attr{dealloc=NEXUS_Memory_Free}
#
# 7   4/1/11 9:59a erickson
# SW7420-1671: refactor object database for multiprocess stress
#
# 6   2/16/11 2:17p erickson
# SW7420-1123: implement nelem/nelem_out for IPC thunk
#
# 5   2/15/11 2:28p erickson
# SW7420-1123: refactor IPC thunk to separate in/out params
#
# 4   1/28/11 9:04a erickson
# SW7420-1440: refactor secondary_destructor
#
# 3   1/21/11 4:03p erickson
# SW7420-1123: verify NEXUS_HeapHandle as function param
#
# 2   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 1   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
#
#############################################################################
use strict;

package bapi_ipc_server;

sub generate_class_code_post
{
    my ($classes, $func, $in_args, $out_args) = @_;
    CLASS: for (@$classes) {
        my $class = $_;
        my $id=$class->{CLASS_NAME};
        for (@{$class->{CONSTRUCTORS}}) {
            if($_ == $func) {
                if($class->{DESTRUCTOR_TYPE} eq 'destructor') {
                    return ["NEXUS_DRIVER_CREATE_OBJECT($id, (void *)$out_args.__retval);"];
                }
                last CLASS;
            }
        }
        if($class->{ACQUIRE} == $func) {
            my $param = @{$func->{PARAMS}}[0];
            return ["NEXUS_DRIVER_ACQUIRE_OBJECT($id, (void *)$out_args.__retval);"];
        }
    }
    return undef;
}

sub generate_class_code_pre
{
    my ($classes, $func, $in_args, $out_args) = @_;
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
                my $handle = "$in_args.$$params[0]->{NAME}";
                $bypass = 1;
                $shutdown = "nexus_driver_shutdown_close_$classtype($handle);";

            }
            my $param =  @{$func->{PARAMS}}[0];
            return ($bypass,["NEXUS_DRIVER_DESTROY_OBJECT($id, (void *)$in_args.$param->{NAME});",$shutdown]);
        }
        if($class->{RELEASE} == $func) {
            my $param =  @{$func->{PARAMS}}[0];
            return ($bypass,["NEXUS_DRIVER_RELEASE_OBJECT($id, (void *)$in_args.$param->{NAME});"]);
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
        push @verify_lines, "if ((CLIENT_ID)->limited_api) {BERR_TRACE(-1); goto err_fault;}";
    }

    #print "generate_class_verification: $func->{FUNCNAME}\n";
    for (@{$func->{PARAMS}}) {
        my $param = $_;
        my $param_field = "$args.$param->{NAME}";

        # check each class that was defined in this module
        #print "  param $param->{NAME} $param->{TYPE}\n";
        for (@$classes) {
            my $class = $_;
            my $handletype = $class->{CLASS_TYPE};

            # check if param is class handle
            if ($param->{TYPE} eq $handletype) {
                my $class_name = $class->{CLASS_NAME};
                my $typecast = ($class_name eq $handletype) ? "(void *)" : "";
                push @verify_lines, "if (NEXUS_OBJECT_VERIFY($class_name,$typecast ${param_field})) {BERR_TRACE(-1); goto err_fault;}";
            }
        }

        # generate cross-module verification
        foreach (@$class_handles) {
            my $handletype = $_;
            #print "generate_class_verification $handletype\n";
            # first, check if param is a struct which has a class handle field
            if ($param->{ISREF} && $param->{INPARAM} && exists $structs->{$param->{BASETYPE}}) {
                my $struct_field;
                for $struct_field (@{$structs->{$param->{BASETYPE}}}) {
                    if ($struct_field->{TYPE} eq $handletype) {
                        my $class_name = bapi_classes::get_class_name($handletype);
                        my $struct_param = "${param_field}.$struct_field->{NAME}";
                        my $typecast = ($class_name eq $handletype) ? "(void *)" : "";
                        my $local_class = 0;
                        for (@$classes) {
                            if($handletype eq $_->{CLASS_TYPE}) {
                                $local_class = 1; last;
                            }
                        }

                        # verify non-NULL params
                        push @verify_lines, "if(!${param_field}_isnull && $struct_param) {";
                        if($local_class) {
                            push @verify_lines, "  if (NEXUS_OBJECT_VERIFY($class_name,$typecast $struct_param)) {BERR_TRACE(-1); goto err_fault;}";
                        } else {
                            push @verify_lines, "  if (NEXUS_OBJECT_VERIFY_AND_ACQUIRE($class_name, $typecast $struct_param)) {BERR_TRACE(-1); goto err_fault;}";
                        }
                        push @verify_lines, "}";
                        if(!$local_class) {
                            push @post_verify_lines, "if(!${param_field}_isnull && $struct_param) {";
                            push @post_verify_lines, "  NEXUS_OBJECT_RELEASE_AFTER_VERIFY($class_name, $typecast $struct_param);";
                            push @post_verify_lines, "}";
                        }
                    }
                }
            }
            # second, check if param is a class handle field
            elsif (!$param->{ISREF} && $param->{INPARAM}) {
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
                    my $struct_param = "${param_field}.$struct_field->{NAME}";
                    push @verify_lines, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @verify_lines, "  if (!${param_field}_isnull) {"; #_isnull check replaces ioctl pointer check 
                    push @verify_lines, "    if ((void *)$struct_param == NULL) {";
                    push @verify_lines, "      $struct_param = $struct_field->{TYPE}_eInvalid;";
                    push @verify_lines, "    }";
                    push @verify_lines, "  }";
                    push @verify_lines, "}";

                    push @undo_lines, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @undo_lines, "  if (!${param_field}_isnull) {"; #_isnull check replaces ioctl pointer check 
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
                push @verify_lines, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                push @verify_lines, "  if ((void *)${param_field} == NULL) {";
                push @verify_lines, "    ${param_field} = $param->{TYPE}_eInvalid;";
                push @verify_lines, "  }";
                push @verify_lines, "}";

                push @undo_lines, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                push @undo_lines, "  if (${param_field} == $param->{TYPE}_eInvalid) {";
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
                    my $struct_param = "${param_field}.$struct_field->{NAME}";
                    # verify all heap handles, even if NULL
                    push @verify_lines, "if(!${param_field}_isnull) {";
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
                    push @verify_lines, "if (!${param_field} && (CLIENT_ID)->verify != b_objdb_verify_none) {";
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

sub generate_ipc
{
    my ($file, $module, $version, $structs, $classes, $funcs, $class_handles, $untrusted_api) = @_;
    my $func;
    my $ipc = bapi_common::version_ipc $module;
    my $name;

    for $func (@$funcs) {
        my $params = $func->{PARAMS};
        my $param;
        my $ipc = bapi_common::ipc_name $module, $func;
        my $in_args = "in_data->data.$func->{FUNCNAME}.in";
        my $out_args = "out_data->data.$func->{FUNCNAME}.out";
        my $retval = "$out_args.__retval";
        my $attr = bapi_common::process_function_attributes $func, $structs, $funcs;
        my ($bypass,$class_code_pre) = generate_class_code_pre $classes, $func, $in_args, $out_args;
        my $class_code_post = generate_class_code_post $classes, $func, $in_args, $out_args;
        my ($class_verification,$post_class_verification) = generate_class_verification $classes, $structs, $func, $in_args, $class_handles, $untrusted_api;
        
        next if(exists $func->{ATTR}->{'local'});

        print $file "case $ipc:\n";
        print $file "   NEXUS_IPC_ENTER($func->{FUNCNAME});\n";

        # verify in size for fixed params. auto-gen code also verifies each variable param.
        print $file "   if (in_data_size < sizeof(in_data->header)+sizeof($in_args)) {\n";
        print $file "     BDBG_ERR((\"short in data %d < %d+%d: %d\", in_data_size, sizeof(in_data->header),sizeof($in_args), __LINE__));\n";
        print $file "     goto err_fault;\n";
        print $file "   }\n";
        # verify out size for fixed params. auto-gen code also verifies each variable param.
        print $file "   *p_out_data_size = sizeof(out_data->header)+sizeof($out_args);\n";
        print $file "   if (out_mem_size < *p_out_data_size) {\n";
        print $file "     BDBG_ERR((\"short out size %d < %d+%d: %d\", out_mem_size, sizeof(out_data->header), *p_out_data_size, __LINE__));\n";
        print $file "     goto err_fault;\n";
        print $file "   }\n";

        # must do class_verification before driver_pre_call to avoid leaks if not verified
        bapi_util::print_code $file, $class_verification, "   ";
        bapi_util::print_code $file, $attr->{driver_pre_call}, "   ";
        bapi_util::print_code $file, $class_code_pre, "   ";


        if (!$bypass) {
            # make a function call
    
            if ($func->{RETTYPE} ne "void") {
                print $file "   $retval = \n"
            }
    
    
            my @args;
            for $param (@$params) {
                my $info = bapi_common::process_function_param $func, $param;
                push @args, "\n        " . $info->{'driver_arg'};
            }
    
            print $file "   " . bapi_util::call($func->{FUNCNAME} , @args) . ";\n\n";
        }

        if (defined $attr->{driver_post_success} || defined $attr->{driver_post_error} || defined $class_code_post || defined $post_class_verification) {
            bapi_util::print_code $file, $post_class_verification, "     ";
            if ($func->{RETTYPE} eq "NEXUS_Error") {
                print $file "  if ($retval == NEXUS_SUCCESS) {\n";
            } elsif ($func->{RETTYPE} ne "void") {
                print $file "  if ((void *)$retval != NULL) {\n";
            } else {
                print $file "  {\n";
            }
            bapi_util::print_code $file, $attr->{driver_post_success}, "     ";
            bapi_util::print_code $file, $class_code_post, "     ";
            if (($func->{RETTYPE} ne "void") && (defined $attr->{driver_post_error}) ) {
                print $file "  } else { \n";
                bapi_util::print_code $file, $attr->{driver_post_error}, "     ";
            }
            print $file "  }\n";
        }
        bapi_util::print_code  $file, $attr->{driver_post_always}, "  ";
        print $file "  NEXUS_IPC_LEAVE($func->{FUNCNAME});\n";
        print $file "  break;\n\n";
    }

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
    print $file "#define NEXUS_SERVER_MODULE_NAME \"$module\"\n";
    print $file "#include \"nexus_${module_lc}_module.h\"\n";
    print $file "BDBG_MODULE(nexus_${module_lc}_ipc_server);\n";
    print $file "#include \"nexus_core_utils.h\"\n";
    print $file "#include \"nexus_class_verification.h\"\n";
    print $file "\n\n";
    print $file "/* defines to make all module symbols uniques */\n";
    print $file "#define nexus_server_module_open nexus_server_${module_lc}_open\n";
    print $file "#define nexus_server_module_close nexus_server_${module_lc}_close\n";
    print $file "#define nexus_server_process nexus_server_${module_lc}_process\n";
    print $file "#define nexus_server_args " . (bapi_common::ipc_block $module) ."\n";
    print $file "#define nexus_server_data_size nexus_server_${module_lc}_data_size\n";
    print $file "\n\n";
    if(scalar @$classes) {
        print $file "#define NEXUS_DRIVER_MODULE_CLASS_TABLE    nexus_server_$module\_class_table\n";
    }
    print $file "\n\n";

    print $file "#include \"server/nexus_server_prologue.h\"\n";

    print $file "#include \"" . (bapi_common::ipc_header $module) . "\"\n";
    print $file "#define NEXUS_IPC_HEADER_VERSION   " . (bapi_common::version_define $module) . "\n";
    print $file "\n\n\n";
    bapi_classes::generate_class_table $file, $classes;
    print $file "\n\n\n";
    print $file "#include \"server/nexus_server_body.h\"\n";
    print $file "\n\n\n";
    generate_ipc $file, $module, $version, $structs, $classes, $funcs, $class_handles, $untrusted_api;
    print $file "\n\n\n";
    print $file "#include \"server/nexus_server_epilogue.h\"\n";
    print $file "\n\n\n";
    bapi_classes::generate_class_verify_functions $file, $classes, $module;
    print $file "\n\n\n";
    close FILE;
}

1;

