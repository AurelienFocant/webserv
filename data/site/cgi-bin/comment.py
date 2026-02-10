#!/usr/bin/env python3

import sys
import os
import urllib.parse
from datetime import datetime
import html

# Read request body from stdin
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
body = sys.stdin.read(content_length)

# Parse application/x-www-form-urlencoded body
params = urllib.parse.parse_qs(body)

comment = params.get("comment", [""])[0].strip()

# Prepare response
if not comment:
    response_body = "<h1>Error</h1><p>Empty comment.</p>"
else:
    safe_comment = html.escape(comment)
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    with open("comment", "a", encoding="utf-8") as f:
        f.write(f"[{timestamp}] {safe_comment}\n")

    response_body = "<h1>OK</h1><p>Comment saved.</p>"

# Output HTTP response to stdout
print("HTTP/1.1 200 OK")
print("Content-Type: text/html; charset=utf-8")
print(f"Content-Length: {len(response_body.encode('utf-8'))}")
print()
print(response_body)
