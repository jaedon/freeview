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
# $brcm_Workfile: bapi_common.pm $
# $brcm_Revision: 49 $
# $brcm_Date: 7/16/12 1:01p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/bapi_common.pm $
# 
# 49   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 48   7/9/12 11:29a erickson
# SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
# 
# 47   3/21/12 2:32p erickson
# SW7340-296: remove unused secondary_destructor attribute
# 
# 46   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 45   11/29/11 11:31a erickson
# SW7420-2129: remove null_allowed=trusted attribute
# 
# 44   11/7/11 3:59p bandrews
# SW7231-446: merge to main
# 
# SW7231-446/2   11/4/11 9:44p bandrews
# SW7231-446: fix based on review
# 
# SW7231-446/1   11/3/11 9:27p bandrews
# SW7231-446: implemented
# 
# 43   9/13/11 1:23p erickson
# SW7420-1148: change client modes to unprotected/protected/untrusted
# 
# 42   7/29/11 12:15p erickson
# SW7420-1991: reorganize to allow ipcthunk/bapi_common.pm layout to be
#  closer
# 
# 41   7/28/11 3:28p erickson
# SW7425-1029: fix NEXUS_CallbackDesc proxy for functions with enums, not
#  handles
#
# 40   7/26/11 12:07p erickson
# SW7420-1992: callback database must take client id so that callbacks
#  can be disconnected when a client releases
# 
# 39   7/26/11 9:38a erickson
# SW7405-5438: fix warning
# 
# 38   7/25/11 2:46p erickson
# SW7420-1992: allow class with secondary_destructor and release, no
#  destructor
# 
# 37   7/21/11 7:06p jtna
# SW7405-5438: convert address to offset correctly in multi-element array
# 
# 36   7/12/11 1:26p erickson
# SW7420-1979: add support for new shutdown attribute
# 
# 35   5/13/11 12:17p erickson
# SW7420-1873: remove shutdown attribute
# 
# 34   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 33   4/14/11 12:01p erickson
# SW7420-1679: clean up NEXUS_Memory_Allocate using
#  attr{dealloc=NEXUS_Memory_Free}
#
# 32   3/17/11 11:59a erickson
# SW7346-101: fix NEXUS_StopCallbacks for secondary destructors
#  (specifically, NEXUS_Playpump_ClosePidChannel)
#
# 31   3/1/11 9:31a erickson
# SW7420-1123: add skip_thunk to centralize test for filenames that
#  should not be thunked
#
# 30   2/16/11 4:00p erickson
# SW7420-1507: fix callback registration in Open functions
#
# 29   2/16/11 1:03p erickson
# SW7420-1491: fix last check in. improve error checking.
#
# 28   2/15/11 2:16p erickson
# SW7420-1491: consolidate to nelem and nelem_out
#
# 27   2/3/11 6:37p vsilyaev
# SW7420-1441: Use local (non proxied) implementation for function that
#  flush CPU cache
#
# 26   1/28/11 9:04a erickson
# SW7420-1440: refactor secondary_destructor
#
# 25   1/27/11 3:05p erickson
# SW7208-152: fix and standardize get_destructor_hash
#
# 24   1/19/11 3:42p erickson
# SW7420-1123: refactor kernelproxy to correspond to usermode changes
#
# 23   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 22   10/14/10 12:34p erickson
# SW7420-1148: add secondary_destructor to support
#  NEXUS_Playpump_ClosePidChannel, verify function attr in one place,
#  support multiple param verifications per ioctl
#
# 21   10/12/10 4:29p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 20   6/10/10 4:56p vsilyaev
# SW7405-4011: Added StopCallbacks into the driver ioctl code
#
# 19   3/19/10 10:07a erickson
# SW7405-3832: rename variable to avoid conflict
#
# 18   8/4/08 4:50p vsilyaev
# PR 45397: Added support to handle address remapping in variable size
#  structure, this fixes dma example
#
# 17   6/10/08 3:29p vsilyaev
# PR 43184: Added support for Start/Stop callbacks in linux kernel/user
#  proxy mode
#
# 16   5/28/08 10:50a vsilyaev
# PR 42158: Added hot-fix for variable size complex function arguments
#
# 15   2/6/08 4:50p vsilyaev
# PR 38682: Fixed typo
#
# 14   2/6/08 4:37p vsilyaev
# PR 38682: Added attributes to set array size for 'void *' type
#
# 13   2/4/08 4:24p vsilyaev
# PR 38682: Added more tags for the linux kernel/user proxy mode
#
# 12   2/4/08 3:31p vsilyaev
# PR 38682: Added more cases in  support of variable size arrays
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
# 7   1/30/08 4:28p vsilyaev
# PR 38682: Improved handling of callbacks traversing user/kernel
#  boundary
#
# 6   1/30/08 11:49a vsilyaev
# PR 38682: Added routing of callbacks into the user space
#
# 5   1/29/08 5:55p vsilyaev
# PR 38682: Support of callbacks in the linux kernel/user proxy
#  environment
#
# 4   1/28/08 12:51p vsilyaev
# PR 38682: Fixed redundant mapping of addreses inside structures
#
# 3   1/28/08 12:33p vsilyaev
# PR 38682: Added support for attribute tags applied to structure
#  members.
#
# 2   1/25/08 2:35p vsilyaev
# PR 38682: Added handling of 'attr' to the function arguments
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/2   1/18/08 1:13p vsilyaev
# PR 38682: Updated nexus kernel environment
#
#############################################################################
use strict;
use bapi_util;

package bapi_common;

sub version_value
{
    my $version = shift;
    sprintf " 0x%08Xul ", ($version & 0xFFFFFFFF);
}

sub version_ioctl
{
    my $module = shift;
    return "IOCTL_${module}_NEXUS_INIT";
}

sub version_define
{
    my $module = shift;
    return "NEXUS_${module}_MODULE_VERSION";
}

sub ioctl_name
{
    my $module = shift;
    my $func = shift;

    return "IOCTL_${module}_$func->{FUNCNAME}";
}

sub ioctl_struct
{
    my $module = shift;
    my $func = shift;
    return "${module}_$func->{FUNCNAME}_data";
}

sub ioctl_header
{
    my $module = shift;
    my $module_lc = lc $module;

    return "nexus_${module_lc}_ioctl.h";
}

sub process_function_attributes {
    my $func = shift;
    my $structs = shift;
    my $funcs = shift;
    my $params = $func->{PARAMS};
    my @proxy_post_success;
    my @proxy_pre_call;
    my @driver_pre_call;
    my @driver_post_success;
    my @driver_post_error;
    my @driver_post_always;
    my @proxy_vars;
    my @driver_ioctl_reserved;
    my @driver_ioctl_data;
    my %driver_overwrite;
    my ($attr,$value);
    my $param;
    my $field;
    my $arg_no=0;
    my $driver_arg = "module->args.$func->{FUNCNAME}";

    if(exists $func->{ATTR}) {
        while (($attr, $value) = each %{$func->{ATTR}} ) {
            next if($attr eq 'destructor');
            next if($attr eq 'release');
            next if($attr eq 'local');
            next if($attr eq 'shutdown');
            print STDERR "ERROR: Unsupported attribute $attr = $value for $func->{FUNCNAME}\n";
        }
    }

    for $param (@$params) {
        my $name = $param->{NAME};
        my $name_copy = "$name\_copy";
        $arg_no ++;
        
        if (!exists $param->{ATTR}->{'nelem'} &&
            !exists $param->{ATTR}->{'fixed_length'})
        {
            # ordinary params
            if ($param->{ISREF} && $param->{BASETYPE} ne 'void') {
                if ($param->{INPARAM}) {
                    push @driver_pre_call, "if ($driver_arg.ioctl.$param->{NAME}) {";
                    push @driver_pre_call, "  if (copy_from_user_small(&$driver_arg.$name, $driver_arg.ioctl.$name, sizeof($driver_arg.$name))!=0) {BERR_TRACE(-1);goto err_fault;}";
                    push @driver_pre_call, "}";
                    my $null_allowed = $param->{ATTR}->{'null_allowed'};
                    if (!defined $null_allowed) {
                        push @driver_pre_call, "else {";
                        push @driver_pre_call, "  BDBG_ERR((\"NULL not allowed for $name param in $func->{FUNCNAME}: %d\", __LINE__));";
                        push @driver_pre_call, "  goto err_fault;";
                        push @driver_pre_call, "}";
                    }
                } 
                else {
                    push @driver_pre_call, "NEXUS_IOCTL_CLEAR($driver_arg.ioctl.$name);";
                }
            }
        }
        
        # must exchange eInvalid and NULL for protected clients for Timebase and ParserBand as direct params
        # this code only handles output param conversion post call
        # pre-call, post-verification NULL->eInvalid input conversion is done in bapi_driver_ioctl.pm
        if ($param->{ISREF})
        {
            if ($param->{BASETYPE} eq 'NEXUS_Timebase' || $param->{BASETYPE} eq 'NEXUS_ParserBand')
            {
                if (!$param->{INPARAM}) 
                {
                    push @driver_post_success, "if (nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @driver_post_success, "  if ($driver_arg.ioctl.$name != NULL) {"; # check pointer
                    push @driver_post_success, "    if (*$driver_arg.ioctl.$name == $param->{TYPE}_eInvalid) {"; # check pointed to value
                    push @driver_post_success, "      *$driver_arg.ioctl.$name = ($param->{TYPE})NULL;";
                    push @driver_post_success, "    }";
                    push @driver_post_success, "  }";
                    push @driver_post_success, "}";
                }
            }
        }
            
        if($param->{BASETYPE} eq 'NEXUS_CallbackDesc') {
            if ($param->{INPARAM}) {
                my $handle = "$driver_arg.ioctl.$$params[0]->{NAME}";
                my $id = sprintf("0x%05x", ((bapi_util::func_id $funcs, $func)*256 + $arg_no + 0x10000));
                my $notnull_test = "$driver_arg.ioctl.$name !=  NULL";
                push @driver_pre_call, "if($notnull_test) {";
                push @driver_pre_call, "   /* MAP callback from proxy space to the driver space */";
                push @driver_pre_call, "   NEXUS_DRIVER_CALLBACK_TO_DRIVER(&$driver_arg.$name, $handle, $id);";
                push @driver_pre_call, "}";
                push @driver_post_success, "if($notnull_test) {";
                push @driver_post_success, "    /* after function succeded, commit changes */";
                push @driver_post_success, "    NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(&$driver_arg.$name, $handle, $id);";
                push @driver_post_success, "}";
                push @driver_post_error, "if($notnull_test) {";
                push @driver_post_error, "    /* if error occured, then cancel changes in the callback mapping */";
                push @driver_post_error, "    NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(&$driver_arg.$name, $handle, $id);";
                push @driver_post_error, "}";
            } else {
                print STDERR "ERROR: pointer to NEXUS_CallbackDesc could be only input parameter\n";
            }
        }

        if (exists $structs->{$param->{BASETYPE}}) {
            my $field_no = 0;
            my @pre_update_proxy;
            my @pre_update_driver;
            my @post_update_proxy;
            my @post_update_driver;
            my @post_error_driver;

#            bapi_parse_c::print_struct $structs->{$param->{BASETYPE}}
            for $field (@{$structs->{$param->{BASETYPE}}}) {
                my $field_driver = "$driver_arg.$name.$field->{NAME}";

                $field_no ++;
                if($field->{TYPE} eq 'NEXUS_CallbackDesc') {
                    my $id = sprintf("0x%04x", ((bapi_util::struct_id $structs, $param->{BASETYPE})*256 + $field_no));
                    my $handle;
                    if ($func->{RETTYPE_ISHANDLE}) {
                        $handle = 'NULL'; # update after success
                    }
                    elsif ($$params[0]->{ISHANDLE}) {
                        $handle = "$driver_arg.ioctl.$$params[0]->{NAME}";
                    }
                    elsif (!$$params[0]->{ISREF} && $param != $$params[0]) {
                        # enum instead of handle.
                        # use enum type + enum value as psuedo handle
                        # hardcoded for a max of 128 enum values per type
                        # value<<2+1 to use non-4 byte aligned value to ensure no conflict with actual handle
                        $handle = "(($id<<9) + ($driver_arg.ioctl.$$params[0]->{NAME}<<2) + 1)";
                    }
                    if (defined $handle && $param->{ISREF}) {
                        if ($param->{INPARAM}) {
                            push @pre_update_driver, "/* MAP callback from proxy space to the driver space */";
                            push @pre_update_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER(&$field_driver, $handle, $id);";
                            if($handle eq 'NULL') {
                                push @post_update_driver, "/* since callback wasn't avaliable before calling function, use the return result to update callback */";
                                push @post_update_driver, "NEXUS_DRIVER_CALLBACK_UPDATE(&$field_driver, $handle, $id, $driver_arg.ioctl.__retval);";
                            } else {
                                push @post_update_driver, "/* after function succeded, commit changes */";
                                push @post_update_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(&$field_driver, $handle, $id);";
                            }
                            push @post_error_driver, "/* if error occured, then cancel changes in the callback mapping */";
                            push @post_error_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(&$field_driver, $handle, $id);";
                        } else {
                            push @post_update_driver, "/* MAP callback from the driver space to proxy space */";
                            push @post_update_driver, "NEXUS_DRIVER_CALLBACK_TO_USER(&$field_driver, $handle, $id);";
                        }
                    }
                }
                # must change eInvalid back to NULL for protected clients for Timebase and ParserBand as struct members
                # this code only handles output param conversion post call
                # pre-call, post-verification NULL->eInvalid input conversion is done in bapi_driver_ioctl.pm
                # TODO: is field as pointer supported?
                elsif ($field->{TYPE} eq 'NEXUS_Timebase' || $field->{TYPE} eq 'NEXUS_ParserBand')
                {
                    if (!$param->{INPARAM})
                    {
                        push @driver_post_success, "if (nexus_driver_client_id(context)->config.mode != NEXUS_ClientMode_eUnprotected) {";
                        push @driver_post_success, "  if ($field_driver == $field->{TYPE}_eInvalid) {";
                        push @driver_post_success, "    $field_driver = ($field->{TYPE})NULL;";
                        push @driver_post_success, "  }";
                        push @driver_post_success, "}";
                    }
                }

                if (exists $field->{ATTR}) {
#                   print " $func->{FUNCNAME} :";
#                   print "  field: $field->{TYPE} $field->{NAME} -> ";
#                   bapi_parse_c::print_attr $field->{ATTR};
                    while (($attr, $value) = each %{$field->{ATTR}} ) {
                        my $field_proxy = "$name->$field->{NAME}";
                        my $field_copy = "$name_copy.$field->{NAME}";
                        if($attr eq 'memory') {
                            if($value eq 'cached') {
                                if ($param->{ISREF}) {
                                    if ($param->{INPARAM}) {
                                        next if (exists $param->{ATTR}->{'nelem'});
                                        push @pre_update_proxy, "/* convert address to the device offset */";
                                        push @pre_update_proxy, "$field_copy = $field_copy ? (void *) NEXUS_AddrToOffset($field_copy) : $field_copy;";

                                        push @pre_update_driver, "/* convert offset (substituted by the proxy ) to the cached address */";
                                        push @pre_update_driver, "NEXUS_DRIVER_RECV_ADDR($field_driver,cached);";
                                        next;
                                    } else {
                                        push @post_update_proxy, "/* convert offset (substituted by the driver) to the cached address */";
                                        push @post_update_proxy, "$field_proxy = $field_proxy ? NEXUS_OffsetToCachedAddr((uint32_t)$field_proxy) : $field_proxy ;";

                                        push @post_update_driver, "/* convert offset (substituted by the proxy) to the cached address */";
                                        push @post_update_driver, "NEXUS_DRIVER_SEND_ADDR($field_driver,cached);";
                                        next;
                                    }
                                }
                            }
                        }
                        print STDERR "ERROR: Unsupported attribute $attr = $value for $field->{NAME} in $param->{BASETYPE}\n";
                    }
                }
            }
            if(scalar @pre_update_proxy) {
                push @proxy_vars, "$param->{BASETYPE} $name_copy; /* placeholder for modified parameters */ ";
                push @proxy_vars, "const $param->{BASETYPE} *$name\_old; /* copy of old pointer */ ";
                push @proxy_pre_call, "BSTD_UNUSED($name\_old);\n";
                push @proxy_pre_call, "if($name) {";
                push @proxy_pre_call, "     /* replace the original pointer with local copy */";
                push @proxy_pre_call, "     $name\_old=$name;$name\_copy=*$name;$name=&$name\_copy;";
                push @proxy_pre_call, "     /* update values ... */";
                bapi_util::append_code \@proxy_pre_call, \@pre_update_proxy, "     ";
                push @proxy_pre_call, "}";
            }
            bapi_util::append_if \@driver_pre_call, "$driver_arg.ioctl.$name", \@pre_update_driver;
            bapi_util::append_if \@driver_post_error, "$driver_arg.ioctl.$name", \@post_error_driver;
            bapi_util::append_if \@driver_post_success, "$driver_arg.ioctl.$name", \@post_update_driver;
            bapi_util::append_if \@proxy_post_success , "$name", \@post_update_proxy;
        }
        if(exists $param->{ATTR}) {
            while (($attr, $value) = each %{$param->{ATTR}} ) {
                if($attr eq 'nelem_convert' || $attr eq 'nelem_out') {
                    if( !exists $param->{ATTR}->{'nelem'}) {
                        print STDERR "ERROR: $attr requires nelem: $func->{FUNCNAME}\n";
                    }
                    next; # handled with nelem
                }
                next if($attr eq 'null_allowed'); # skip here

                if($attr eq 'reserved') {
                    if($param->{BASETYPE} ne 'void') {
                        push @driver_ioctl_reserved, "$param->{BASETYPE} $name\[$value];";
                    } else {
                        push @driver_ioctl_reserved, "uint8_t $name\[$value];";
                    }
                }
                elsif($attr eq 'fixed_length' && $param->{ISREF}) {
                    my $length = $value;
                    push @driver_ioctl_reserved, "uint8_t $name\[$value];";
                    push @driver_ioctl_data, "void * $name;";
                    push @driver_pre_call, "$driver_arg.$name=$driver_arg.ioctl.$name?$driver_arg.reserved.$name:NULL;";
                    if($param->{INPARAM}) {
                        push @driver_pre_call, "if (copy_from_user_small($driver_arg.$name, $driver_arg.ioctl.$name, $value) !=0) {NEXUS_IOCTL_FAULT($func->{FUNCNAME}, $name);goto err_fault;}";
                    }  else {
                        push @driver_post_success, "if ($driver_arg.ioctl.$name!=NULL && copy_to_user_small($driver_arg.ioctl.$name, $driver_arg.$name, $value) !=0) {NEXUS_IOCTL_FAULT($func->{FUNCNAME}, $name);goto err_fault;}";
                    }
                    $driver_overwrite{$name} = 1;
                }
                elsif($attr eq 'nelem' && $param->{ISREF}) {
                    my $length = "$driver_arg.ioctl.$value";
                    my $scale = "";
                    if( exists $param->{ATTR}->{'nelem_convert'}) {
                        $length =  $param->{ATTR}->{'nelem_convert'} . "($length)";
                    }
                    if($param->{BASETYPE} ne 'void') {
                        push @driver_ioctl_data, "$param->{BASETYPE} * $name;";
                        $scale = "sizeof(*$driver_arg.ioctl.$name) *";
                    } else {
                        push @driver_ioctl_data, "void * $name;";
                    }
                    $length = "($scale $length)";
                    push @driver_pre_call, "if($driver_arg.ioctl.$name!=NULL) {";
                    push @driver_pre_call, "    /* allocate space for the $name */";
                    push @driver_post_always, "/* free space allocated for $name */";
                    if( exists $param->{ATTR}->{'reserved'}) {
                        push @driver_pre_call, "    if( $length <= $scale $param->{ATTR}->{'reserved'} ) {";
                        push @driver_pre_call, "        $driver_arg.$name = $driver_arg.reserved.$name;";
                        push @driver_pre_call, "    } else {";
                        push @driver_pre_call, "        $driver_arg.$name = NEXUS_DRIVER_ALLOC($length);";
                        push @driver_pre_call, "        if($driver_arg.$name == NULL) {goto err_alloc;}";
                        push @driver_pre_call, "    }";

                        push @driver_post_always, "if($driver_arg.$name != $driver_arg.reserved.$name) {";
                        push @driver_post_always, "    NEXUS_DRIVER_FREE($driver_arg.$name);";
                        push @driver_post_always, "}";

                    } else {
                        push @driver_pre_call, "    $driver_arg.$name = NEXUS_DRIVER_ALLOC($length);";
                        push @driver_post_always, "NEXUS_DRIVER_FREE($driver_arg.$name);";
                    }
                    if($param->{INPARAM}) {
                        push @driver_pre_call, "    if (copy_from_user_small($driver_arg.$name, $driver_arg.ioctl.$name, $length) !=0) {NEXUS_IOCTL_FAULT($func->{FUNCNAME}, $name);goto err_fault;}";
                        # Handle address translation
                        my @driver_update_structure;
                        my @proxy_update_structure;
                        if (exists $structs->{$param->{BASETYPE}}) {
                            my $field;
                            for $field (@{$structs->{$param->{BASETYPE}}}) {
                                if (exists $field->{ATTR}) {
                                    my ($attr,$value);
                                    while (($attr, $value) = each %{$field->{ATTR}} ) {
                                        if($attr eq 'memory' && $value eq 'cached' && $param->{ISREF} && $param->{INPARAM}) {
                                            my $field_copy = "$name_copy->$field->{NAME}";

                                            push @proxy_update_structure, "/* convert address to the device offset */";
                                            push @proxy_update_structure, "$field_copy = $field_copy ? (void *) NEXUS_AddrToOffset($field_copy) : $field_copy;";

                                            push @driver_update_structure, "/* convert offset (substituted by the proxy) to the cached address */";
                                            push @driver_update_structure, "NEXUS_DRIVER_RECV_ADDR($driver_arg.$name [i].$field->{NAME},cached);";
                                            next;
                                        }
                                        print STDERR "ERROR: Unsupported attribute $attr = $value for $field->{NAME} in $param->{BASETYPE}\n";
                                    }
                                }
                            }
                        }
                        if (scalar @driver_update_structure) {
                            push @driver_pre_call, "    {unsigned i;for(i=0;i<$driver_arg.ioctl.$value;i++) {";
                            bapi_util::append_code \@driver_pre_call, \@driver_update_structure, "        ";
                            push @driver_pre_call, "    }}";
                        }
                        if (scalar @proxy_update_structure) {
                            # Approximate handling
                            push @proxy_pre_call, "    unsigned i;";
                            push @proxy_pre_call, "    $param->{BASETYPE} * $name_copy = __builtin_alloca(sizeof($param->{BASETYPE}) * $value);";
                            push @proxy_pre_call, "    if($name!=NULL) {";
                            push @proxy_pre_call, "        void *_save_data = $name_copy;";
                            push @proxy_pre_call, "        BKNI_Memcpy($name_copy, $name, sizeof($param->{BASETYPE}) * $value);";
                            push @proxy_pre_call, "        for(i=0;i<$value;i++) {";
                            bapi_util::append_code \@proxy_pre_call, \@proxy_update_structure, "            ";
                            push @proxy_pre_call, "            $name_copy++;";
                            push @proxy_pre_call, "        }";
                            push @proxy_pre_call, "        $name = _save_data;";
                            push @proxy_pre_call, "    }";
                        }
                    }
                    else {
                        my $length_out = $length;

                        # nelem_out is an optimization for out params. only copy the data actually populated.
                        if( exists $param->{ATTR}->{'nelem_out'}) {
                            $length_out = "$driver_arg.$param->{ATTR}->{'nelem_out'}";
                            if( exists $param->{ATTR}->{'nelem_convert'}) {
                                $length_out =  $param->{ATTR}->{'nelem_convert'} . "($length_out)";
                            }
                            $length_out = "($length_out * sizeof(*$driver_arg.ioctl.$name))";
                        }
                        push @driver_post_success, "if ($driver_arg.ioctl.$name!=NULL && copy_to_user_small($driver_arg.ioctl.$name, $driver_arg.$name, $length_out ) !=0) {NEXUS_IOCTL_FAULT($func->{FUNCNAME}, $name);goto err_fault;}";
                    }
                    push @driver_pre_call, "} else {";
                    push @driver_pre_call, "    $driver_arg.$name = NULL;";
                    push @driver_pre_call, "}";
                    $driver_overwrite{$name} = 1;
                }
                elsif($attr eq 'memory' && $value eq 'cached') {
                    if ($param->{INPARAM}) {
                        push @proxy_pre_call, "/* convert offset (substituted by the driver) to the cached address */";
                        push @proxy_pre_call, "$name = $name ? (void *)NEXUS_AddrToOffset($name) : $name;";
                        push @driver_pre_call, "/* convert address to the device offset */";
                        push @driver_pre_call, "NEXUS_DRIVER_RECV_ADDR($driver_arg.ioctl.$name, cached);";
                    }
                    else {
                        # handle return value. if client passes NULL outparam, only the client crashes
                        push @proxy_post_success, "/* convert offset (substituted by the driver) to the cached address */";
                        push @proxy_post_success, "*$name = *$name ? NEXUS_OffsetToCachedAddr((uint32_t)*$name) : *$name;";
                        push @driver_post_success, "/* convert address to the device offset */";
                        push @driver_post_success, "NEXUS_DRIVER_SEND_ADDR($driver_arg.$name, cached);";
                    }
                }
                else {
                    print STDERR "ERROR: Unsupported attribute $attr = $value for $param->{NAME} in $func->{FUNCNAME}\n";
                }
            }
        }
    }
    
    my %result;
    if (scalar @proxy_post_success) {
        $result{'proxy_post_success'} = \@proxy_post_success;
    }
    if (scalar @driver_post_success) {
        $result{'driver_post_success'} = \@driver_post_success;
    }
    if (scalar @proxy_pre_call) {
        $result{'proxy_pre_call'} = \@proxy_pre_call;
    }
    if (scalar @driver_pre_call) {
        $result{'driver_pre_call'} = \@driver_pre_call;
    }
    if (scalar @proxy_vars) {
        $result{'proxy_vars'} = \@proxy_vars;
    }
    if (scalar @driver_post_error) {
        $result{'driver_post_error'} = \@driver_post_error;
    }
    if (scalar @driver_post_always) {
        $result{'driver_post_always'} = \@driver_post_always;
    }

    if(scalar @driver_ioctl_reserved) {
        push @driver_ioctl_data, "struct {";
        bapi_util::append_code \@driver_ioctl_data, \@driver_ioctl_reserved, "    ";
        push @driver_ioctl_data, "} reserved;";
    }
    if(scalar @driver_ioctl_data) {
        $result{'driver_ioctl_data'} = \@driver_ioctl_data;
    }
    $result{'driver_overwrite'} = \%driver_overwrite;
    return \%result;
}


1;

