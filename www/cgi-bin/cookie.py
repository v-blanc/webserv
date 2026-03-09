#!/usr/bin/env python3
"""
cgi-bin/cookie.py
CGI cookie manager: set, read, or delete cookies via Set-Cookie headers.
Tests the full cookie round-trip through WebServ:
  - Server must forward HTTP_COOKIE from request to CGI environment
  - Server must forward Set-Cookie headers from CGI stdout to client
"""

import os
import datetime

# --- Parse QUERY_STRING ---
qs = os.environ.get("QUERY_STRING", "")
params = {}
if qs:
    for pair in qs.split("&"):
        if "=" in pair:
            k, v = pair.split("=", 1)
            # Basic URL decode
            v = v.replace("+", " ").replace("%20", " ").replace("%3D", "=")
            params[k] = v

action = params.get("action", "read")
name = params.get("name", "webserv_session")
value = params.get("value", "default_value")
http_cookie = os.environ.get("HTTP_COOKIE", "")
now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# --- Parse existing cookies ---
existing = {}
if http_cookie:
    for pair in http_cookie.split(";"):
        pair = pair.strip()
        if "=" in pair:
            k, v = pair.split("=", 1)
            existing[k.strip()] = v.strip()

# --- Decide what to do ---
set_cookie_header = ""
action_result = ""

if action == "set":
    set_cookie_header = f"Set-Cookie: {name}={value}; Max-Age=3600; Path=/"
    action_result = f"Cookie SET: <span style='color:#00ff9d'>{name}={value}</span>"

elif action == "delete":
    set_cookie_header = f"Set-Cookie: {name}=; Max-Age=0; Path=/"
    action_result = f"Cookie DELETED: <span style='color:#ff6b6b'>{name}</span>"

elif action == "read":
    found = existing.get(name)
    if found:
        action_result = (
            f"Cookie FOUND: <span style='color:#00ff9d'>{name}={found}</span>"
        )
    else:
        action_result = f"Cookie <span style='color:#ff6b6b'>NOT FOUND</span>: {name}"

# --- Build cookie table ---
cookie_rows = ""
if existing:
    for k, v in existing.items():
        highlight = "color:#ffd166" if k == name else "color:#e8e8f0"
        cookie_rows += f"<tr><td style='color:#888899'>{k}</td><td style='{highlight}'>{v}</td></tr>"
else:
    cookie_rows = (
        "<tr><td colspan='2' style='color:#444'>No cookies in request</td></tr>"
    )

# --- Output ---
# IMPORTANT: Set-Cookie must come BEFORE the blank line
print("Content-Type: text/html; charset=utf-8")
print("X-CGI-Script: cookie.py")
if set_cookie_header:
    print(set_cookie_header)
print()  # blank line

print(
    f"""<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8"><title>Cookie CGI</title>
  <style>
    body {{ font-family:monospace; background:#0a0a0f; color:#e8e8f0; padding:2rem; }}
    h2   {{ color:#7c6bff; }}
    h3   {{ color:#ffd166; margin:1.5rem 0 .5rem; font-size:.85rem; text-transform:uppercase; letter-spacing:.1em; }}
    table{{ border-collapse:collapse; width:100%; }}
    td, th {{ padding:.4rem .8rem; border:1px solid #2a2a3d; font-size:.78rem; }}
    th   {{ color:#00ff9d; }}
    .result {{ background:#111118; border:1px solid #2a2a3d; padding:1rem; border-radius:4px; margin:1rem 0; font-size:.85rem; }}
    .hdr {{ color:#7c6bff; }}
  </style>
</head>
<body>
  <h2>cookie.py — Cookie Manager</h2>

  <div class="result">
    <strong>Action:</strong> <span style="color:#ffd166">{action.upper()}</span><br>
    <strong>Result:</strong> {action_result}<br>
    <strong>Timestamp:</strong> {now}
  </div>

  {"<div class='result'><strong>Set-Cookie header sent:</strong><br><span class='hdr'>" + set_cookie_header + "</span></div>" if set_cookie_header else ""}

  <h3>Cookie Header Received (HTTP_COOKIE)</h3>
  <div class="result" style="color:#888899">{http_cookie or "(no Cookie header in request)"}</div>

  <h3>Parsed Cookies</h3>
  <table>
    <tr><th>Name</th><th>Value</th></tr>
    {cookie_rows}
  </table>

  <h3>Request Info</h3>
  <table>
    <tr><th>Variable</th><th>Value</th></tr>
    <tr><td style="color:#888899">REQUEST_METHOD</td><td>{os.environ.get("REQUEST_METHOD","?")}</td></tr>
    <tr><td style="color:#888899">QUERY_STRING</td><td>{qs}</td></tr>
    <tr><td style="color:#888899">HTTP_COOKIE</td><td>{http_cookie or "(empty)"}</td></tr>
  </table>
</body>
</html>"""
)
