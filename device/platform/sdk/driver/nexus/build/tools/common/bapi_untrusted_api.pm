#############################################################################
#    (c)2008-2011 Broadcom Corporation
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
# $brcm_Workfile: bapi_untrusted_api.pm $
# $brcm_Revision: 1 $
# $brcm_Date: 8/8/11 5:10p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/tools/common/bapi_untrusted_api.pm $
# 
# 1   8/8/11 5:10p erickson
# SW7420-1689: add untrusted client api enforcement
# 
#############################################################################
use strict;

package bapi_untrusted_api;

sub parse
{
    my $file = shift @_;
    my $FILE;
    my %result;
    my @allow_funcs;
    my @deny_funcs;
    my $allow;
    my $deny;    

    #print "open $file\n";    
    open(FILE, $file) or die "Unable to open file $file\n";
    
    for (<FILE>) {
        chomp $_;
        # remove line comments
        s/\w*\#.*$//;
        # discard blank lines
        next if $_ eq "";
        
        if (/allow:/) {
            $allow = 1;
            undef $deny;
        }
        elsif (/deny:/) {
            undef $allow;
            $deny = 1;
        }
        else {
            if ($allow) {
                push @allow_funcs, $_;
            }
            elsif ($deny) {
                push @deny_funcs, $_;                
            }
        }
    }
    
    $result{'allow_funcs'} = \@allow_funcs;
    $result{'deny_funcs'} = \@deny_funcs;
    return \%result;
}

1;
