#!/usr/bin/env python3

import os

# Read CGI env vars
name = os.environ.get("QUERY_STRING", "").split("name=")[-1].split("&")[0] or "World"
method = os.environ.get("REQUEST_METHOD", "GET")
server = os.environ.get("SERVER_NAME", "localhost")
port = os.environ.get("SERVER_PORT", "8080")
qs = os.environ.get("QUERY_STRING", "")

# CGI response: headers, blank line, body
print("Content-Type: text/html")
print()

print(
    f"""<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>Hello from Python</title>
<style>
  body  {{ font-family: monospace; background: #0a0a0f; color: #e8e8f0; padding: 2rem; }}
  h1    {{ color: #4b8bbe; }}
  td,th {{ padding: .3rem .8rem; border: 1px solid #2a2a3d; }}
  th    {{ color: #4b8bbe; }}
  .k    {{ color: #888899; }}
</style></head>
<body>
  <h1>Hello, {name}!</h1>
  <p style="color:#888899">Served by <strong>hello.py</strong> (Python 3)</p>
  <table>
    <tr><th>Variable</th><th>Value</th></tr>
    <tr><td class="k">REQUEST_METHOD</td><td>{method}</td></tr>
    <tr><td class="k">SERVER_NAME</td>   <td>{server}</td></tr>
    <tr><td class="k">SERVER_PORT</td>   <td>{port}</td></tr>
    <tr><td class="k">QUERY_STRING</td>  <td>{qs or "(empty)"}</td></tr>
  </table>
</body></html>"""
)
