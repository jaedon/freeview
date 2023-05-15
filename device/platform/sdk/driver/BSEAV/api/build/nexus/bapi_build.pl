#!/usr/bin/perl
#############################################################################
#
#		Copyright (c) 2003-2012, Broadcom Corporation.
#		All rights reserved.
#		Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bapi_build.pl $
# $brcm_Revision: 2 $
# $brcm_Date: 9/6/12 11:28p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/nexus/bapi_build.pl $
# 
# 2   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/6/12 5:28p mphillip
# SW7425-3739: Remove symlinks
# 
# 1   3/25/08 10:13a jgarrett
# PR 40857: Adding shim thunk layer
# 
#############################################################################
use strict;

use lib '../proxy';
use bapi_parse_c;
use bapi_thunks;

my $file;
my @funcs;

if ($#ARGV == -1) {
	print "You must run autogen to build proxy layer\n";
	return 1;
}

# Process all files on the command line
for $file (@ARGV) {
	push @funcs, bapi_parse_c::get_func_prototypes $file;
}

# Build the perl datastructure
my @funcrefs = bapi_parse_c::parse_funcs @funcs;

# Print out the perl datastructure
#bapi_parse_c::print_api @funcrefs;

print "Autogenerating code...\n";

# Build thunk layer for Settop API
bapi_thunks::build_thunks "bsettop_autogen_thunks.c", @funcrefs;
bapi_thunks::build_remapping "bsettop_autogen_thunks.h", @funcrefs;

print "Done\n";
