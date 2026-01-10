#!/usr/bin/env python3
import sys
import os

body = sys.stdin.read()
print("Content-Type: text/plain\n")
print("=== POST Body Test ===")
print("received body:", body)
print("Content-Length:", os.environ.get("CONTENT_LENGTH", "undef"))
print("Content-Type:", os.environ.get("CONTENT_TYPE", "undef"))
print("Request-Method:", os.environ.get("REQUEST_METHOD", "undef"))