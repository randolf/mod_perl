# please insert nothing before this line: -*- mode: cperl; cperl-indent-level: 4; cperl-continued-statement-offset: 4; indent-tabs-mode: nil -*-
use strict;
use warnings FATAL => 'all';

# The Cookie HTTP header can be accessed via $r->headers_in and in certain
# situations via $ENV{HTTP_COOKIE}.
#
# 'SetHandler perl-script', combined with 'PerlOptions -SetupEnv', or
# 'SetHandler modperl' do not populate %ENV with CGI variables.  So in
# this test we call $r->subprocess_env, which adds them on demand, and
# we are able to get the cookie via %ENV.
#
# the last sub-test makes sure that mod_cgi env vars don't persist 
# and are properly re-set at the end of each request.
#
# since the test is run against the same interpreter we also test that
# the cookie value doesn't persist if it makes it to %ENV.


use Apache::Test;
use Apache::TestUtil;
use Apache::TestRequest;

use TestCommon::SameInterp;

plan tests => 3, need 'HTML::HeadParser';

my $module   = 'TestModperl::cookie';
my $location = '/' . Apache::TestRequest::module2path($module);

my $cookie = 'foo=bar';
my %cookies = (
     header   => $cookie,
     env      => $cookie,
     nocookie => '',
);

# 'nocookie' must be run last, server-side shouldn't find a cookie
# (testing that %ENV is reset to its original values for vars set by
# $r->subprocess_env, which is run internally for 'perl-script')
# this requires that all the tests are run against the same interpter

my @tests_ordered = qw(header env nocookie);

t_debug "getting the same interp ID for $location";
my $same_interp = Apache::TestRequest::same_interp_tie($location);

my $skip = $same_interp ? 0 : 1;
for my $test (@tests_ordered) {
    my $expected = $test eq 'nocookie' ? '' : "bar";
    my @headers = ();
    push @headers, (Cookie => $cookies{$test}) unless $test eq 'nocookie';

    my $received = same_interp_req_body($same_interp, \&GET,
                                        "$location?$test", @headers);
    $skip++ unless defined $received;
    same_interp_skip_not_found(
        $skip,
        $received,
        $expected,
        "perl-script+SetupEnv/cookie: $test"
    );
}
