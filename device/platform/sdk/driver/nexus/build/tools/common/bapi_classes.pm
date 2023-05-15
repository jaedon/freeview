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
# $brcm_Workfile: bapi_classes.pm $
# $brcm_Revision: 6 $
# $brcm_Date: 7/23/12 11:35a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/common/bapi_classes.pm $
# 
# 6   7/23/12 11:35a vsilyaev
# SW7420-2328: Use per-thread storage to keep information about the
#  current client, remove per-module client information
# 
# 5   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 4   6/22/12 1:03p vsilyaev
# SW7420-2085: Fixed typo
# 
# 3   6/19/12 12:41p vsilyaev
# SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
# 
# 2   3/21/12 2:32p erickson
# SW7340-296: remove unused secondary_destructor attribute
# 
# 1   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
#############################################################################
use strict;

package bapi_classes;

# generate hash of destructor functions mapped to their various destructors by attr
# the key is the constructor
# one destructor per attr per function
# the validation of destructors happens only here. every one else trusts this output.
sub get_destructors
{
    my ($funcs) = @_;
    my $func;
    my ($attr,$value);
    my %destructors;

    for $func (@$funcs) {
        if(exists $func->{ATTR}) {
            while (($attr, $value) = each %{$func->{ATTR}} ) {
                # one "if" clause for each prototype of destructor. valid prototypes are:
                # 1) void destructor(resource_handle)
                # 2) void destructor(main_handle, resource_handle)
                if($attr eq 'destructor' || $attr eq 'release') {
                    my $destructor;
                    for (@$funcs) {
                        if ($value eq $_->{FUNCNAME}) {
                            $destructor = $_;
                            last;
                        }
                    }
                   if (defined $destructor && $destructor->{RETTYPE} eq 'void' &&
                       (scalar @{$destructor->{PARAMS}}) == 1 &&
                       ${$destructor->{PARAMS}}[0]->{TYPE} eq $func->{RETTYPE} )
                    {
                        $destructors{$func}->{$attr} = $destructor;
                        next;
                    }
                    print STDERR "ERROR: invalid $attr $value in function $func->{FUNCNAME}\n";
                }
            }
        }
    }
    return \%destructors;
}

# check if a function is a destructor and which handle is being destroyed
sub get_stopcallbacks_handle
{
    my ($func, $destructors) = @_;
    my $d;

    for $d (keys %{$destructors}) {
        if ($destructors->{$d}->{'destructor'} == $func) {
            return $func->{PARAMS}[0]->{NAME};
        }
    }
    return undef;
}

# return a list of classes
sub get_classes
{
    my ($funcs, $destructors) = @_;
    my $func;
    my %classes;
    my $class;

    for $func (@$funcs) {
        my $attr;
        foreach $attr (keys %{$destructors->{$func}}) {
            my $destructor = $destructors->{$func}->{$attr};

            #print "$func->{FUNCNAME} $attr -> $destructor->{FUNCNAME}\n";
            if ($attr eq "release") {
                my $class_type = $func->{RETTYPE};
                $class = $classes{$class_type};

                if(defined $class && defined $class->{RELEASE}) {
                    print STDERR "ERROR: cannot have more than one acquire/release acquire=$func->{FUNCNAME}, release=$destructor->{FUNCNAME}\n";
                }
                else {
                    $class->{RELEASE} = $destructor;
                    $class->{ACQUIRE} = $func;
                    $classes{$class_type} = $class;
                }
            }
            else {
                my $class_type = $func->{RETTYPE};

                $class = $classes{$class_type};

                if(defined $class && $class->{DESTRUCTOR}) {
                    # you can have more than one constructor per class, but only one destructor per class
                    if($class->{DESTRUCTOR} == $destructor) {
                        push @{$class->{CONSTRUCTORS}}, $func;
                    } else {
                        print STDERR "ERROR: destructor mismatch $destructor->{FUNCNAME} for function $func->{FUNCNAME}:$func->{RETTYPE}\n";
                        print STDERR "  $class->{DESTRUCTOR} != $destructor\n";
                    }
                } else {
                    $class->{DESTRUCTOR} = $destructor;
                    $class->{CONSTRUCTORS} = [$func];
                    $class->{DESTRUCTOR_TYPE} = $attr;
                    $class->{CLASS_TYPE} = $class_type;
                    $class->{CLASS_NAME} = get_class_name($class_type);
                    $classes{$class_type} = $class;
                }
            }
        }
        if (exists $func->{ATTR}->{"shutdown"}) {
            my $class_type = $func->{PARAMS}[0]->{TYPE};
            $class = $classes{$class_type};
            my $shutdown;
            $shutdown->{'shutdown_target'} = $func->{ATTR}->{'shutdown'};
            $shutdown->{'shutdown_get_connector'} = $func->{FUNCNAME};
            push @{$class->{SHUTDOWN}}, $shutdown;
            $classes{$class_type} = $class;
        }
        
    }

    my @c = values %classes;
    return \@c;
}

sub callable_by_untrusted
{
    my ($func, $classes, $untrusted_api) = @_;

    my $allow_funcs = $untrusted_api->{'allow_funcs'};
    my $deny_funcs = $untrusted_api->{'deny_funcs'};
    my $params = $func->{PARAMS};

    if (grep {$_ eq $func->{FUNCNAME}} @$allow_funcs) {
        return 1;
    }

    if (grep {$_ eq $func->{FUNCNAME}} @$deny_funcs) {
        return 0;
    }

    if ((scalar @{$params}) == 1 && !${$params}[0]->{INPARAM}) {
        # GetDefaultSettings allowed
        return 1;
    }

    # first param is validated handle
    if ((scalar @{$params}) >= 1) {
        if (${$params}[0]->{TYPE} eq 'NEXUS_HeapHandle') {
            return 1;
        }
        for (@$classes) {
            my $class = $_;
            my $handletype = $class->{CLASS_TYPE};
            if (${$params}[0]->{TYPE} eq $handletype) {
                return 1;
            }
        }
    }

    # untrusted
    return 0;
}

sub skip_thunk
{
    my $file = shift;
    return $file =~ /_init.h$/ ||
        $file =~ /nexus_platform_client.h$/ ||
        $file =~ /nexus_base\w*.h$/;
}

sub get_class_name {
    my $type = shift;
    $type =~ s/Handle$//;
    return $type;
}

# generate global functions which verify handles per module
# this prevents the need for a global class database. 
sub generate_class_verify_functions
{
    my ($file, $classes, $module) = @_;
    $module = lc $module;
    for (@$classes) {
        my $class = $_;
        my $handletype = $_->{CLASS_TYPE};
        my $class_name = $_->{CLASS_NAME};

        # required for cross-module handle verification
        print $file "int NEXUS_OBJECT_VERIFY_AND_ACQUIRE_NAME($class_name)(void *object, NEXUS_ModuleHandle origin) {\n";
        print $file "  NEXUS_ModuleHandle target = NEXUS_MODULE_SELF;\n";
        print $file "  return b_objdb_verify_and_acquire(&nexus_driver_module_state.header.objdb, &NEXUS_OBJECT_DESCRIPTOR($class_name), object, origin==target?NULL:target, __FILE__, __LINE__);\n";
        print $file "}\n";

        print $file "void NEXUS_OBJECT_RELEASE_AFTER_VERIFY_NAME($class_name)(void *object, NEXUS_ModuleHandle origin) {\n";
        print $file "  NEXUS_ModuleHandle target = NEXUS_MODULE_SELF;\n";
        print $file "  const struct b_objdb_client *client = b_objdb_get_client();\n";
        print $file "  if (client && client->verify == b_objdb_verify_none) {return;}\n";
        print $file "  NEXUS_BaseObject_P_Release_Tagged(NULL, object, &NEXUS_OBJECT_DESCRIPTOR($class_name), origin==target?NULL:target, __FILE__, __LINE__);\n";
        print $file "}\n";

    }
}

sub generate_class_table
{
    my ($file, $classes)=@_;
    if(scalar @$classes) {
        my $class;
                
        # create local close function so that pre-close functions can be called
        for $class (@$classes) {
            next if ($class->{DESTRUCTOR_TYPE} ne 'destructor');
            next if (!exists $class->{SHUTDOWN});
            
            my $handletype = $class->{CLASS_TYPE};
            my $func = $class->{DESTRUCTOR};
            
            # shutdown_close will perform a platform-level shutdown of the connector, then close.
            print $file "static void nexus_driver_shutdown_close_$handletype($handletype handle)\n";
            print $file "{\n";
            print $file "  NEXUS_UnlockModule();\n";
            for (@{$class->{SHUTDOWN}}) {
                my $shutdown_target = $_->{'shutdown_target'};
                my $shutdown_get_connector = $_->{'shutdown_get_connector'};
                my $params = $func->{PARAMS};
                print $file "  nexus_driver_shutdown_$shutdown_target($shutdown_get_connector(handle));\n";
            }
            print $file "  NEXUS_LockModule();\n";
            # actual close
            print $file "  $func->{FUNCNAME}(handle);\n";
            print $file "}\n\n";
            
        }
        
        print $file "B_OBJDB_TABLE_BEGIN(NEXUS_DRIVER_MODULE_CLASS_TABLE)\n";
        for $class (@$classes) {
            my $class_type = $class->{CLASS_TYPE};
            my $class_name = $class->{CLASS_NAME};
            my $destructorfunc;
            if ($class->{DESTRUCTOR_TYPE} eq 'destructor' && exists $class->{SHUTDOWN}) {
                $destructorfunc = "nexus_driver_shutdown_close_$class_type";
            }
            else {
                $destructorfunc = $class->{DESTRUCTOR}->{FUNCNAME};
            }
            my $releasefunc = $class->{RELEASE}->{FUNCNAME};
            if (!defined $destructorfunc) {
                $destructorfunc = "ERROR: missing destructor"; # required. this will cause the compilation to fail.
            }
            if (!defined $releasefunc) {
                $releasefunc = "NULL"; # not required
            }
            print $file "    B_OBJDB_TABLE_ENTRY($class_name,$releasefunc,$destructorfunc)\n";
        }
        print $file "B_OBJDB_TABLE_END\n";
    }
}

1;
