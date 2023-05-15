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
# $brcm_Revision: 28 $
# $brcm_Date: 7/16/12 1:01p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/ipcthunk/bapi_common.pm $
# 
# 28   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 27   7/9/12 11:30a erickson
# SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
# 
# 26   6/11/12 10:37a erickson
# SW7425-3174: fix variable length out params
# 
# 25   3/21/12 2:32p erickson
# SW7340-296: remove unused secondary_destructor attribute
# 
# 24   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 23   11/29/11 11:31a erickson
# SW7420-2129: remove null_allowed=trusted attribute
# 
# 22   11/7/11 3:59p bandrews
# SW7231-446: merge to main
# 
# SW7231-446/2   11/4/11 9:44p bandrews
# SW7231-446: fix based on review
# 
# SW7231-446/1   11/3/11 7:55p bandrews
# SW7231-446: implemented
# 
# 21   9/13/11 1:23p erickson
# SW7420-1148: change client modes to unprotected/protected/untrusted
# 
# 20   8/30/11 9:10a erickson
# SW7420-1991: fix conversion of input params
# 
# 19   7/29/11 12:11p erickson
# SW7420-1991: fix nelem,nelem_out attribute support, reorganize to be
#  closer to kernelproxy/bapi_common.pm layout
# 
# 19   7/29/11 12:10p erickson
# SW7420-1991: fix nelem,nelem_out attribute support, reorganize to be
#  closer to kernelproxy/bapi_common.pm layout
# 
# 18   7/28/11 3:28p erickson
# SW7425-1029: fix NEXUS_CallbackDesc proxy for functions with enums, not
#  handles
# 
# 17   7/26/11 12:07p erickson
# SW7420-1992: callback database must take client id so that callbacks
#  can be disconnected when a client releases
# 
# 15   7/12/11 1:26p erickson
# SW7420-1979: add support for new shutdown attribute
# 
# 14   5/13/11 12:18p erickson
# SW7420-1873: remove shutdown attribute
# 
# 13   4/18/11 1:48p erickson
# SW7420-1123: use void* for intermediate device offset to avoid strict
#  aliasing warnings
#
# 12   4/14/11 11:59a erickson
# SW7420-1679: clean up NEXUS_Memory_Allocate using
#  attr{dealloc=NEXUS_Memory_Free}
#
# 11   3/17/11 11:59a erickson
# SW7346-101: fix NEXUS_StopCallbacks for secondary destructors
#  (specifically, NEXUS_Playpump_ClosePidChannel)
#
# 10   3/1/11 9:31a erickson
# SW7420-1123: add skip_thunk to centralize test for filenames that
#  should not be thunked
#
# 9   2/28/11 3:27p erickson
# SW7420-1123: add nelem_convert attribute support
#
# 8   2/16/11 4:00p erickson
# SW7420-1507: fix callback registration in Open functions
#
# 7   2/16/11 2:17p erickson
# SW7420-1123: implement nelem/nelem_out for IPC thunk
#
# 6   2/15/11 2:28p erickson
# SW7420-1123: refactor IPC thunk to separate in/out params
#
# 5   2/9/11 11:43a erickson
# SW7420-1441: support attr{local=true}
#
# 4   1/28/11 9:04a erickson
# SW7420-1440: refactor secondary_destructor
#
# 3   1/27/11 3:05p erickson
# SW7208-152: fix and standardize get_destructor_hash
#
# 2   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 1   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
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

sub version_ipc
{
    my $module = shift;
    return "B_IPC_${module}_NEXUS_INIT";
}

sub version_define
{
    my $module = shift;
    return "NEXUS_${module}_MODULE_VERSION";
}

sub ipc_name
{
    my $module = uc shift;
    my $func = shift;

    return "B_IPC_${module}_$func->{FUNCNAME}";
}

sub ipc_struct
{
    my $module = uc shift;
    my $func = shift;
    return "B_IPC_${module}_$func->{FUNCNAME}_data";
}

sub ipc_block
{
    my $module = uc shift;
    return "B_IPC_${module}_data";
}

sub ipc_header
{
    my $module = shift;
    my $module_lc = lc $module;

    return "nexus_${module_lc}_ipc_api.h";
}

sub process_function_param {
    my $func = shift;
    my $param = shift;
    my @ipc_inparams;
    my @ipc_outparams;
    my $driver_arg;
    my $completed=0;
    my $null_allowed=0;
    my $name = $param->{NAME};
    my $driver_in_arg = "in_data->data.$func->{FUNCNAME}.in";
    my $driver_out_arg = "out_data->data.$func->{FUNCNAME}.out";

    if (exists $param->{ATTR}) {
        my $attr;
        my $value;
        while (($attr, $value) = each %{$param->{ATTR}} ) {
            next if($attr eq 'reserved');
            next if($attr eq 'nelem_out');
            next if($attr eq 'nelem_convert');
            if(($attr eq 'nelem' || $attr eq 'fixed_length')) {
                # unix domain sockets can pass 4K atomically. to make the implementation much simpler, the thunk
                # requires that all params (including nelem and nelem_out params) to fit within one packet.
                # anything greater will cause the call to fail. # the app can then be rewritten to request smaller chunks.
                if ($param->{INPARAM}) {
                    push @ipc_inparams, "unsigned ${name}_variable_offset;";
                    $driver_arg = "(void *)&(&$driver_in_arg.variable_params)[$driver_in_arg.${name}_variable_offset]";
                }
                else {
                    push @ipc_outparams, "unsigned ${name}_variable_offset;";
                    $driver_arg = "(void *)&(&$driver_out_arg.variable_params)[$driver_out_arg.${name}_variable_offset]";
                }
                $completed = 1;
            }
            elsif($attr eq 'memory' && $value eq 'cached' ) {
                if ($param->{INPARAM}) {
                    push @ipc_inparams, "NEXUS_Ipc_DeviceAddress $param->{NAME};";
                    $driver_arg = "NEXUS_OffsetToCachedAddr((uint32_t)$driver_in_arg.$name)";
                }
                else {
                    push @ipc_outparams, "NEXUS_Ipc_DeviceAddress $param->{NAME};";
                    $driver_arg = "($param->{TYPE})&$driver_out_arg.$name";
                }
                $completed = 1;
            }
            elsif($attr eq 'null_allowed') {
                $null_allowed = $value;
            }
            else {
                print STDERR "ERROR: Unsupported attribute $attr = $value for $param->{NAME} in $func->{FUNCNAME}\n";
            }
        }
    }

    if($completed) {
    }
    elsif ($param->{ISREF} && $param->{BASETYPE} ne 'void') {
        if ($param->{INPARAM}) {
            push @ipc_inparams, "$param->{BASETYPE} $name;";
            $driver_arg = "($driver_in_arg.$name\_isnull ? NULL : &$driver_in_arg.$name )";
            push @ipc_inparams, "bool $name\_isnull;";
        } else {
            push @ipc_outparams, "$param->{BASETYPE} $name;";
            $driver_arg = "&$driver_out_arg.$name";
        }
    }
    else {
        # must be inparam
        push @ipc_inparams, "$param->{TYPE} $param->{NAME};";
        $driver_arg = "$driver_in_arg.$name";
    }
    my %result;
    $result{'ipc_inparams'} = \@ipc_inparams;
    $result{'ipc_outparams'} = \@ipc_outparams;
    $result{'driver_arg'} = $driver_arg;
    return \%result;
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
    my %driver_overwrite;
    my ($attr,$value);
    my $param;
    my $field;
    my $arg_no=0;
    my $driver_in_arg = "in_data->data.$func->{FUNCNAME}.in";
    my $driver_out_arg = "out_data->data.$func->{FUNCNAME}.out";

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
        $arg_no ++;

        if (!exists $param->{ATTR}->{'nelem'} &&
            !exists $param->{ATTR}->{'memory'} &&
            !exists $param->{ATTR}->{'fixed_length'})
        {
            # ordinary params
            if ($param->{ISREF} && $param->{BASETYPE} ne 'void') {
                if ($param->{INPARAM}) {
                    my $null_allowed = $param->{ATTR}->{'null_allowed'};
                    if (!defined $null_allowed) {
                        push @driver_pre_call, "if ($driver_in_arg.$name\_isnull) {";
                        push @driver_pre_call, "    BDBG_ERR((\"NULL not allowed for $name param in $func->{FUNCNAME}: %d\", __LINE__));";
                        push @driver_pre_call, "    goto err_fault;";
                        push @driver_pre_call, "}";
                    }
                    push @proxy_pre_call, "if($name!=NULL) {";
                    push @proxy_pre_call, "    $driver_in_arg.$name\_isnull = false;";
                    push @proxy_pre_call, "    $driver_in_arg.$name = * $name;";
                    push @proxy_pre_call, "} else {";
                    push @proxy_pre_call, "    $driver_in_arg.$name\_isnull = true;";
                    push @proxy_pre_call, "}";
                } else {
                    # NULL out param will crash the client; harmless to server
                    push @proxy_post_success, "* $name = $driver_out_arg.$name;";
                }
            }
            else {
                # must be inparam
                push @proxy_pre_call, "$driver_in_arg.$name = $name;";
            }
        }

        # must exchange eInvalid and NULL for protected clients for Timebase and ParserBand as direct params
        # this code only handles output param conversion post call
        # pre-call, post-verification NULL->eInvalid input conversion is done in bapi_ipc_server.pm
        if ($param->{ISREF})
        {
            if ($param->{BASETYPE} eq 'NEXUS_Timebase' || $param->{BASETYPE} eq 'NEXUS_ParserBand')
            {
                if (!$param->{INPARAM})
                {
                    push @driver_post_success, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                    push @driver_post_success, "  if ($driver_out_arg.$name != NULL) {"; # check pointer
                    push @driver_post_success, "    if (*$driver_out_arg.$name == $param->{BASETYPE}_eInvalid) {"; # check pointed to value
                    push @driver_post_success, "      *$driver_out_arg.$name = ($param->{BASETYPE})NULL;";
                    push @driver_post_success, "    }";
                    push @driver_post_success, "  }";
                    push @driver_post_success, "}";
                }
            }
        }
        
        if($param->{BASETYPE} eq 'NEXUS_CallbackDesc') {
            if ($param->{INPARAM}) {
                my $handle = "$driver_in_arg.$$params[0]->{NAME}";
                my $id = sprintf("0x%05x", ((bapi_util::func_id $funcs, $func)*256 + $arg_no + 0x10000));
                my $notnull_test = "! $driver_in_arg.$name\_isnull";
                push @driver_pre_call, "if($notnull_test) {";
                push @driver_pre_call, "   /* MAP callback from proxy space to the driver space */";
                push @driver_pre_call, "   NEXUS_DRIVER_CALLBACK_TO_DRIVER(&$driver_in_arg.$name, $handle, $id);";
                push @driver_pre_call, "}";
                push @driver_post_success, "if($notnull_test) {";
                push @driver_post_success, "    /* after function succeded, commit changes */";
                push @driver_post_success, "    NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(&$driver_in_arg.$name, $handle, $id);";
                push @driver_post_success, "}";
                push @driver_post_error, "if($notnull_test) {";
                push @driver_post_error, "    /* if error occured, then cancel changes in the callback mapping */";
                push @driver_post_error, "    NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(&$driver_in_arg.$name, $handle, $id);";
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
                my $field_driver_in = "$driver_in_arg.$name.$field->{NAME}";
                my $field_driver_out = "$driver_out_arg.$name.$field->{NAME}";

                $field_no ++;
                if($field->{TYPE} eq 'NEXUS_CallbackDesc') {
                    my $id = sprintf("0x%04x", ((bapi_util::struct_id $structs, $param->{BASETYPE})*256 + $field_no));
                    my $handle;
                    if ($func->{RETTYPE_ISHANDLE}) {
                        $handle = 'NULL'; # update after sucess
                    }
                    elsif ($$params[0]->{ISHANDLE}) {
                        $handle = "$driver_in_arg.$$params[0]->{NAME}";
                    }
                    elsif (!$$params[0]->{ISREF} && $param != $$params[0]) {
                        # enum instead of handle.
                        # use enum type + enum value as psuedo handle
                        # hardcoded for a max of 128 enum values per type
                        # value<<2+1 to use non-4 byte aligned value to ensure no conflict with actual handle
                        $handle = "(($id<<9) + ($driver_in_arg.$$params[0]->{NAME}<<2) + 1)";
                    }
                    if (defined $handle && $param->{ISREF}) {
                        if ($param->{INPARAM}) {
                            push @pre_update_driver, "/* MAP callback from proxy space to the driver space */";
                            push @pre_update_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER(&$field_driver_in, $handle, $id);";
                            if($handle eq 'NULL') {
                                push @post_update_driver, "/* since callback wasn't avaliable before calling function, use the return result to update callback */";
                                push @post_update_driver, "NEXUS_DRIVER_CALLBACK_UPDATE(&$field_driver_in, $handle, $id, $driver_out_arg.__retval);";
                            } else {
                                push @post_update_driver, "/* after function succeded, commit changes */";
                                push @post_update_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER_COMMIT(&$field_driver_in, $handle, $id);";
                            }
                            push @post_error_driver, "/* if error occured, then cancel changes in the callback mapping */";
                            push @post_error_driver, "NEXUS_DRIVER_CALLBACK_TO_DRIVER_CANCEL(&$field_driver_in, $handle, $id);";
                        } else {
                            push @post_update_driver, "/* MAP callback from the driver space to proxy space */";
                            push @post_update_driver, "NEXUS_DRIVER_CALLBACK_TO_CLIENT(&$field_driver_out, $handle, $id);";
                        }
                    }
                }
                # must exchange eInvalid and NULL for protected clients for Timebase and ParserBand as struct members
                # this code only handles output param conversion post call
                # pre-call, post-verification NULL->eInvalid input conversion is done in bapi_driver_ioctl.pm
                # TODO: is field as pointer supported?
                elsif ($field->{TYPE} eq 'NEXUS_Timebase' || $field->{TYPE} eq 'NEXUS_ParserBand')
                {
                    if (!$param->{INPARAM})
                    {
                        push @driver_post_success, "if (client_module_state->client->config.mode != NEXUS_ClientMode_eUnprotected) {";
                        push @driver_post_success, "  if ($field_driver_out == $field->{TYPE}_eInvalid) {";
                        push @driver_post_success, "    $field_driver_out = ($field->{TYPE})NULL;";
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
                        my $field_copy = "$driver_in_arg.$name.$field->{NAME}";
                        if($attr eq 'memory') {
                            if($value eq 'cached') {
                                if ($param->{ISREF}) {
                                    if ($param->{INPARAM}) {
                                        next if (exists $param->{ATTR}->{'nelem'});
                                        push @pre_update_proxy, "/* convert address to the device offset */";
                                        push @pre_update_proxy, "$field_copy = $field_copy ? (void *) NEXUS_AddrToOffset($field_copy) : $field_copy;";

                                        push @pre_update_driver, "/* convert offset (substituted by the proxy ) to the cached address */";
                                        push @pre_update_driver, "NEXUS_DRIVER_RECV_ADDR($field_driver_in,cached);";
                                        next;
                                    } else {
                                        push @post_update_proxy, "/* convert offset (substituted by the driver) to the cached address */";
                                        push @post_update_proxy, "$field_proxy = $field_proxy ? NEXUS_OffsetToCachedAddr((uint32_t)$field_proxy) : $field_proxy ;";

                                        push @post_update_driver, "/* convert offset (substituted by the proxy) to the cached address */";
                                        push @post_update_driver, "NEXUS_DRIVER_SEND_ADDR($field_driver_out,cached);";
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
                push @proxy_pre_call, "if($name) {";
                push @proxy_pre_call, "     /* update values ... */";
                bapi_util::append_code \@proxy_pre_call, \@pre_update_proxy, "     ";
                push @proxy_pre_call, "}";
            }
            bapi_util::append_if \@driver_pre_call, "! $driver_in_arg.$name\_isnull", \@pre_update_driver;
            # out params cannot be null
            bapi_util::append_code \@driver_post_error, \@post_error_driver;
            bapi_util::append_code \@driver_post_success, \@post_update_driver;
            bapi_util::append_code \@proxy_post_success , \@post_update_proxy;
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
                next if($attr eq 'reserved'); # unnecessary for socket ipc

                if(($attr eq 'nelem' || $attr eq 'fixed_length')) {
                    # unix domain sockets can pass 4K atomically. to make the implementation much simpler, the thunk
                    # requires that all params (including nelem and nelem_out params) to fit within one packet.
                    # anything greater will cause the call to fail. # the app can then be rewritten to request smaller chunks.
                    if ($param->{INPARAM}) {
                        my $length = "$value";
                        my $array_type;
    
                        if (exists $param->{ATTR}->{'nelem_convert'}) {
                            $length =  $param->{ATTR}->{'nelem_convert'} . "($length)";
                        }
                        if($param->{BASETYPE} ne 'void') {
                            $length = "(sizeof($param->{BASETYPE})*($length))";
                            $array_type = $param->{BASETYPE};
                        }
                        else {
                            $array_type = "uint8_t";
                        }
                        
                        push @proxy_pre_call, "if (__variable_in_offset+$length>__data_size) {BDBG_ERR((\"length exceeded %d > %d: %d\", __variable_in_offset+$length, __data_size, __LINE__)); goto err_call;}";
                        push @proxy_pre_call, "$driver_in_arg.${name}_variable_offset = __variable_in_offset;";
                        push @proxy_pre_call, "__variable_in_offset += $length;";
                        push @proxy_pre_call, "{";
                        push @proxy_pre_call, "  $array_type *dest = ($array_type*)&(&$driver_in_arg.variable_params)[$driver_in_arg.${name}_variable_offset];";
                        push @proxy_pre_call, "  BKNI_Memcpy(dest, $name, $length);";
                        
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
                                            my $field_copy = "dest[i].$field->{NAME}";
                                            push @proxy_update_structure, "    /* convert address to the device offset */";
                                            push @proxy_update_structure, "    $field_copy = (void *) NEXUS_AddrToOffset($field_copy);";

                                            push @driver_update_structure, "    /* convert offset (substituted by the proxy) to the cached address */";
                                            push @driver_update_structure, "    NEXUS_DRIVER_RECV_ADDR($field_copy,cached);";
                                            next;
                                        }
                                        print STDERR "ERROR: Unsupported attribute $attr = $value for $field->{NAME} in $param->{BASETYPE}\n";
                                    }
                                }
                            }
                        }
                        if (scalar @driver_update_structure) {
                            # no dest pointer needed unless field conversion is required    
                            push @driver_pre_call, "{";
                            push @driver_pre_call, "  $array_type *dest = ($array_type*)&(&$driver_in_arg.variable_params)[$driver_in_arg.${name}_variable_offset];";
                            push @driver_pre_call, "  {unsigned i;for(i=0;i<$driver_in_arg.$value;i++) {";
                            push @driver_pre_call, @driver_update_structure;
                            push @driver_pre_call, "  }}";
                            push @driver_pre_call, "}";
                        }
                        if (scalar @proxy_update_structure) {
                            push @proxy_pre_call, "  {unsigned i;for(i=0;i<$value;i++) {";
                            push @proxy_pre_call, @proxy_update_structure;
                            push @proxy_pre_call, "  }}";
                        }
                        push @proxy_pre_call, "}";
                    }
                    else {
                        my $driver_length = "$driver_in_arg.$value";
                        my $proxy_length = "$value";
                        my $proxy_length_out;
    
                        if (exists $param->{ATTR}->{'nelem_out'}) {
                            $proxy_length_out = "$driver_out_arg.$param->{ATTR}->{'nelem_out'}";
                        }
                        else {
                            $proxy_length_out = $proxy_length;
                        }
                        if (exists $param->{ATTR}->{'nelem_convert'}) {
                            $proxy_length_out =  $param->{ATTR}->{'nelem_convert'} . "($proxy_length_out)";
                        }
    
                        if($param->{BASETYPE} ne 'void') {
                            $proxy_length_out = "(sizeof($param->{BASETYPE})*($proxy_length_out))";
                            $proxy_length = "(sizeof($param->{BASETYPE})*($proxy_length))";
                            $driver_length = "(sizeof($param->{BASETYPE})*($driver_length))";
                        }
                        
                        push @driver_pre_call, "if (*p_out_data_size-sizeof(void*)+__variable_out_offset+$driver_length>out_mem_size) {BDBG_ERR((\"length exceeded %d-%d+%d+%d > %d: %d\", *p_out_data_size,sizeof(void*),__variable_out_offset,$driver_length,out_mem_size, __LINE__)); goto err_fault;}";
                        push @driver_pre_call, "$driver_out_arg.${name}_variable_offset = __variable_out_offset;";
                        push @driver_pre_call, "__variable_out_offset += $driver_length;";
                        push @driver_pre_call, "(*p_out_data_size) += $driver_length;";
                        push @proxy_post_success, "BKNI_Memcpy($name, &(&$driver_out_arg.variable_params)[$driver_out_arg.${name}_variable_offset], $proxy_length_out);";
                    }
                }
                elsif($attr eq 'memory' && $value eq 'cached' ) {
                    if ($param->{INPARAM}) {
                        push @proxy_pre_call, "/* convert offset (substituted by the driver) to the cached address */";
                        push @proxy_pre_call, "$driver_in_arg.$name = (void *)NEXUS_AddrToOffset((void *)$name);";
                        # driver_pre_call NEXUS_OffsetToCachedAddr handled in process_function_param
                    }
                    else {
                        # handle return value. if client passes NULL outparam, only the client crashes
                        push @proxy_post_success, "/* convert offset (substituted by the driver) to the cached address */";
                        push @proxy_post_success, "*$name = $driver_out_arg.$name ? NEXUS_OffsetToCachedAddr((uint32_t)$driver_out_arg.$name) : NULL;";
                        push @driver_post_success, "/* convert address to the device offset */";
                        push @driver_post_success, "NEXUS_DRIVER_SEND_ADDR($driver_out_arg.$name, cached);";
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

    $result{'driver_overwrite'} = \%driver_overwrite;
    return \%result;
}


1;

