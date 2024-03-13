#!/usr/bin/perl
#############################################################################
#
#		Copyright (c) 2003-2007, Broadcom Corporation.
#		All rights reserved.
#		Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bapi_build.pl $
# $brcm_Revision: 3 $
# $brcm_Date: 7/9/07 1:30p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/proxy/bapi_build.pl $
# 
# 3   7/9/07 1:30p erickson
# PR32377: autogen the thunk layer
# 
# 2   3/10/06 10:49a erickson
# PR17108: added warning to capture incorrect execution
# 
# 1   2/7/05 6:55p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/2   8/12/04 9:28a erickson
# PR11135: reworked bhandle_mgr autogen code to be data driven and close
# down in correct order
# 
# Irvine_BSEAVSW_Devel/1   5/17/04 11:14a erickson
# PR11135: added initial linux proxy impl
#
#############################################################################
use strict;

use lib ".";
use bapi_parse_c;
use bapi_ioctl_def;
use bapi_usercall;
use bapi_driver_ioctl;
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

# Build kernelmode proxy layer for Settop API
bapi_ioctl_def::parse "bsettop_autogen_ioctl.h", @funcrefs;
bapi_usercall::parse "bsettop_autogen_usercall.c", @funcrefs;
bapi_driver_ioctl::parse "bsettop_autogen_driver.c", @funcrefs;
bapi_driver_ioctl::build_closelist "bsettop_autogen_closelist.c", @funcrefs;

# Build thunk layer for Settop API
bapi_thunks::build_thunks "bsettop_autogen_thunks.c", @funcrefs;
bapi_thunks::build_remapping "bsettop_autogen_thunks.h", @funcrefs;

# Build Linux kernel driver export list
bapi_thunks::build_kernel_export "bsettop_autogen_kernel_export.c", @funcrefs;

print "Done\n";
