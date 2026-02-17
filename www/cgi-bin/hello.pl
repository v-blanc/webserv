use strict;
use warnings;

print "Content-Type: text/html\n\n";

my $query = $ENV{'QUERY_STRING'};

my %params;


foreach my $pair (split /&/, $query)
{
	my ($key, $value) = split /=/, $pair;
	$params{$key} = $value;
}

print "<html><body>";
print "<h1>Hello $params{name}</h1>";
print "</body></html>";
