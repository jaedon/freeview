#!/usr/bin/perl

#################################
# DME: This perl script processes all header files for the
# latest clearcase check in date/time. It prints the file
# with the latest date/time. Ideally this should be bsettop.h because
# any change to the public API requires that BSETTOP_VERSION be incremented.
#################################

use POSIX;

$dirname = $ENV{"BSEAV"} . "/api/include";
opendir(DIR, $dirname);
@filelist = grep {/\.h$/} readdir(DIR);

# find the highest clearcase modification time
$maxtimeval = 0;
$maxfile="none";

foreach $file (@filelist) {
    open FILE, "$dirname/$file";
    read FILE,$_,1024;
    ($filedate, $filetime) = /brcm_Log: \S+ \$\s \* \s \* \S+\s+(\S+) (\S+)/m;
    $_ = "$filedate,$filetime";
    if (/(.+)\/(.+)\/(.+),(.+):(.+)(\w)/) {
        ($mm,$dd,$yyyy,$hh,$min,$am) = ($1,$2,$3,$4,$5,$6);
    }
    else {
        next;
    }

    if ($am eq "p") {
        $hh += 12;
    }
#   print "$mm,$dd,$yyyy,$hh,$min,$am\n";
    $timeval = mktime(0,$min,$hh,$dd,$mm-1,$yyyy+2000-1900);
#   print "$file = $_ = $timeval\n";

    if (($file eq "bsettop.h" && $timeval == $maxtimeval) || $timeval > $maxtimeval) {
        $maxtimeval = $timeval;
        $maxfile = $file;
    }
}

# Print the file with the last clearcase modification time
print "$maxfile\n";
