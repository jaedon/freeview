#!/usr/bin/env perl

use strict;
use File::Basename;
use LWP::UserAgent;
use HTTP::Request::Common qw(GET);
use JSON;

my $stress_url = $ARGV[0];
$stress_url = "http://lgux-pnavarro3.corp.netflix.com:8080/" unless(length($stress_url));

my $req = GET("$stress_url/certification/gibbon/resources/templates.txt");
my $ua = new LWP::UserAgent;
my $response = $ua->request($req);
$response->code == 200 || die "Unable to fetch profiles: $stress_url";
my $data = from_json($response->content);

my %tests;
foreach(@{$data->{templates}}) {
    my $template = $_;
    my $template_name = $template->{id}->{value};
    my $test_label = "stress_${template_name}";
    $test_label =~ s,[-+],_,g;
    my $test_name = "gibbon_${test_label}";
    next if($tests{$test_name});
    $tests{$test_name} = $template_name;
    print "add_test(NAME ${test_name} COMMAND \${GIBBON_TEST_HARNESS} --name ${test_name} \${GIBBON_BINARY} --no-console -J nologcheck -X -U ${stress_url}/nrdptest/13.3/certification/gibbon/index.js?debug=true&profile=${template_name} \${GIBBON_TEST_HARNESS_FLAGS})\n";
    print "set_tests_properties(${test_name} PROPERTIES LABELS \"stress;$test_label\")\n\n";
}
