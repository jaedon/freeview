# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

#
# Abstract:
#
#     This script builds a profile header .h file from
#     a .mk file
#

$PROFILE = @ARGV[0];
$COMPILETIMEOPTIONS = @ARGV[1];
$TEMPLATE = @ARGV[2];
$debug = @ARGV[3];

#
# C:\foo\BarBlah.mk -> __BARBLAHMK__
#
$headerndef = $PROFILE;
$headerndef =~ s/.*[\\\/]//g;
$headerndef =~ s/\.//g;
$headerndef =~ tr/a-z/A-Z/;
$headerndef = "__" . $headerndef . "_H__";

#
# C:\foo\BarBlah.mk -> C:\foo\
#
$profilepath = $PROFILE;
if( $profilepath =~ m/(.*[\\\/])/ )
{
    $profilepath = $1;
}
else
{
    die "Unable to parse profile path \"" . $profilepath . "\""
}

#
# While the list of files to process is not empty
#  Open next file and remove it from the list of files to process
#  For each line in next file,
#  if it's A=B and NOT in an if,
#   add A,B to the hash
#   add any associated comment lines as A,commentlines to the comments hash
#  else if it's !INCLUDE X and NOT in an if
#   add X to the list of files to process
#  else if it's a comment that doesn't start with #* e.g. # This is a comment (but not a copyright comment)
#   add X to the comments corresponding to the next A=B
#  Close the file
#
%varhash = ();
%comments = ();
@profilelist = ();
push @profilelist, $PROFILE;
$previouscomment = "";
while( scalar(@profilelist) > 0 )
{
    $inif = 0;
    $PROFILE = pop @profilelist;

    open( PROFILE ) or die "Cannot open the .mk file: \"" . $PROFILE . "\"";
    while( <PROFILE> )
    {
        if( m/[!]IF/ )
        {
            $inif = 1;
        }
        if( m/[!]ENDIF/ )
        {
            $inif = 0;
        }
        elsif( m/^([^!].*)=(.*)/ )
        {
            if( $inif == 0 )
            {
                $varhash{$1}=$2;
                $comments{$1}=$previouscomment;
                $previouscomment = "";
            }
        }
        elsif( m/[!]INCLUDE \"([^"]*)\"/ )
        {
            if( $inif == 0 )
            {
                push @profilelist, ($profilepath . $1);
            }
        }
        elsif( m/^\#[^*].*/ )
        {
            $previouscomment .= $_;
        }
    }
    close( PROFILE );
}

#
# Output the hash if $debug
#
if( $debug )
{
    foreach $key (sort keys %varhash)
    {
        print "Key: ".$key."=".$varhash{$key}."\n";
        print "Comment: ".$key."=".$comments{$key}."\n";
    }
}

#
# Write the template
#
open( TEMPLATE ) or die "Cannot open $TEMPLATE";
while( <TEMPLATE> )
{
    print $_;
}
close( TEMPLATE );

#
# Write the #ifndef start
#
print "#ifndef " . $headerndef . "\n";
print "#define " . $headerndef . "\n";
print "\n";

#
# Loop over each line in compile time options
#
open( COMPILETIMEOPTIONS ) or die "Cannot open the .mk file: @ARGV[1]";
while( <COMPILETIMEOPTIONS> )
{
    if( m/^_DRM_C_DEFINES.*/ )
    {

# Strip _DRM_C_DEFINES=$(_DRM_C_DEFINES) 

        $nextvar = $_;

# Remove cr/lf (\r\n)
        $nextvar =~ s/[\x0A\x0D]//g;

        $nextvar =~ s/_DRM_C_DEFINES.*_DRM_C_DEFINES *\) *//;

# Save off var in =$(var)
        $submatch = $nextvar;
        $submatch =~ m/[^\$]*\$\((.*)\) */;
        $submatch = $1;

        if( $debug )
        {
            print "Submatch: ".$submatch."\n\n";
        }

# Strip =$(var)
        $nextvar =~ s/\= *\$\(.*\)//;

# Strip whitespace
        $nextvar =~ s/ *//g;
        $nextvar =~ s/\n//;

# Find out if this variable has any associated comments
        $comment = "";
        if( exists $comments{$nextvar."_VIA_PROFILE"} )
        {
            $comment = $comments{$nextvar."_VIA_PROFILE"};
        }
        elsif( exists $comments{$submatch."_VIA_PROFILE"} )
        {
            $comment = $comments{$submatch."_VIA_PROFILE"};
        }

# If there are comments
        if( $comment =~ m/.+/ )
        {
# Strip the '#' character from the comments
            $comment =~ s/\#//g;

# Start C-style comment
            print "/********************************************************************************\n**\n";

            $commentline = $comment;
            pos($commentline) = 0;

# While there are still lines in the comment...
            while( $commentline =~ /([\x0A\x0D]*[^\x0A\x0D]*[\x0A\x0D]*)/g )
            {
                my $printedcommentline = $1;

# Remove cr/lf (\r\n)
# Remove whitespace from beginning and end
                $printedcommentline =~ s/[\x0A\x0D]//g;
                $printedcommentline =~ m/ *(.*) */;
                $printedcommentline = $1;

# If there is actual data in the comments, add it to the output
                if( $printedcommentline =~ m/.+/ )
                {
                    print "** " . $printedcommentline . "\n";
                }
# Continue where we left off
                $commentline =~ /\G/;
            }
# End C-style comment
            print "**\n********************************************************************************/\n";
        }

        print "#ifndef " . $nextvar . "\n";
        if( exists $varhash{$nextvar."_VIA_PROFILE"} )
        {
            print "#define " . $nextvar . " " . $varhash{$nextvar."_VIA_PROFILE"} . "\n";
        }
        elsif( exists $varhash{$submatch."_VIA_PROFILE"} )
        {
            print "#define " . $nextvar . " " . $varhash{$submatch."_VIA_PROFILE"} . "\n";
        }
        else
        {
            die "Unknown " . $nextvar;
        }
        print "#endif /* " . $nextvar . " */\n\n";
    }
}
close( COMPILETIMEOPTIONS );

#
# Write the #endif end
#
print "\n";
print "#endif /* " . $headerndef . " */\n\n";

