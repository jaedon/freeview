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
# $brcm_Workfile: bapi_parse_c.pm $
# $brcm_Revision: 19 $
# $brcm_Date: 8/22/12 1:24p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/common/bapi_parse_c.pm $
# 
# 19   8/22/12 1:24p erickson
# SW7231-958: use .preload files to get struct definitions from other
#  modules
# 
# 18   5/16/12 4:59p erickson
# SW7405-5548: parse named substructs for attributes, pointer conversion,
#  callbacks, etc.
# 
# 17   4/17/12 10:03a erickson
# SW7425-1988: allow space before ; in struct members
# 
# 16   4/6/12 10:18a erickson
# SW7420-1148: change ERROR to WARNING for coding convention
# 
# 15   11/4/11 8:54p bandrews
# SW7420-1123: whitespace stripping op must come after function attribute
#  check as it replaces positionals
# 
# 14   11/4/11 5:14p erickson
# SW7420-1123: strip whitespace to correctly parse void foo( void )
# 
# 13   7/28/11 3:08p erickson
# SW7425-1029: set ISHANDLE and RETTYPE_ISHANDLE for NEXUS_VideoInput,
#  etc.
# 
# 12   3/1/11 9:32a erickson
# SW7420-1123: add naming convention test
#
# 11   2/16/11 4:00p erickson
# SW7420-1507: fix callback registration in Open functions
#
# 10   2/15/11 2:24p erickson
# SW7420-1123: set INPARAM correctly for all PARAMS, clarify that [out]
#  is not parsed
#
# 9   11/4/09 1:07p erickson
# SW7405-3340: rework regex for parsing functions to allow whitespace
#  between function name and param list
#
# 7   3/13/09 2:07p erickson
# PR53223: trim multiple spaces between struct member types and names
#
# 6   1/31/08 5:02p vsilyaev
# PR 38682: Added function level attributes
#
# 5   1/31/08 12:45p vsilyaev
# PR 38682: Build proxy for the core module
#
# 4   1/29/08 5:55p vsilyaev
# PR 38682: Support of callbacks in the linux kernel/user proxy
#  environment
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
# Nexus_Devel/2   1/11/08 4:27p vsilyaev
# PR 38682: Adding kernel support
#
# Nexus_Devel/1   9/27/07 1:33p erickson
# PR35395: initial impl
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

sub get_file_text
{
    my $filename = shift;
    open FILE, $filename;
    my $text = join '',<FILE>;

    # pack preprocessor line continutation
    $text =~ s/\\\s+(\w)/$1/sg;
    # remove preprocessor stuff that can cause parsing problems
    $text =~ s/#\w+.*?\n//g;

    # Grab all attr{} hints and move them out of comment
    # Blow away the comment because it isn't going to stay anyway.
    $text =~ s/
        \/\*            # start of a comment
        [^\/]*           # inside comment
        (attr{[^}]+})   # keep attr{..}
        .*?             # rest of stuff
        \*\/            # end of the comment
        /$1/sgx;

    # Remove all comments
    # No nested comments allowed
    $text =~ s/\/\*.*?\*\///sg;

    return $text;
}

sub get_func_prototypes
{
    my $filename = shift;
    my $text = get_file_text $filename;

    # If we have any ", attr", reverse the order. Much easier to parse later.
    $text =~ s/(,)\s*(attr{[^}]+})/ $2$1/sg;

    # Parse functions
    my @funcs = $text =~ /(
        \w[\w\s]+   # return type
        [\s\*]      # whitespace between return type & function name, or * for pointer
        \w+         # function name
        \s*         # possible whitespace between function name and param list. this violates coding convention, but should not break the function parsing.
        \([^\)]*?\)) # parameters. excluding ) avoids detecting "typedef int (*function)(param);" as return type of "typedef" and function name of "int".
        \s*;        # must terminate with ;
        /xsg;

    @funcs;
}

sub checksum
{
    my $sum = 0;
    my $str;
    for $str (@_) {
        $sum += unpack("%16C*", $str);
    }
    return $sum;
}


# parse_attr
#
# parse attributes in the comma separated name=value pairs
# store result in the hash
sub parse_attr
{
    my $attr = shift;
    my %attrs;


#   print "ATTR -> $attr\n";
   for(split(';',$attr)) {
        if (/(\w+)\s*=\s*(\S+)/) {
             $attrs{$1}=$2;
#            print "attr '$1' -> '$2'\n";
        }
   }
   return \%attrs;
}

sub print_attr
{
    my $attr = shift;
    my ($name, $value);
    if(defined $attr) {
        print "attr{";
        while (($name, $value) = each %$attr) {
            print " $name = $value  ";
        }
        print "}\n";
    }
}

sub version_attr
{
    my $attr = shift;
    my ($name, $value);
    my $sum = 0;
    if(defined $attr) {
        while (($name, $value) = each %$attr) {
            $sum += checksum $name, $value;
        }
    }
    $sum;
}

sub print_struct
{
    my $struct = shift;
    my $name;
    my $fields;
    while (($name, $fields) = each %$struct) {
        my $field;
        print "structure $name: \n";
        for $field (@$fields) {
            print "  field: $field->{TYPE} $field->{NAME}\n";
            print_attr $field->{ATTR};
        }
    }
}


sub parse_struct
{
    my $filename = shift;
    my $preload_struct = shift;
    my $text = get_file_text $filename;
    my %struct;

    if (defined $preload_struct) {
        %struct = %$preload_struct;
    }
    
    while ($text =~  /
        typedef\s+struct\s+(\w+)\s*{  # typedef
        (.+?)}                 # body
        \s*\1                   # termination
        \s*;                      # must terminate with ;
        /xgs) {
        my @params; # this is a list of a param hash
        my @level; # fields on the current level
        my $name= $1;
        my $body = $2;
        #print "structure $1\n";
        $struct{$name} = \@params;
        while($body =~ /(struct\s+{)|(union\s+{)|(}[^;]+;)|((\w[\w\s]*[\s\*]+)(\w+))\s*;\s*(attr{([^}]+)})?/sg) {
            
            # interpretation of regex:
            # $1 = "struct {"  ==> the beginning of a substruct
            # $2 = "union {"   ==> the beginning of a subunion
            # $3 = "} name;"   ==> close of a sub-struct/union
            # $4 = "MemberType member"  ==> struct member, type and name
            # $5 = "MemberType"         ==> struct member, type
            # $6 = "member"             ==> struct member, name
            # $7 = "attr{name=value}"   ==> member's whole attr string
            # $7 = "name=value"         ==> member's inner attr string
                
            #print "'$1' '$2' '$3' '$4' '$5' '$6' '$7' '$8'\n";
            if(defined $5) {
                my $type = $5;
                my $name = $6;
                my $attr = $8;
                $type =~ s/\s+$//s;
                # if $type is a struct, then nest it
                if (defined $struct{$type}) {
                    my $substruct = $struct{$type};
                    my $field;
                    for $field (@$substruct) {
                        #print "  substruct field: $field->{TYPE} $name.$field->{NAME}\n";
                        my %paramhash = %$field; # copy hash
                        $paramhash{NAME} = "$name.$paramhash{NAME}";
                        push @level, \%paramhash;
                    }
                }
                else {
                    my %paramhash;
                    if(defined $attr) {
                        $paramhash{ATTR} = parse_attr $attr;
                    }
                    $paramhash{NAME} = $name;
                    $paramhash{TYPE} = $type;
                    push @level, \%paramhash;
                }
            } elsif (defined $1 || defined $2) {
                push @params, @level;
                @level = ();
            } elsif (defined $3) {
                my $fields = $3;
                while($fields =~ /(\w+)/sg) {
                    # append structure name to each field (note, single level only. use named substructs for multilevel.)
                    my $name = $1;
                    my $field;
                    for $field (@level) {
                        my %paramhash = %$field; # copy hash
                        $paramhash{NAME} = "$name.$paramhash{NAME}";
                        push @params, \%paramhash;
                    }
                }
                @level = ();
            }
        }
        push @params, @level;
    }

#    print_struct \%struct;

    return \%struct;
}

sub version_struct
{
    my $struct = shift;
    my $sum = 0;
    my $name;
    my $fields;
    my $field;

    while (($name, $fields) = each %$struct) {
        $sum += unpack("%32C*", $name);
        for $field (@$fields) {
            $sum += checksum $field->{TYPE} , $field->{NAME};
            $sum += version_attr $field->{ATTR};
        }
    }
    return $sum;
}



# parse_func
#
# The function data structure is a hash with the following members:
# PROTOTYPE = string, the original prototype
# RETTYPE = string, the return type
# FUNCNAME = string, the function name
# PARAMS = reference to array of hash references
#   Each hash reference is a single parameter with the following members:
#     NAME
#     TYPE
#
# This subroutine returns a reference to the function hash.
#
sub parse_func
{
    my $prototype = shift;
    my %func;
    my $more;
    my @params; # this is a list of a param hash

    # comment out the attr hint int the actual prototype
    my $actual_prototype = $prototype;
    $actual_prototype =~ s/(attr{.+?})/\/* $1 *\//sg;
    $func{PROTOTYPE} = $actual_prototype;
#    print "'$actual_prototype'\n";


    ($func{RETTYPE}, $more) = $prototype =~ /(.*?)\s*([\s*\*])\w+\s*\(/;
    if ($more eq "*") {
        $func{RETTYPE} .= $more;
    }
    $func{RETTYPE_ISHANDLE} = ($func{RETTYPE} =~ /Handle$/ ||
                               $func{RETTYPE} =~ /NEXUS_(Video|Audio)(Input|Output)/
                               );
    ($func{FUNCNAME}) = $prototype =~ /(\w+)\s*\(/;

    if (!($func{FUNCNAME} =~ /^NEXUS_\w+$/)) {
        print "WARNING: $func{FUNCNAME} does not follow coding convention\n";
    }

    # get the attributes and params into a raw list
    $prototype =~ /\(\s*(attr{(.+?)})?(.*?)\)$/s;
    # function attribute check
    if(defined $2) {
        $func{ATTR} = parse_attr $2;
    }
    my $params=$3;
    # strip whitespace
    $params =~ s/^\s*(.*?)\s*$/$1/;
    if ($params eq "void") {
        $params = undef;
    }
    my @rawparams = split /,/, $params;
    my $p;

    for $p (@rawparams) {
        my %paramhash;

        # See if we have a attr hint and grab it now
        # This also removes that hint from the variable
        if ($p =~ s/attr{(.+?)}//) {
#            print "$func{FUNCNAME} attr = $1\n";
            $paramhash{ATTR} = parse_attr $1;
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
            # note that "const void **" is parsed as a non-const pointer to "const void *", so it's out
            # nexus does not parse [out]
            $paramhash{INPARAM} = ($type =~ /^\s*const/) && !($type =~ /\*\s*\*/);
            if ($paramhash{INPARAM}) {
                ($paramhash{BASETYPE}) = ($type =~ /^\s*const\s*(.*?)\s*\*\s*$/);
            }
            else {
                ($paramhash{BASETYPE}) = ($type =~ /^\s*(.*?)\s*\*\s*$/);

                # non-const void* params w/ attr{nelem_out} are out params, otherwise they are actually in params
                if ($paramhash{BASETYPE} eq 'void' && !defined $paramhash{ATTR}->{'nelem_out'}) {
                    $paramhash{INPARAM} = 1;
                }
            }
        }
        else {
            # if not by-reference, then by-value, which is always in in param
            $paramhash{INPARAM} = 1;
            $paramhash{ISHANDLE} = ($paramhash{TYPE} =~ /Handle$/ ||
                                    $paramhash{TYPE} =~ /NEXUS_(Video|Audio)(Input|Output)/
                                    );
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

        push @funcrefs, $funcref;

#       print_api($funcref);
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
        print_attr $func->{ATTR};
        for $param (@$params) {
            print "  Param: $param->{TYPE} $param->{NAME}\n";
            print "    isref:    $param->{ISREF}\n";
            print "    inparam:  $param->{INPARAM}\n";
            print "    basetype: $param->{BASETYPE}\n";
            print_attr $param->{ATTR};
        }
    }
}

sub version_api
{
    my $sum = 0;
    my $func;
    for $func (@_) {
        my $params = $func->{PARAMS};
        my $param;

        $sum += checksum $func->{RETTYPE}, $func->{FUNCNAME};
        for $param (@$params) {
            $sum += checksum $param->{TYPE}, $param->{NAME}, $param->{ISREF}, $param->{INPARAM}, $param->{BASETYPE};
            $sum += version_attr $param->{ATTR};
        }
    }
    return $sum;
}

1;
