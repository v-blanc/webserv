#!/usr/bin/env python3
"""
cgi-bin/form.py
Handles POST bodies from stdin (url-encoded, json, multipart, raw).
WebServ must:
  - Set CONTENT_LENGTH correctly
  - Pipe the POST body to stdin
  - Set CONTENT_TYPE header
"""

import os
import sys
import json
import datetime

method = os.environ.get("REQUEST_METHOD", "GET")
content_type = os.environ.get("CONTENT_TYPE", "")
content_len = os.environ.get("CONTENT_LENGTH", "0")
query_string = os.environ.get("QUERY_STRING", "")


def parse_urlencoded(raw):
    """Parse application/x-www-form-urlencoded manually."""
    result = {}
    for pair in raw.split("&"):
        if "=" in pair:
            k, v = pair.split("=", 1)
            result[k] = v.replace("+", " ").replace("%20", " ").replace("%40", "@")
    return result


def read_stdin():
    try:
        length = int(content_len)
        if length > 0:
            return sys.stdin.buffer.read(length).decode("utf-8", errors="replace")
    except Exception:
        pass
    return sys.stdin.read()


# --- Read body ---
body_raw = ""
fields = {}

if method == "POST":
    body_raw = read_stdin()
    if "application/json" in content_type:
        try:
            fields = json.loads(body_raw)
        except Exception:
            fields = {"parse_error": "invalid JSON", "raw": body_raw}
    elif "application/x-www-form-urlencoded" in content_type:
        fields = parse_urlencoded(body_raw)
    elif "multipart/form-data" in content_type:
        fields = {"note": "multipart detected", "raw_preview": body_raw[:200]}
    else:
        fields = {"raw_body": body_raw}
elif method == "GET" and query_string:
    fields = parse_urlencoded(query_string)

now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# --- Output ---
print("Content-Type: text/html; charset=utf-8")
print("X-CGI-Script: form.py")
print()

rows = ""
for k, v in fields.items():
    rows += f"<tr><td class='k'>{k}</td><td class='v'>{v}</td></tr>\n"

if not rows:
    rows = "<tr><td colspan='2' style='color:#444'>No fields parsed</td></tr>"

print(
    f"""<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8"><title>Form CGI</title>
  <style>
    body {{ font-family: monospace; background: #0a0a0f; color: #e8e8f0; padding: 2rem; }}
    h2   {{ color: #7c6bff; }}
    h3   {{ color: #ffd166; margin-top:1.5rem; }}
    table{{ border-collapse:collapse; width:100%; margin-top:.5rem; }}
    td, th {{ padding:.4rem .8rem; border:1px solid #2a2a3d; }}
    th {{ color:#00ff9d; }}
    .k {{ color:#888899; }}
    .v {{ color:#00ff9d; }}
    pre {{ background:#111118; padding:1rem; border:1px solid #2a2a3d; color:#aaa; overflow-x:auto; }}
  </style>
</head>
<body>
  <h2>form.py — POST/GET Field Parser</h2>

  <h3>Request Info</h3>
  <table>
    <tr><th>Variable</th><th>Value</th></tr>
    <tr><td class="k">REQUEST_METHOD</td><td class="v">{method}</td></tr>
    <tr><td class="k">CONTENT_TYPE</td><td class="v">{content_type or "(none)"}</td></tr>
    <tr><td class="k">CONTENT_LENGTH</td><td class="v">{content_len}</td></tr>
    <tr><td class="k">QUERY_STRING</td><td class="v">{query_string or "(empty)"}</td></tr>
    <tr><td class="k">Timestamp</td><td class="v">{now}</td></tr>
  </table>

  <h3>Parsed Fields</h3>
  <table>
    <tr><th>Field</th><th>Value</th></tr>
    {rows}
  </table>

  <h3>Raw Body (first 500 chars)</h3>
  <pre>{body_raw[:500] or "(empty — GET request or no body)"}</pre>
</body>
</html>"""
)
