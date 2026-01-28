#!/usr/bin/env python3

import os
import sys
import urllib.parse

def main():
    # Read POST body
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(content_length)

    data = urllib.parse.parse_qs(body)

    username = data.get("username", [""])[0]
    password = data.get("password", [""])[0]

    # HTTP headers
    print("Content-Type: text/html")
    print()

    # Simple response
    print("<html><body>")
    if username and password:
        print(f"<h1>Login OK</h1>")
        print(f"<p>Welcome, {username}</p>")
    else:
        print("<h1>Login Failed</h1>")
        print("<p>Missing username or password</p>")
    print('<a href="/">Back</a>')
    print("</body></html>")

if __name__ == "__main__":
    main()
