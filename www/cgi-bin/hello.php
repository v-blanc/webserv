#!/usr/bin/php-cgi
<?php

$name   = $_GET['name']               ?? 'World';
$method = $_SERVER['REQUEST_METHOD']  ?? 'GET';
$server = $_SERVER['SERVER_NAME']     ?? 'localhost';
$port   = $_SERVER['SERVER_PORT']     ?? '8080';
$qs     = $_SERVER['QUERY_STRING']    ?? '';

// CGI response: header() handles the headers + blank line
header('Content-Type: text/html');

?><!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>Hello from PHP</title>
<style>
  body  { font-family: monospace; background: #0a0a0f; color: #e8e8f0; padding: 2rem; }
  h1    { color: #8993be; }
  td,th { padding: .3rem .8rem; border: 1px solid #2a2a3d; }
  th    { color: #8993be; }
  .k    { color: #888899; }
</style></head>
<body>
  <h1>Hello, <?= htmlspecialchars($name) ?>!</h1>
  <p style="color:#888899">Served by <strong>hello.php</strong> (PHP <?= PHP_MAJOR_VERSION ?>)</p>
  <table>
    <tr><th>Variable</th><th>Value</th></tr>
    <tr><td class="k">REQUEST_METHOD</td><td><?= htmlspecialchars($method) ?></td></tr>
    <tr><td class="k">SERVER_NAME</td>   <td><?= htmlspecialchars($server) ?></td></tr>
    <tr><td class="k">SERVER_PORT</td>   <td><?= htmlspecialchars($port)   ?></td></tr>
    <tr><td class="k">QUERY_STRING</td>  <td><?= htmlspecialchars($qs)     ?></td></tr>
  </table>
</body></html>
