# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

$debug = 0;

$total     = 0;
$totalPass = 0;
$totalFail = 0;
$totalSkip = 0;

sub startHTML
{
    print "<html><head><style>";

    print "tr.header {                      \n";
    print "    background-color: navy;      \n";
    print "    color: whitesmoke;           \n";
    print "    font-weight: bolder;         \n";
    print "}                                \n";
    print "td {                             \n";
    print "    border: 1px solid black;     \n";
    print "    text-align: center;          \n";
    print "}                                \n";
    print "td.area, tr.header td {          \n";
    print "    text-align: left;            \n";
    print "}                                \n";
    print "tr.fail, td.fail {               \n";
    print "    background-color: pink;      \n";
    print "}                                \n";
    print "tr.pass, td.pass {               \n";
    print "    background-color: #CCFFCC;   \n";
    print "}                                \n";
    print "tr.skip, td.skip {               \n";
    print "    background-color: lightgray  \n";
    print "}                                \n";

    print "</style></head><body> \n";
}

sub endHTML
{
    print "</body></html> \n";
}

sub doOneArea
{
    my $area    = shift(@_);  # the first parameter passed in is the test area
    my $oneFile = shift(@_);  # the second parameter passed in is the full path to the file to process

    $passCount = 0;
    $failCount = 0;
    $skipCount = 0;
    $class     = "pass";   # be optomistic

    open(FILE, $oneFile);
    my @wholeFile = <FILE>;
    close(FILE);

    foreach (@wholeFile)    # loop over each line of the file
    {
        if (m/Result\=\"PASS\"/)
        {
            $passCount++;
            $totalPass++;
            $total++;
        }
        if (m/Result\=\"FAIL\"/)
        {
            $failCount++;
            $totalFail++;
            $total++;
        }
        if (m/Result\=\"SKIP\"/)
        {
            $skipCount++;
            $totalSkip++;
            $total++;
        }
    }

    $class = "fail" if ($failCount > 0);
    $class = "skip" if ($passCount == 0) && ($failCount == 0) && ( ($skipCount > 0) || ($skipCount == 0) );
    
    $localTotal = $passCount + $failCount + $skipCount;

    if ( -e $oneFile )
    {
        print "<tr class=\"".$class."\"><td class=\"area\"><a href=\"".$oneFile."\">".$area."</a></td><td>".$localTotal."</td><td>".$passCount."</td><td>".$failCount."</td><td>".$skipCount."</td></tr> \n";
    }
    else
    {
        print "<tr class=\"".$class."\"><td class=\"area\">".$area."</td><td colspan=4> N/A </td></tr> \n";
    }
}


sub processAllTestAreas
{
    my ($areas)  = shift(@_);  # the first parameter passed in is the file listing the test areas
    my ($runDir) = shift(@_);  # the second parameter passed in is the directory where the test results are located

    print "test areas file = ".$areas."\n"   if ($debug==1);
    print "test run dir    = ".$runDir."\n"  if ($debug==1);

    open(FILE, $areas);
    my @wholeFile = <FILE>;
    close(FILE);

    foreach (@wholeFile)    # loop over each line of the file
    {
        next if /^\#/;      # skip comment lines
        next if /^\s+$/;    # skip blank lines

        @splitLine = split();
        $testArea    = $splitLine[0];   # first column
        $testXml     = $splitLine[1];   # second column
        $testXml     =~ s/exe/xml/;     # change the file suffix from .exe to .xml
        $fullXmlPath = "$runDir/$testArea/$testXml";

        print "test area     = ".$testArea."\n"     if ($debug==1);
        print "test xml      = ".$testXml."\n"      if ($debug==1);
        print "full xml path = ".$fullXmlPath."\n"  if ($debug==1);

        doOneArea( $testArea, $fullXmlPath );
    }
}

#main
{
    $pkTestAreasFile = $ARGV[0];
    $pkTestRunDir    = $ARGV[1];

    startHTML();

    print "<h2>Breakdown</h2> \n";
    print "<table border=1> \n";
    print "<tr class=\"header\"><th>Test Area</th><th>Total</th><th>Pass</th><th>Fail</th><th>Skip</th></tr> \n";
    processAllTestAreas( $pkTestAreasFile, $pkTestRunDir );
    print "</table> \n";

    print "<h2>Summary</h2> \n";
    print "<table border=1> \n";
    print "<tr class=\"header\"><th>TOTAL</th><th>Total Pass</th><th>Total Fail</th><th>Total Skip</th></tr> \n";
    print "<tr><td>".$total."</td><td class=\"pass\">".$totalPass."</td><td class=\"fail\">".$totalFail."</td><td class=\"skip\">".$totalSkip."</td></tr> \n";
    print "</table> \n";

    endHTML();
}

