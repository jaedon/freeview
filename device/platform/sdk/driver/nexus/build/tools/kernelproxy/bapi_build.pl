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
# $brcm_Workfile: bapi_build.pl $
# $brcm_Revision: 16 $
# $brcm_Date: 8/22/12 1:24p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/kernelproxy/bapi_build.pl $
# 
# 16   8/22/12 1:24p erickson
# SW7231-958: use .preload files to get struct definitions from other
#  modules
# 
# 15   7/26/12 4:46p vsilyaev
# SW7435-280: Moved NEXUS_Time_Add to the private API
# 
# 14   7/25/12 9:27a erickson
# SW7420-1148: add NEXUS_Time_Get export
# 
# 13   4/18/12 5:36p erickson
# SW7420-1148: don't run nexus_base_*.h through C parser. just manually
#  export useful functions.
# 
# 12   4/10/12 11:36a erickson
# SW7420-1148: add three more exports without changing C parser
# 
# 11   4/9/12 10:20a erickson
# SW7420-1148: fix kernel mode for previous checkin
# 
# 10   4/6/12 10:20a erickson
# SW7420-1148: allow nexus_base_*.h to have kernel export
# 
# 9   8/8/11 5:10p erickson
# SW7420-1689: add untrusted client api enforcement
# 
# 6   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
# 
# 5   11/3/10 5:32p erickson
# SW7420-1223: exclude nexus_base_mmap.h from thunks
# 
# 4   9/15/10 3:44p jtna
# SW7420-1004: add synthunk for CORE and PLATFORM
# 
# 3   7/14/10 11:56a erickson
# SW7405-4621: exclude nexus_base_os.h from thunk
#
# 2   9/22/09 1:54p gmohile
# SW7405-3060 : Export kernel symbols
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/2   1/11/08 4:27p vsilyaev
# PR 38682: Adding kernel support
#
# Nexus_Devel/1   9/27/07 1:33p erickson
# PR35395: initial impl
#
#############################################################################
use strict;

use lib "../common";
use bapi_parse_c;
use bapi_ioctl_def;
use bapi_usercall;
use bapi_driver_ioctl;
use bapi_kernel_export;
use bapi_untrusted_api;

my $file;
my @funcs;
my %structs;

if ($#ARGV == -1) {
    print "Usage: perl bapi_build.pl module destdir class_handles file1.h file2.h ...\n";
    exit;
}

# Process all files on the command line
my $module = shift @ARGV;
my $module_lc = lc $module;
my $destdir = shift @ARGV;

# read class_handles.inc file into a list
my $class_list_file = shift @ARGV;
open FILE, $class_list_file;
my @class_handles = <FILE>;
close FILE;
chomp @class_handles;

my @base_funcs;

my %preload_structs;
for $file (@ARGV) {
    my $name;
    my $members;
    if (($file =~ /_init.h$/ && $file !~ /nexus_platform_init.h$/) ||
        $file =~ /nexus_base_\w+.h$/) {
        next;
    }
    #print "kernelproxy/bapi_build.pl parsing $file\n";

    # a .preload file allows nexus to load structs from other header files
    # for proxy autogen. this removes the need to have attr/callback definitions present 
    # in the same header file.
    if ($file =~ /\.preload/) {
        open FILE, $file;
        my @preload_files = <FILE>;
        close FILE;
        chomp @preload_files;
        my ($dir) = $file =~ /(.+)\/\w+/;
        for $file (@preload_files) {
            # trim comments and test for some non-whitespace
            $file =~ s/(.*)\#.*/$1/g;
            if ($file =~ /\w/) {
                #print "kernelproxy/bapi_build.pl parsing preload $dir/$file\n";
                my $file_structs = bapi_parse_c::parse_struct "$dir\/$file";
                while (($name, $members) = each %$file_structs) {
                    $preload_structs{$name} = $members;
                }
            }
        }
        next;
    }
    push @funcs, bapi_parse_c::get_func_prototypes $file;
    
    my $file_structs = bapi_parse_c::parse_struct $file, \%preload_structs;
    while (($name, $members) = each %$file_structs) {
        $structs{$name} = $members;
    }
}

# Build the perl datastructure
my @funcrefs = bapi_parse_c::parse_funcs @funcs;
my @base_funcrefs = bapi_parse_c::parse_funcs @base_funcs;

if ($module eq "CORE") {
    # these functions from nexus_base_*.h aren't C parser friendly, but are still 
    # of value to export for kernel driver code
    my @export_funcs = ("NEXUS_AddrToOffset",
        "NEXUS_OffsetToCachedAddr",
        "NEXUS_OffsetToUncachedAddr",
        "NEXUS_Thread_GetDefaultSettings",
        "NEXUS_Thread_Create",
        "NEXUS_Thread_Destroy",
        "NEXUS_FlushCache",
        "NEXUS_GetEnv",
        "NEXUS_SetEnv",
        "NEXUS_atoi",
        "NEXUS_Time_Diff",
        "NEXUS_Time_Get");
    for (@export_funcs) {
        push @base_funcrefs, ({FUNCNAME => $_});
    }
}

# Print out the perl datastructure
#bapi_parse_c::print_api @funcrefs;
#bapi_parse_c::print_struct \%structs;

my $version = bapi_parse_c::version_api @funcrefs;
$version = ($version * 0x10000) + bapi_parse_c::version_struct \%structs;

my $untrusted_api = bapi_untrusted_api::parse "../common/nexus_untrusted_api.txt";

# Build thunk layer
bapi_ioctl_def::generate "${destdir}/" . (bapi_common::ioctl_header $module) , $module, 101, $version, @funcrefs;
bapi_usercall::generate "${destdir}/nexus_${module_lc}_proxy.c", $module, \%structs, @funcrefs;
bapi_kernel_export::generate "${destdir}/nexus_${module_lc}_kernel_export.c", $module_lc, (@funcrefs, @base_funcrefs);
bapi_driver_ioctl::generate "${destdir}/nexus_${module_lc}_driver.c", $module, $version, \%structs, \@funcrefs, \@class_handles, $untrusted_api;

