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
# $brcm_Revision: 7 $
# $brcm_Date: 8/22/12 1:24p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/ipcthunk/bapi_build.pl $
# 
# 7   8/22/12 1:24p erickson
# SW7231-958: use .preload files to get struct definitions from other
#  modules
# 
# 6   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 5   8/8/11 5:10p erickson
# SW7420-1689: add untrusted client api enforcement
# 
# 4   3/1/11 9:34a erickson
# SW7420-1123: use bapi_common::skip_thunk
#
# 3   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 2   11/3/10 5:32p erickson
# SW7420-1223: exclude nexus_base_mmap.h from thunks
#
# 1   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
#
#############################################################################
use strict;

use lib ".";
use lib "../common";
use bapi_parse_c;
use bapi_ipc_apidef;
use bapi_ipc_client;
use bapi_ipc_server;
use bapi_untrusted_api;
use Getopt::Long;

my $file;
my @funcs;
my %structs;
my $source_file;
my $file_headers;
my @file_list;

if ($#ARGV == -1) {
    print "Usage: perl bapi_build.pl [--source listfile] module destdir class_handles file1.h file2.h ...\n";
    exit;
}

#check presence of a sources file and process if present
GetOptions ('source=s' => \$source_file);
if($source_file) {
    open(HEADERS, $source_file) || die("Could not open source file: $source_file");
    $file_headers=<HEADERS>;
    close(HEADERS);

    chomp($file_headers);
    @file_list = split(/\s+/,$file_headers);
    my $count;
    $count = @file_list;
    print "Sources file $source_file contains $count headers\n";
}

# Process all files on the command line
my $module = lc shift @ARGV;
my $destdir = shift @ARGV;

# read class_handles.inc file into a list
my $class_list_file = shift @ARGV;
open FILE, $class_list_file;
my @class_handles = <FILE>;
close FILE;
chomp @class_handles;

my %preload_structs;
for $file (@file_list,@ARGV) {
    my $name;
    my $members;
    next if (bapi_classes::skip_thunk($file));
    #print "ipcthunk/bapi_build.pl parsing $file\n";
    
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
                #print "ipcthunk/bapi_build.pl parsing preload $dir/$file\n";
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

# Print out the perl datastructure
#bapi_parse_c::print_api @funcrefs;
#bapi_parse_c::print_struct \%structs;

my $version = bapi_parse_c::version_api @funcrefs;
$version = ($version * 0x10000) + bapi_parse_c::version_struct \%structs;

my $untrusted_api = bapi_untrusted_api::parse "../common/nexus_untrusted_api.txt";

# Build thunk layer
bapi_ipc_apidef::generate "${destdir}/" . (bapi_common::ipc_header $module) , $module, $version, @funcrefs;
bapi_ipc_client::generate "${destdir}/nexus_${module}_ipc_client.c", $module, $version, \%structs, @funcrefs;
bapi_ipc_server::generate "${destdir}/nexus_${module}_ipc_server.c", $module, $version, \%structs, \@funcrefs, \@class_handles, $untrusted_api;
