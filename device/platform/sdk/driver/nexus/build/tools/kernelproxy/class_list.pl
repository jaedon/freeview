#############################################################################
#    (c)2008-2012 Broadcom Corporation
#
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: class_list.pl $
# $brcm_Revision: 15 $
# $brcm_Date: 7/23/12 11:35a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/class_list.pl $
# 
# 15   7/23/12 11:35a vsilyaev
# SW7420-2328: Use per-thread storage to keep information about the
#  current client, remove per-module client information
# 
# 14   7/16/12 1:01p vsilyaev
# SW7420-2324: Use atomic verify and acquire for handle verification
# 
# 13   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 12   11/15/11 11:07a erickson
# SW7231-391: fix stub macro
# 
# 11   11/1/11 2:51p erickson
# SW7231-391: fix naming, warnings
# 
# 10   11/1/11 11:21a erickson
# SW7231-391: add macro to test if nexus class exists
# 
# 9   9/2/11 12:39p erickson
# SW7420-1148: refactor class_list.pl to generate per-module "get current
#  client" functions
# 
# 8   9/2/11 10:03a erickson
# SW7420-1148: allow nexus_register/unregister to objdb from nexus
#  module. enables more complex resource management.
# 
# 7   7/25/11 2:46p erickson
# SW7420-1992: allow class with secondary_destructor and release, no
#  destructor
# 
# 6   7/12/11 1:26p erickson
# SW7420-1979: add support for new shutdown attribute
# 
# 5   5/17/11 12:26p erickson
# SW7420-1679: NEXUS_Memory_Free must check pointer with objdb before
#  freeing
# 
# 4   3/1/11 9:34a erickson
# SW7420-1123: use bapi_common::skip_thunk
#
# 3   1/19/11 3:42p erickson
# SW7420-1123: refactor kernelproxy to correspond to usermode changes
#
# 2   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 1   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
#############################################################################
use strict;
use lib ".";
use lib "../common";
use bapi_parse_c;
use bapi_common;
use bapi_classes;

my $class_file = shift @ARGV;
my $class_verification_file = shift @ARGV;
my $file;
my @funcs;
my %structs;
my @modules;

# @ARGV is a list of all nexus public api header files and module names
# -headers FILE FILE FILE -modules MODULE MODULE MODULE
my $parsing_headers;
foreach $file (@ARGV) {
    if ($file eq "-headers") {
        $parsing_headers = 1;
        next;
    }
    elsif ($file eq "-modules") {
        undef $parsing_headers;
        next;
    }
    
    if ($parsing_headers) {
        next if (bapi_classes::skip_thunk($file));
        #print "kernelproxy/class_list.pl parsing $file\n";
    
        push @funcs, bapi_parse_c::get_func_prototypes $file;
    
        my $file_structs = bapi_parse_c::parse_struct $file;
        my $name;
        my $members;
        while (($name, $members) = each %$file_structs) {
            $structs{$name} = $members;
        }
    }
    else {
        push @modules, $file;
    }
}

# Build the perl datastructure
my @funcrefs = bapi_parse_c::parse_funcs @funcs;
my $funcs = \@funcrefs;

# create a file which contains the handles for every "class" (an interface with a constructor/destructor)
my $destructors = bapi_classes::get_destructors $funcs;
my $classes = bapi_classes::get_classes $funcs, $destructors;

# create nexus/platforms/$(NEXUS_PLATFORM)/bin/syncthunk/class_list.inc
open(OUTFILE, ">$class_file") or die "Unable to open output file $class_file";
for (@$classes) {
    my $class = $_;
    #print "class $class->{CLASS_TYPE}\n";
    print OUTFILE "$class->{CLASS_TYPE}\n";
}
close OUTFILE;

# create nexus/platforms/$(NEXUS_PLATFORM)/bin/syncthunk/nexus_class_verification.h
open(OUTFILE, ">$class_verification_file") or die "Unable to open output file $class_verification_file";
for (@$classes) {
    my $class = $_;
    my $handletype = $class->{CLASS_TYPE};
    my $class_name = $_->{CLASS_NAME};

    print OUTFILE "#if NEXUS_SERVER_SUPPORT\n";
    print OUTFILE "#define ${handletype}_CLASS 1\n";
    print OUTFILE "int NEXUS_OBJECT_VERIFY_AND_ACQUIRE_NAME($class_name)(void *object, NEXUS_ModuleHandle origin);\n";
    print OUTFILE "void NEXUS_OBJECT_RELEASE_AFTER_VERIFY_NAME($class_name)(void *object, NEXUS_ModuleHandle origin);\n";
    print OUTFILE "#endif\n";
    
    if (exists $class->{SHUTDOWN}) {
        for (@{$class->{SHUTDOWN}}) {
            my $shutdown_target = $_->{'shutdown_target'};
            print OUTFILE "extern void nexus_driver_shutdown_$shutdown_target(void *);\n";
        }
    }
}

close OUTFILE;
