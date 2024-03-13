#!/usr/bin/perl
#     (c)2003-2011 Broadcom Corporation
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
# $brcm_Revision: 8 $
# $brcm_Date: 12/7/11 10:41a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/syncthunk/bapi_build.pl $
# 
# 8   12/7/11 10:41a erickson
# SW7420-2141: merge as much duplicated kernelmode proxy/usermode ipc
#  perl code as possible
# 
# 7   3/17/11 4:57p erickson
# SW7420-1661: test for duplicate functions to avoid obscure nexus link
#  failures
#
# 6   3/1/11 9:34a erickson
# SW7420-1123: use bapi_common::skip_thunk
#
# 5   1/19/11 2:02p erickson
# SW7420-1123: socket-based usermode IPC
#
# 4   11/3/10 5:32p erickson
# SW7420-1223: exclude nexus_base_mmap.h from thunks
#
# 3   7/14/10 11:56a erickson
# SW7405-4621: exclude nexus_base_os.h from thunk
#
# 2   3/23/10 3:40p erickson
# SWDEPRECATED-2423: add --source option for build systems with cmdline
#  limitations
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/4   10/12/07 11:47a erickson
# PR36066: remove "lc" when getting destdir
#
# Nexus_Devel/3   10/2/07 11:54a jgarrett
# PR 34416: Removing print
#
# Nexus_Devel/2   9/28/07 10:02a erickson
# PR35395: remove autogen from thunk file name
#
# Nexus_Devel/1   9/27/07 1:33p erickson
# PR35395: initial impl
#
#############################################################################
use strict;

use lib ".";
use lib "../common";
use bapi_parse_c;
use bapi_thunks;
use bapi_classes;
use Getopt::Long;

my $file;
my @funcs;
my $source_file;
my $file_headers;
my @file_list;

if ($#ARGV == -1) {
    print "Usage: perl bapi_build.pl [--source listfile] module destdir file1.h file2.h ...\n";
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

for $file (@file_list,@ARGV) {
    next if (bapi_classes::skip_thunk($file));
    #print "syncthunk/bapi_build.pl parsing $file\n";

    push @funcs, bapi_parse_c::get_func_prototypes $file;
}

# Build the perl datastructure
my @funcrefs = bapi_parse_c::parse_funcs @funcs;

# test for duplicate functions. these will fail later in the link, but we can catch the failure now.
# this can be from copying a header file (for example, cp nexus_types.h nexus_types.backup.h). the *.h will suck that in.
# it can also happen if you using #if. the perl thunk does not recognize that.
my $f1;
for $f1 (@funcrefs) {
    my $f2;
    my $count = 0;
    for $f2 (@funcrefs) {
        if ($f2->{FUNCNAME} eq $f1->{FUNCNAME}) {
            if (++$count == 2) {
                print "ERROR: Duplicate function $f1->{FUNCNAME} found within $module module.\n";
                print "ERROR:   You may have unnecessary copies of header files in your public API directory.\n";
                print "ERROR:   You may be using #if around duplicate prototypes. Nexus does not run a preprocessor in its thunk.\n";
                exit 1;
            }
        }
    }
}

# Print out the perl datastructure
#bapi_parse_c::print_api @funcrefs;

#print "Building synchronization thunk for module ${module}\n";

# Build thunk layer
bapi_thunks::build_thunks ${module}, "${destdir}/nexus_${module}_thunks.c", \@funcrefs;
bapi_thunks::build_remapping "${destdir}/nexus_${module}_thunks.h", \@funcrefs;

