#!/usr/bin/env perl

use strict;
use File::Basename;
use LWP::UserAgent;
use HTTP::Request::Common qw(GET);

## JSON support
use JSON qw( decode_json );

## YAML support if we want to source YAML directly
# apt-get install libyaml-perl
# use YAML qw(LoadFile);

## grab the url off of a command line or hardcoded string
my $base_test_url = $ARGV[0];
$base_test_url = "http://lgux-pnavarro3.corp.netflix.com:8080/master/nrdptest" unless(length($base_test_url));

my $yaml_testsuite_path = "$base_test_url/functional/gibbon/scripts/testSuites/fullAutorun.yml";
my $yaml_query_path = "$base_test_url/functional/gibbon/scripts/testSuites/query.yaml.testcfg.php";
my $req = GET($yaml_query_path);

## ensure we have a response
my $ua = new LWP::UserAgent;
my $response = $ua->request($req);
$response->code == 200 || die "Unable to fetch auto test: $yaml_query_path";

## store json content
my $json_text = $response->decoded_content;
my $decoded_json = decode_json($json_text);

## iterate through each test item group
foreach my $item( @$decoded_json ) { 
    ## each key is a unique_testname
    my $yamlkey     = (keys %$item)[0];
    my @val_array   = @{$item->{$yamlkey}};

    ## make a short version of the key
    my @first_octet = split /-/, $yamlkey;
    my $yaml_short_key = @first_octet[0];
    
    ## iterate through each entry to build test
    my $group_total = @val_array;
    my $group_counter = 0;
    my @group_test_names;

    for my $test_entry (@val_array) {
        ## variables
        my $test_prefix = "gibbon_" . $yaml_short_key;
        my $previous_test_name = "";
        my $test_name = "";
        my $group = "";
        my $test_label = "auto";

        my $manual_url = "";
        my $manual_args = "";
        
        my $auto_url = "";
        my $auto_args = "--no-console";

        ## get our variables
        my $test_url                = $test_entry->{test_url};
        my $override_boot           = $test_entry->{override_boot};
        my $use_custom_splash       = $test_entry->{use_custom_splash};
        my $use_custom_nrdjs        = $test_entry->{use_custom_nrdjs};
        my $run_to_end              = $test_entry->{run_to_end};
        my $additional_url_params   = $test_entry->{additional_url_params};
        my $additional_cmd_params   = $test_entry->{additional_cmd_params};
        my $gdb_timeout_seconds     = $test_entry->{gdb_timeout_seconds};
        my $run_serial              = $test_entry->{run_serial};

        ## handle if the test needs to run serially
        if ($run_serial eq "") {
            $run_serial = 0;
        } else {
            $run_serial = 1 if($run_to_end ne "false");
        }
        
        ## set the default timeout
        if ($gdb_timeout_seconds eq "") {
            $gdb_timeout_seconds = 1800;
        }

        ## handle additional url params
        my $test_url_params = "";
        if (length $additional_url_params) {
            $additional_url_params = join('&', @$additional_url_params);
        }

        ## produce a unique test guid
        $manual_url = $test_url;
        $manual_url =~ s,^TEST:\$BASE_URL,$base_test_url,;

        if($test_url =~ /^TEST:\$BASE_URL\/functional\/gibbon\/static\/([^\/]*)\/.*$/ ||
           $test_url =~ /^TEST:\$BASE_URL\/certification\/gibbon\/static\/([^\/]*)\/.*$/ ||
           $test_url =~ /^TEST:\$BASE_URL\/functional\/gibbon\/(bugs)\/automated\/.*$/) {
        }

        $auto_url = "$manual_url?ctest&autorun";
        $auto_url .= "&runToEnd" if($run_to_end ne "false");

        ## append additional URL parameters
        if(length $additional_url_params) {
            $manual_url  .= "?$additional_url_params";
            $auto_url    .= "&$additional_url_params";
        }

        ## append additional CMD parameters
        if(length($additional_cmd_params)) {
            my @extra_cmd_params = split / /, $additional_cmd_params;
            foreach(@extra_cmd_params) {
                $manual_args = "$manual_args \"$_\"";
                $auto_args = "$auto_args \"$_\"";
            }
        }

        ## how do we want to launch?
        my $url_launch = "-0";
        if (defined($override_boot) && $override_boot == 0) {
            $url_launch = "-U";
        }

        ## use custom splash?
        if (!defined($use_custom_splash) || (defined($use_custom_splash) && $use_custom_splash == 0)) {
            $auto_args      = "$auto_args --splash-screen=none";
            $manual_args    = "$manual_args --splash-screen=none";
        }

        ## inject latest nrdjs?
        if (!defined($use_custom_nrdjs) || (defined($use_custom_nrdjs) && $use_custom_nrdjs == 0)) {
            $auto_args      = "$auto_args --inject-js http://ppd.builds.test.netflix.net/view/NRDJS/job/PPD-NRDJS-master/lastSuccessfulBuild/artifact/origin/master/nrdjs.min.js";
            $manual_args    = "$manual_args --inject-js http://ppd.builds.test.netflix.net/view/NRDJS/job/PPD-NRDJS-master/lastSuccessfulBuild/artifact/origin/master/nrdjs.min.js";
        }
        
        ## are we running in a group? if so, update the test name
        if($group_total > 1) {
            $group = "--group $yaml_short_key";
            $test_prefix .= "_run_" . (++$group_counter) . "_of_" . $group_total;
        } else {
            $test_prefix .= "_";
        }
        
        ## produce a unique test guid
        $test_name = $test_url;
        $test_name =~ s,^TEST:\$BASE_URL,$test_prefix,;
        $test_name =~ s,[\./-],_,g;
        push @group_test_names, "$test_name";
        if($group_counter > 1) {
            $previous_test_name = @group_test_names[$group_counter-2];
        }

        ## This block also sets the CTest labels
        if($test_url =~ /^TEST:\$BASE_URL\/functional\/gibbon\/static\/([^\/]*)\/.*$/ ||
           $test_url =~ /^TEST:\$BASE_URL\/certification\/gibbon\/static\/([^\/]*)\/.*$/ ||
           $test_url =~ /^TEST:\$BASE_URL\/functional\/gibbon\/(bugs)\/automated\/.*$/) {
            my $magic = $1;
            $test_label = "$test_label;${test_label}_${magic}";
        }

        ## DEFINE TEST
        print "\## $yamlkey:\n";

        ## AUTOMATED TEST
        print "add_test(NAME ${test_name} COMMAND \${GIBBON_TEST_HARNESS} --name ${test_name} --autotest $group \${GIBBON_BINARY} $url_launch $auto_url $auto_args )\n";
        print "set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION \"AutoTestFailure:\")\n";
        print "set_tests_properties(${test_name} PROPERTIES TIMEOUT $gdb_timeout_seconds)\n";
        print "set_tests_properties(${test_name} PROPERTIES DEPENDS \"${previous_test_name}\")\n" unless($previous_test_name eq "");
        print "set_tests_properties(${test_name} PROPERTIES RUN_SERIAL TRUE)\n" if($run_serial);
        print "set_tests_properties(${test_name} PROPERTIES LABELS \"$test_label;${test_name};${yaml_short_key}\")\n";

        ## MANUAL TEST
        print "add_test(NAME ${test_name}_manual COMMAND \${GIBBON_TEST_HARNESS} --name ${test_name}_manual $group \${GIBBON_BINARY} $url_launch $manual_url $manual_args )\n";
        print "set_tests_properties(${test_name}_manual PROPERTIES FAIL_REGULAR_EXPRESSION \"AutoTestFailure:\")\n";
        print "set_tests_properties(${test_name}_manual PROPERTIES TIMEOUT $gdb_timeout_seconds)\n";
        print "set_tests_properties(${test_name}_manual PROPERTIES DEPENDS \"${previous_test_name}_manual\")\n" unless($previous_test_name eq "");
        print "set_tests_properties(${test_name}_manual PROPERTIES RUN_SERIAL TRUE)\n" if($run_serial);
        print "set_tests_properties(${test_name}_manual PROPERTIES LABELS \"${test_name}_manual;${yaml_short_key}_manual\")\n\n";
    }
}