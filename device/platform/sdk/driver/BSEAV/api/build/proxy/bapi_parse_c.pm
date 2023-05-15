#!/usr/bin/perl
#############################################################################
#
#		Copyright (c) 2003-2009, Broadcom Corporation.
#		All rights reserved.
#		Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bapi_parse_c.pm $
# $brcm_Revision: 3 $
# $brcm_Date: 7/2/09 10:47a $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/proxy/bapi_parse_c.pm $
# 
# 3   7/2/09 10:47a erickson
# PR56558: add bsettop_set_init_settings and bsettop_init_frontend
#
# 2   3/15/06 4:51p vsilyaev
# PR20221: NetIF DMA interface
#
# 1   2/7/05 6:56p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/4   6/29/04 11:26a erickson
# PR11135: const void ** is not an out parameter. Take this into account.
#
# Irvine_BSEAVSW_Devel/3   6/16/04 5:15p erickson
# PR11135: added callback support
#
# Irvine_BSEAVSW_Devel/2   6/1/04 11:28a erickson
# PR11135: added support for strings (const char *)
#
# Irvine_BSEAVSW_Devel/1   5/17/04 11:13a erickson
# PR11135: added initial linux proxy impl
#
#############################################################################
use strict;

package bapi_parse_c;

local *FILE;

sub writeDebug
{
	print "@_\n";
}

sub get_func_prototypes
{
	my $filename = shift;
	open FILE, $filename;
	my $text = join '',<FILE>;

	# pack preprocessor line continutation
	$text =~ s/\\\s+(\w)/$1/sg;
	# remove preprocessor stuff that can cause parsing problems
	$text =~ s/#\w+.*?\n//g;

	# Grab all size_is() hints and move them out of comment temporarily
	# Blow away the comment because it isn't going to stay anyway.
	$text =~ s/
		\/\*			# start of a comment
		\s* \[ [\w,]*		# Valid hints before a possible size_is.
		(size_is\(\w+\)) # Here's what we need to keep
		.*?
		\*\/			# end of the comment
		/$1/sgx;

	# Remove all comments
	# No nested comments allowed
	$text =~ s/\/\*.*?\*\///sg;

	# If we have any ", size_is", reverse the order. Much easier to parse later.
	$text =~ s/(,)\s*(size_is\(\w+\))/ $2$1/sg;

	# Parse functions
	my @funcs = $text =~ /(
		\w[\w\s]+	# return type
		[\s\*]		# delimiter or \s or *
		\w+ 		# function name
		\(.*?\))	# parameters
		\s*;		# must terminate with ;
		/xsg;

	@funcs;
}

# parse_func
#
# The function data structure is a hash with the following members:
# PROTOTYPE = string, the original prototype
# RETTYPE = string, the return type
# FUNCNAME = string, the function name
# PARAMS = reference to array of hash references
#	Each hash reference is a single parameter with the following members:
#	  NAME
#	  TYPE
#
# This subroutine returns a reference to the function hash.
#
sub parse_func
{
	my $prototype = shift;
	my %func;
	my $more;
	my @params; # this is a list of a param hash

	# remove the size_is hint from the actual prototype
	my $actual_prototype = $prototype;
	$actual_prototype =~ s/size_is\(\w+\)//sg;
	$func{PROTOTYPE} = $actual_prototype;

	($func{RETTYPE}, $more) = $prototype =~ /(.*?)\s*([\s*\*])\w+\s*\(/;
	if ($more eq "*") {
		$func{RETTYPE} .= $more;
	}
	($func{FUNCNAME}) = $prototype =~ /(\w+)\s*\(/;

	#print "$prototype\n";

	# get the params into a raw list
	my ($params) = $prototype =~ /\((.*?)\)$/s;
	if ($params eq "void") {
		$params = undef;
	}
	my @rawparams = split /,/, $params;
	my $p;

	for $p (@rawparams) {
		my %paramhash;

		# See if we have a size_is hint and grab it now
		# This also removes that hint from the variable
		if ($p =~ s/size_is\((\w+)\)//) {
			$paramhash{ARRAYLENGTH} = $1;
		}

		# parse type and name
		my ($type, $name) = $p =~ /(\w[\w\s]*[\s\*]+)(\w+)/;
		# strip leading and trailing whitespace
		$type =~ s/^\s*(.*?)\s*$/$1/;

		$paramhash{NAME} = $name;
		$paramhash{TYPE} = $type;
		# a pointer in the type means the data is passed by reference
		$paramhash{ISREF} = ($type =~ /\*\s*$/);

		if ($paramhash{ISREF}) {
			# a const pointer is assumed to be an input parameter,
			# unless it's a **. Yes, this is a big stretch.
			# TODO: Should we depend on [out] instead?
			$paramhash{INPARAM} = ($type =~ /^\s*const/) && !($type =~ /\*\s*\*/);
			if ($paramhash{INPARAM}) {
				($paramhash{BASETYPE}) = ($type =~ /^\s*const\s*(.*?)\s*\*\s*$/);

				# const char * is a special type: string.
				# if size_is is not specified, use a default size and pass the
				# string.
				if (($paramhash{BASETYPE} eq "char") && !$paramhash{ARRAYLENGTH}) {
					$paramhash{ARRAYLENGTH} = "__maxstringlength";
				}
			}
			else {
				($paramhash{BASETYPE}) = ($type =~ /^\s*(.*?)\s*\*\s*$/);
			}

			# void * is assumed to be a byte array, so convert
			if ($paramhash{BASETYPE} eq "void") {
				$paramhash{BASETYPE} = "unsigned char";
			}
		}
		push @params, \%paramhash;
	}
	$func{PARAMS} = \@params;

	# The only case this is needed is bsettop_uninit (no return code, no parameters)
	# and this is a special exception. But I'm leaving it in for completeness.
	if ($#params == -1 && $func{RETTYPE} eq "void") {
		$func{NOSTRUCT} = 1;
	}

	# Return a reference to the func hash
	return \%func;
}

# this is the main function of the module
# it takes an array of functions as input
# and returns an array of function data structures
sub parse_funcs
{
	my $func;
	my @funcrefs;
	for $func (@_) {
		my $funcref = parse_func $func;

		# The following functions are handled as special cases.
		# Don't autogenerate any code.
		if ($funcref->{FUNCNAME} eq "bsettop_init" ||
			$funcref->{FUNCNAME} eq "bsettop_get_init_settings" ||
			$funcref->{FUNCNAME} eq "bsettop_set_init_settings" ||
			$funcref->{FUNCNAME} eq "bsettop_uninit")
		{
			next;
		}

		push @funcrefs, $funcref;

		#&print_api($funcref);
	}
	@funcrefs;
}

#
# debug printout
#
sub print_api
{
	my $func;
	for $func (@_) {
		my $params = $func->{PARAMS};
		my $param;
		print "Function: $func->{RETTYPE} $func->{FUNCNAME}\n";
		for $param (@$params) {
			print "  Param: $param->{TYPE} $param->{NAME}\n";
			print "    isref:	 $param->{ISREF}\n";
			print "    inparam:  $param->{INPARAM}\n";
			print "    basetype: $param->{BASETYPE}\n";
			print "    arraylen: $param->{ARRAYLENGTH}\n";
		}
	}
}

1;
