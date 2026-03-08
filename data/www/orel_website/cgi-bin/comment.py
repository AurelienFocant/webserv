#!/usr/bin/env python3

import os
import sys
import urllib.parse
import html
from datetime import datetime


ROOT = "data/www/orel_website/"
COMMENTS = ROOT + "comments/"


def save_comment_to_file(comment):
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"comment_{timestamp}"
    with open(COMMENTS + filename, "w+") as f:
        f.write(comment)


def main():
    # Read POST body
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(content_length)

    print(body, file=sys.stderr)

    data = urllib.parse.parse_qs(body)

    comment = data.get("comment", [""])[0]
    comment = html.escape(comment)

    if comment:
        save_comment_to_file(comment)
        cmt_html = f"""
        <header>
        <h1>Thanks for your comment!</h1>
        </header>
        <p>{comment}</p>
        """
    else:
        cmt_html = """
        <header>
        <h1>Error</h1>
        </header>
        <p>No comment provided</p>
        """

    # HTTP headers
    print("Content-Type: text/html")
    print()
    print(f"""
<!DOCTYPE html>
<html>
<head>
<meta charset=\"UTF-8\"><title>WebServ</title>
<link rel="stylesheet" href="../css/styles.css">
</head>
<body>
{cmt_html}
<a href="/html/comment.html" class="btn" >Back</a>
<a href="/" class="home-link">WebServ</a>
</body>
</html>
""")


if __name__ == "__main__":
    main()
