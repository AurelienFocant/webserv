#!/usr/bin/env python3

import os
import sys
import urllib.parse
import html
from datetime import datetime


ROOT = "data/www/orel_website/"
COMMENTS = ROOT + "comments/"


def main():
    # Read POST body
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(content_length)

    print(body, file=sys.stderr)

    data = urllib.parse.parse_qs(body)

    comment = data.get("comment", [""])[0]
    comment = html.escape(comment)

    for file in os.listdir(COMMENTS):
        print(file, file=sys.stderr)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"file_{timestamp}"
    with open(COMMENTS + filename, "w+") as f:
        f.write(comment)

    # HTTP headers
    print("Content-Type: text/html")
    print()

    print("<!DOCTYPE html>")
    print("<html>")
    print("<head><meta charset=\"UTF-8\"><title>WebServ</title></head>")
    print("<body>")
    if comment:
        print("<h1>Comment Received</h1>")
        print(f"<p>{comment}</p>")
    else:
        print("<h1>Error</h1>")
        print("<p>No comment provided</p>")
    print('<a href="/html/comment.html">Back</a>')
    print("</body></html>")


if __name__ == "__main__":
    main()
