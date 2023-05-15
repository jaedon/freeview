#!/usr/bin/perl -w
#     (c)2007-2010 Broadcom Corporation
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
# $brcm_Workfile: generate_chp_pwr.pl $
# $brcm_Revision: Hydra_Software_Devel/12 $
# $brcm_Date: 7/20/12 5:15p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /magnum/basemodules/chp/src/common/generate_chp_pwr.pl $
# 
# Hydra_Software_Devel/12   7/20/12 5:15p jtna
# SW7405-4433: generate output files in appropriate locations, as per new
# chp directory scheme
# 
# Hydra_Software_Devel/11   10/26/10 4:53p jtna
# SW7420-972: add check for unacquirable HW nodes
# 
# Hydra_Software_Devel/10   10/20/10 6:45p jtna
# SW7420-972: rewrite to support HW_ -> HW_ dependency
# 
# Hydra_Software_Devel/9   9/1/10 3:35p jtna
# SW7420-972: remove unneeded #include
# 
# Hydra_Software_Devel/8   8/19/10 2:25p jtna
# SW7405-4433: include bchp_priv.h
# 
# Hydra_Software_Devel/7   8/18/10 5:29p jtna
# SW7405-4433: comment out extra debug
# 
# Hydra_Software_Devel/6   8/13/10 6:37p jtna
# SW7405-4433: reword warnings
# 
# Hydra_Software_Devel/5   8/13/10 5:21p jtna
# SW7405-4433: added many->one syntax parsing. changed node
# representation from pointer to #define uint32_t. other related
# changes.
# 
# Hydra_Software_Devel/4   6/9/10 3:34p jtna
# SW7405-4433: add breg handle to bchp handle and don't require a breg
# handle to be passed to the pwr functions
# 
# Hydra_Software_Devel/3   6/9/10 12:12p jtna
# SW7405-4433: added debug string to improve dump function
# 
# Hydra_Software_Devel/2   6/8/10 6:55p jtna
# SW7405-4433: combined Node and Leaf enum lists into one. clarified
# resource type enum naming.
# 
# Hydra_Software_Devel/1   6/1/10 7:32p jtna
# SW7405-4433: added bchp_pwr files
# 
#############################################################################

use strict;

sub insert_in_graph {
    my $graph = shift;
    my $node = shift;
    my @d = @_;

    # check for circular dependencies
    foreach my $dep (@d) {
        if (is_successor($graph, $node, $dep)) {
            die "Circular dependency between $node <-> $dep. Unable to proceed\n";
        }
    }

    # check for duplicate/redudant dependencies
    foreach my $new (@d) {
        my $add = 1;
        foreach my $old (@{$graph->{$node}}) {
            if ($new eq $old) {
                $add = 0;
                die "Duplicate/redundant dependency: $node -> $new. Unable to proceed\n";
                #print "DEBUG: Duplicate/redundant dependency $node -> $new\n";
            }
        }

        if ($add) {
            # push the additional dependency on the array
            push @{$graph->{$node}}, $new;
        }
    }
  
    return;
}

# returns 1 if $child is a successor of $parent in the directed graph
# a node is considered a successor of itself
sub is_successor {
    my $graph = shift;
    my $child = shift;
    my $parent = shift;
    my $rc = 0;

    if ($child eq $parent) {
        return 1;
    }

    if (exists $graph->{$parent}) {
        my @depends = @{$graph->{$parent}};
        foreach my $dep (@depends) {
            $rc |= is_successor($graph, $child, $dep);
        }
    }

    return $rc;  
}

# returns all immediate parents of a node
sub get_parents 
{
    my $graph = shift;
    my $nodename = shift;
    my @parents;
    foreach my $key (keys %$graph) {
        foreach my $dep (@{$graph->{$key}}) {
            if ($nodename eq $dep) {
                push @parents, $key;
            }
        }
    }
    return @parents;
}

# recursively attempt to find a non-HW parent for a HW node. return the first one.
sub find_nonHwParent
{
    my $graph = shift;
    my $nodename = shift;
    my $rc = 0;

    my @parents = get_parents($graph, $nodename);
    if (scalar(@parents)==0) {
        if ($nodename =~ /^HW_/) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        foreach (@parents) {
            $rc |= find_nonHwParent($graph, $_);
        }
        return $rc;
    }
}

# parses a text file and returns three things:
# an array of nodes
# an array of corresponding dependencies
# an array containing all nodes in the same order that they appear in the text file, and without duplicates
sub parse_lines {
    my ($filename, $nodelist, $deplist, $allnodes) = @_;
    my $linenum = 1;
    my %refcnt;
    open FILE, $filename;

    foreach my $line (<FILE>) {
        my ($node, $dep, $valid);
        $valid = 1;

        $line =~ s/\r\n/\n/; # convert DOS to Unix
        
        if ($line =~ /^\s*\n$/ || $line =~ /^\s*#/) {
            # ignore full-line comment
        }
        else {
            $line =~ s/#.*//; # trim trailing comment

            if ($line =~ 
            /^\s*          # leading whitespace
            (\{?)\s*       # $1: {
            (.+?)\s*       # $2: string (nodes)
            (\}?)\s*       # $3: }
            (->){1}\s*     # $4: ->
            (\{?)\s*       # $5: {
            (.+?)\s*       # $6: string (dependencies)
            (\}?)\s*$/x) { # $7: }
                $node = $2;
                $dep = $6;

                if (!$1 && !$3) { # no {} brackets around list of nodes: it must be a single node
                    ($valid = 0) if ($node =~ /\W/);
                }
                elsif ($1 && $3) { # {} brackets around list of nodes: it must be a single node or a list of comma-separated nodes
                    ($valid = 0) if ($node !~ /^(\w+)(\s*,\s*\w+)*$/); # invalid list
                }
                else { # unpaired brackets
                    $valid = 0;
                }

                # repeat the above for list of dependencies
                if (!$5 && !$7) {
                    ($valid = 0) if ($dep =~ /\W/);
                }
                elsif ($5 && $7) {
                    ($valid = 0) if ($dep !~ /^(\w+)(\s*,\s*\w+)*$/);
                }
                else {
                    $valid = 0;
                }

                # (one -> many) or (many -> one) is allowed. (many -> many) is not allowed
                if ($node =~ /\W/ && $dep =~ /\W/) {
                    $valid = 0;
                }
            }

            if ($valid) {
                #print "DEBUG: $linenum: $node -> $dep\n";
                push @$nodelist, $node;
                push @$deplist, $dep;

                my @nodes = split /\s*,\s*/, $node;
                my @deps = split /\s*,\s*/, $dep;
                foreach (@nodes, @deps) {
                    if (exists $refcnt{$_}) {
                        $refcnt{$_}++;
                    }
                    else {
                        $refcnt{$_} = 1;
                        push @$allnodes, $_;
                    }
                }
            }
            else {
                chomp($line);
                die "Invalid syntax at line $linenum: \"$line\"\n", 
                "Correct syntax is \"NODE -> DEP\" or \"NODE -> {DEP0, DEP1 ...}\" or ",
                "\"{NODE0, NODE1 ...} -> DEP\". Unable to proceed\n";
                #print "DEBUG: Invalid syntax at line $linenum: \"$line\"\n";
            }

        }
        $linenum++;
    }

    if (scalar(@$nodelist) != scalar(@$deplist)) {
        die "Unrecoverable parsing error. Unable to proceed\n"; # this should never happen
    }
    close FILE;
}

sub create_graph {
    my ($nodelist, $deplist) = @_;
    my %graph; # hash that represents the directed graph

    foreach my $n (@$nodelist) {
        my @nodes = split /\s*,\s*/, $n;
        my $d = shift @$deplist;
        foreach (@nodes) {
            my @deps = split /\s*,\s*/, $d;
            #print "DEBUG: insert_in_graph: ($_) -> (", join(", ",@deps), ")\n";

            # our directed graph is represented as a hash, where the hash key is the string of the node
            # and the hash value is an array of strings. this means that (one->many) relationships can be 
            # represented naturally, but (many -> one) relationships must be converted into many (one -> one) 
            # relationships
            insert_in_graph(\%graph, $_, @deps);
        }
    }

    return (%graph);
}

sub verify_graph {
    my ($graph, $nodes) = @_;

    # check for HW nodes that have dependencies
    foreach (keys %$graph) {
        if (/^HW_/ && (exists $graph->{$_})) { 
            foreach my $dep (@{$graph->{$_}}) {
                # HW nodes can only have other HW nodes as dependencies
                if ($dep !~ /^HW_/) {
                    die "HW node \"$_\" has non-HW dependency \"$dep\". Unable to proceed\n";
                }
                # a HW_ -> HW_ dependency must have a non-HW parent. otherwise neither HW_ node is acquirable
                else {
                    if (find_nonHwParent($graph, $_)==0) {
                        print "*** Warning: $_ has no non-HW parent node and is not acquirable\n";
                    }
                }
            }
        }
    }

    # check for leaf nodes that are not prefixed with "HW_"
    foreach (@$nodes) {
        # a non-HW leaf node serves no purpose; print a warning
        if ($_ !~ /^HW_/ && (!exists $graph->{$_})) {
            print "*** Warning: Found Non-HW leaf node \"$_\". All leaf nodes should be HW nodes\n";
        }
    }
}

sub print_graph {
    my $graph = shift;
    print "--- current graph ---\n";
    foreach (keys %$graph) {
        my @deps = @{$graph->{$_}};
        my $str = join ", ", @deps;
        print "$_ -> $str\n";
    }
    print "\n";
}

sub print_nodes {
    my $nodes = shift;
    print "\n--- sequential list of all nodes (no duplicates) ---\n";
    print (join ", ", @$nodes);
    print "\n\n";
}

sub generate_enum_string
{
    my $prefix = shift;
    my $elements = shift;
    my $num = 0;
    my @lines;

    push @lines, "typedef enum $prefix {\n";
    foreach (@$elements) {
        if ($num++ == 0) {
            push @lines, "    ${prefix}_e$_ = 0,\n";
        }
        else {
            push @lines, "    ${prefix}_e$_,\n";
        }
    }
    push @lines, "    ${prefix}_eMax\n";
    push @lines, "} ${prefix};\n";

    return @lines;
}

sub generate_public_headerfile
{
    my $filename = shift;
    my $nodes = shift;

    chmod(0777, $filename);
    open(OUTFILE, ">$filename") or die "Unable to open output file $filename";

    print OUTFILE "/***************************************************************\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file is auto-generated by generate_chp_pwr.pl, based on\n";
    print OUTFILE "* bchp_pwr_resources.txt.\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file contains a list of power resource IDs that can be\n";
    print OUTFILE "* acquired and released.\n";
    print OUTFILE "*\n";
    print OUTFILE "***************************************************************/\n";
    print OUTFILE "\n";
    print OUTFILE "#ifndef BCHP_PWR_RESOURCES_H__\n";
    print OUTFILE "#define BCHP_PWR_RESOURCES_H__\n";
    print OUTFILE "\n";

    # get the length of the longest node string
    my $maxlen = 0;
    foreach (@$nodes) {
        if ($_ !~ /^HW_/) {
            if (length > $maxlen) {
                $maxlen = length;
            }
        }
    }
    $maxlen += length("BCHP_PWR_RESOURCE_")+3;

    my $idx = 1;
    foreach (@$nodes) {
        if ($_ !~ /^HW_/) {
            printf OUTFILE ("#define %-${maxlen}s 0x%08x\n", "BCHP_PWR_RESOURCE_$_", $idx++);
        }    
    }
    
    print OUTFILE "\n";
    print OUTFILE "#endif\n";
    close OUTFILE;
    
    print "Output: $filename\n";
}

sub generate_priv_headerfile
{
    my $filename = shift;
    my $graph = shift;
    my $nodes = shift;
    my @lines;
    my (@leafs, @nonleafs, @nonleafshw, @HW_all);

    foreach (@$nodes) {
        if (/^HW_/) {
            push @HW_all, $_;
            if (exists $graph->{$_}) {
                push @nonleafshw, $_;
            }
            else {
                push @leafs, $_;
            }
        }
        else {
            push @nonleafs, $_;
        }
    }

    chmod(0777, $filename);
    open(OUTFILE, ">$filename") or die "Unable to open output file $filename";

    print OUTFILE "/***************************************************************\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file is auto-generated by generate_chp_pwr.pl, based on\n";
    print OUTFILE "* bchp_pwr_resources.txt.\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file contains a list of private power resource IDs that\n"; 
    print OUTFILE "* represent HW clocks, and function prototypes for controlling\n";
    print OUTFILE "* them.\n";
    print OUTFILE "*\n";
    print OUTFILE "***************************************************************/\n";
    print OUTFILE "\n";
    print OUTFILE "#ifndef BCHP_PWR_RESOURCES_PRIV_H__\n";
    print OUTFILE "#define BCHP_PWR_RESOURCES_PRIV_H__\n";
    print OUTFILE "\n";
    print OUTFILE "#include \"bchp_pwr.h\"\n";
    print OUTFILE "\n";
    print OUTFILE "/* Private power resource IDs */\n";

    # get the length of the longest node string
    my $maxlen = 0;
    foreach (@HW_all) {
        if (length > $maxlen) {
            $maxlen = length;
        }
    }
    $maxlen += length("BCHP_PWR_")+3;

    my $idx = 1;
    foreach (@HW_all) {
        printf OUTFILE ("#define %-${maxlen}s 0x%08x\n", "BCHP_PWR_$_", 0xff000000+$idx++);
    }

    print OUTFILE "\n";
    print OUTFILE "/* Protoypes */\n";
    foreach (@HW_all) {
        print OUTFILE "void BCHP_PWR_P_$_\_Control(BCHP_Handle handle, bool activate);\n";
    }
    print OUTFILE "\n";
    print OUTFILE "/* This is the link between the public and private interface */\n";
    print OUTFILE "void BCHP_PWR_P_HW_Control(BCHP_Handle handle, const BCHP_PWR_P_Resource *resource, bool activate);\n";
    print OUTFILE "\n";

    print OUTFILE "#define BCHP_PWR_P_NUM_NONLEAFS   ", scalar(@nonleafs), "\n";
    print OUTFILE "#define BCHP_PWR_P_NUM_NONLEAFSHW ", scalar(@nonleafshw), "\n";
    print OUTFILE "#define BCHP_PWR_P_NUM_LEAFS      ", scalar(@leafs), "\n";
    print OUTFILE "#define BCHP_PWR_P_NUM_ALLNODES   ", scalar(@nonleafs)+scalar(@leafs)+scalar(@nonleafshw), "\n";
    print OUTFILE "\n";

    print OUTFILE "#endif\n";
    close OUTFILE;

    print "Output: $filename\n";
}

sub generate_resource_string
{
    my $prefix1 = shift;
    my $prefix2 = shift;
    my $type = shift;
    my $elements = shift;
    my @lines;

    foreach (@$elements) {
        push @lines, "const $prefix1 ${prefix1}_${_}[] = {{\n";
        push @lines, "    ${prefix1}Type_e${type},\n";
        push @lines, "    ${prefix2}_${_},\n";
        push @lines, "    \"$_\"\n";
        push @lines, "}};\n\n";
    }

    return @lines;
}

sub generate_dependency_string
{
    my $prefix1 = shift;
    my $prefix2 = shift;
    my $graph = shift;
    my $nodes = shift;
    my @lines;

    foreach (@$nodes) {
        push @lines, "static const ${prefix1}* $prefix2${_}[] = {\n";
        foreach my $dep (@{$graph->{$_}}) {
            push @lines, "    ${prefix1}_${dep},\n";
        }
        push @lines, "    NULL\n";
        push @lines, "};\n\n";
    }

    return @lines;
}

sub generate_array_string
{
    my $prefix1 = shift;
    my $prefix2 = shift;
    my $prefix3 = shift;
    my $prefix4 = shift;
    my $nodes = shift;
    my @lines;

    push @lines, "const $prefix1 ${prefix2}[${prefix3}] = {\n";
    foreach (@$nodes) {
        if ($_ eq "NULL") {
            push @lines, "    NULL,\n";    
        } else {
            push @lines, "    ${prefix4}$_,\n";
        }
    }
    push @lines, "};\n\n";

    return @lines;
}

sub generate_hwcontrol_function_string
{
    my $prefix1 = shift;
    my $prefix2 = shift;
    my $nodes = shift;
    my @lines;

    push @lines, "void BCHP_PWR_P_HW_Control(BCHP_Handle handle, const BCHP_PWR_P_Resource *resource, bool activate)\n";
    push @lines, "{\n";
    push @lines, "    switch(resource->id) {\n";
    foreach (@$nodes) {
        push @lines, "        case ${prefix1}_${_}:\n";
        push @lines, "            ${prefix2}_${_}_Control(handle, activate);\n";
        push @lines, "            break;\n";
    }
    push @lines, "        default:\n";
    push @lines, "            BDBG_ASSERT(0);\n";
    push @lines, "            break;\n";
    push @lines, "    }\n";
    push @lines, "}\n\n";

    return @lines;
}

sub generate_impl_hwcontrol_function_string
{
    my $prefix = shift;
    my $nodes = shift;
    my @lines;

    foreach (@$nodes) {
        if ($_ !~ /^HW_/) {
            next;
        }
        my $str = substr($_, 3);
        push @lines, "void ${prefix}_${_}_Control(BCHP_Handle handle, bool activate)\n";
        push @lines, "{\n";
        push @lines, "    BDBG_MSG((\"${_}: %s\", activate?\"on\":\"off\"));\n";
        push @lines, "\n";
        push @lines, "#if 0 /* Edit the register read/modify/write below */\n";
        push @lines, "    BREG_AtomicUpdate32(handle->regHandle, BCHP_REGISTERNAME,\n";
        push @lines, "        BCHP_REGISTERNAME_${str}_MASK,\n";
        push @lines, "        activate ? 0 : 0xFFFFFFFFFF);\n";
        push @lines, "#endif\n";
        push @lines, "}\n\n";
    }

    return @lines;
}

sub generate_resources_sourcefile
{
    my $filename = shift;
    my $graph = shift;
    my $nodes = shift;
    my @lines;
    my (@leafs, @nonleafs, @nonleafshw, @HW_all);

    foreach (@$nodes) {
        if (/^HW_/) { # a HW node can either be a leaf or a non-leaf
            push @HW_all, $_;
            if (exists $graph->{$_}) {
                push @nonleafshw, $_;
            }
            else {
                push @leafs, $_;
            }
        }
        else {
            push @nonleafs, $_;
        }
    }

    chmod(0777, $filename);
    open(OUTFILE, ">$filename") or die "Unable to open output file $filename";

    print OUTFILE "/***************************************************************\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file is auto-generated by generate_chp_pwr.pl, based on\n";
    print OUTFILE "* bchp_pwr_resources.txt.\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file contains a coded representation of power resource\n"; 
    print OUTFILE "* nodes and their dependencies.\n";
    print OUTFILE "*\n";
    print OUTFILE "***************************************************************/\n";
    print OUTFILE "\n";
    print OUTFILE "#include \"bchp_pwr.h\"\n";
    print OUTFILE "#include \"bchp_pwr_resources.h\"\n";
    print OUTFILE "#include \"bchp_pwr_resources_priv.h\"\n";
    print OUTFILE "\n";
    
    print OUTFILE "/* Resource definitions */\n";
    @lines = generate_resource_string("BCHP_PWR_P_Resource", "BCHP_PWR_RESOURCE", "NonLeaf", \@nonleafs);
    print OUTFILE @lines;

    @lines = generate_resource_string("BCHP_PWR_P_Resource", "BCHP_PWR", "Leaf", \@leafs);
    print OUTFILE @lines;

    # NonLeafHw nodes are tacked-on at the end
    @lines = generate_resource_string("BCHP_PWR_P_Resource", "BCHP_PWR", "NonLeafHw", \@nonleafshw);
    print OUTFILE @lines;

    my @resources;
    push @resources, @nonleafs, @HW_all;
    print OUTFILE "/* List of resources */\n";
    @lines = generate_array_string("BCHP_PWR_P_Resource*", "BCHP_PWR_P_ResourceList", 
        "BCHP_PWR_P_NUM_ALLNODES", "BCHP_PWR_P_Resource_", \@resources);
    print OUTFILE @lines;

    print OUTFILE "/* Coded dependencies */\n";
    @lines = generate_dependency_string("BCHP_PWR_P_Resource", "BCHP_PWR_P_Depend_", $graph, \@nonleafs);
    print OUTFILE @lines;

    # NonLeafHw nodes are tacked-on at the end
    @lines = generate_dependency_string("BCHP_PWR_P_Resource", "BCHP_PWR_P_Depend_", $graph, \@nonleafshw);
    print OUTFILE @lines;

    foreach (@resources) {
        if (!exists $graph->{$_}) {
            $_ = "NULL";
        }
    }
    print OUTFILE "/* List of coded dependencies */\n";
    @lines = generate_array_string("BCHP_PWR_P_Resource**", "BCHP_PWR_P_DependList", 
        "BCHP_PWR_P_NUM_ALLNODES", "BCHP_PWR_P_Depend_", \@resources);
    print OUTFILE @lines;

    @lines = generate_hwcontrol_function_string("BCHP_PWR", "BCHP_PWR_P", \@HW_all);
    print OUTFILE @lines;
    close OUTFILE;
    
    print "Output: $filename\n";
}

sub generate_impl_sourcefile
{
    my $filename = shift;
    my $nodes = shift;
    my @lines;

    chmod(0777, $filename);
    open(OUTFILE, ">$filename") or die "Unable to open output file $filename";

    print OUTFILE "/***************************************************************\n";
    print OUTFILE "*\n";
    print OUTFILE "* This file maps the power resource control to register writes.\n"; 
    print OUTFILE "* The templates are auto-generated by generate_chp_pwr.pl,\n";
    print OUTFILE "* but must be filled-in manually.\n";
    print OUTFILE "*\n";
    print OUTFILE "***************************************************************/\n";
    print OUTFILE "\n";
    print OUTFILE "#include \"bchp.h\"\n";
    print OUTFILE "#include \"bchp_priv.h\"\n";
    print OUTFILE "#include \"bdbg.h\"\n";
    print OUTFILE "#include \"bkni.h\"\n";
    print OUTFILE "\n";
    print OUTFILE "BDBG_MODULE(BCHP_PWR_IMPL);\n";
    print OUTFILE "\n";

    @lines = generate_impl_hwcontrol_function_string("BCHP_PWR_P", $nodes);
    print OUTFILE @lines;
    close OUTFILE;
    
    print "Output: $filename\n";
}


################
# main
################

my (@nodelist, @deplist, @allnodes);
my %graph;
my ($platform, $bchp_ver, $targetdir_src, $targetdir_inc);

if ($#ARGV == -1) {
    print "Usage: generate_chp_pwr.pl file.txt\n";
    exit 1;
}

unless (-e $ARGV[0]) {
    print "### Error: $ARGV[0] does not exist\n";
    exit 1;
}


use Cwd 'abs_path';
my $abs_dir = substr(abs_path($ARGV[0]), 0, rindex(abs_path($ARGV[0]), "/")+1);
my $found_inc_dir = 0;

if ($abs_dir =~ m#(.+/)src/([1-9]\d{3,}/pwr/[a-z]\d)/#) { # if location of txt file contains hints about platform and chip_rev...
    $targetdir_inc = $1 . "include/" . $2;

    use Cwd;
    my $dir = getcwd;
    my $back_count = 0;

    # ... then try to get directory of public header file, relative to current working directory
    while ( (my $idx = rindex($dir, "/", length($dir)-2)) != -1) { # rindex is 0-based, while length is 1-based
        $back_count++;
        $dir = substr($dir, 0, $idx+1);

        if ($targetdir_inc =~ m/^$dir(.+)/) {
            $targetdir_inc = "./" . ("../" x$back_count) . $1 . "/";

            if (-d $targetdir_inc) {
                $found_inc_dir = 1;
            }
            last;
        }
    }
}

$targetdir_src = "./";
if ((my $idx = rindex($ARGV[0], "/")) != -1) {
    $targetdir_src = substr($ARGV[0], 0, $idx+1);
}
if ($found_inc_dir == 0) {
    $targetdir_inc = $targetdir_src; # fall-back option
}

# parse the textfile and generate a graph structure (represented as a hash)
# invalid syntax or circular dependencies will result in a die
parse_lines($ARGV[0], \@nodelist, \@deplist, \@allnodes);
%graph = create_graph(\@nodelist, \@deplist);

# verify the generated graph
verify_graph(\%graph, \@allnodes);

# $allnodes is a list of all nodes from the textfile. They are in the same 
# order as they appear in the textfile, and without duplicates.
# This is useful in keeping track of the order that the user specified, 
# because our directed graph (a hash) maintains no such order.

#print_nodes(\@allnodes);
#print_graph(\%graph);

# generate the 4 files
generate_public_headerfile($targetdir_inc . "bchp_pwr_resources.h", \@allnodes);
generate_priv_headerfile($targetdir_src . "bchp_pwr_resources_priv.h", \%graph, \@allnodes);
generate_resources_sourcefile($targetdir_src . "bchp_pwr_resources.c", \%graph, \@allnodes);
generate_impl_sourcefile($targetdir_src . "bchp_pwr_impl.c", \@allnodes);

0;
