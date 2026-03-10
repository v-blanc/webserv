#!/usr/bin/perl

my $qs     = $ENV{QUERY_STRING}    // "";
my $method = $ENV{REQUEST_METHOD}  // "GET";
my $server = $ENV{SERVER_NAME}     // "localhost";
my $port   = $ENV{SERVER_PORT}     // "8080";

# Extract name param from query string
my ($name) = ($qs =~ /(?:^|&)name=([^&]*)/);
$name //= "World";
$name =~ s/%20/ /g;

# CGI response: headers, blank line, body
print "Content-Type: text/html\n";
print "\n";

print <<"HTML";
<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>Hello from Perl</title>
<style>
  body  { font-family: monospace; background: #0a0a0f; color: #e8e8f0; padding: 2rem; }
  h1    { color: #00acd7; }
  td,th { padding: .3rem .8rem; border: 1px solid #2a2a3d; }
  th    { color: #00acd7; }
  .k    { color: #888899; }
</style></head>
<body>
  <h1>Hello, $name!</h1>
  <p style="color:#888899">Served by <strong>hello.pl</strong> (Perl 5)</p>
  <table>
    <tr><th>Variable</th><th>Value</th></tr>
    <tr><td class="k">REQUEST_METHOD</td><td>$method</td></tr>
    <tr><td class="k">SERVER_NAME</td>   <td>$server</td></tr>
    <tr><td class="k">SERVER_PORT</td>   <td>$port</td></tr>
    <tr><td class="k">QUERY_STRING</td>  <td>$qs</td></tr>
  </table>
</body></html>
HTML
