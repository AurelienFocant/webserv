#!/usr/bin/env python3

import cowsay
import urllib.parse
import os
import sys


HTTP_HEADER = """
Content-Type: text/html
"""


def error_animal_not_found():
    sys.exit(1)


def cgi_read_body() -> bytes:
    qs = os.environ.get("QUERY_STRING", "")
    return qs.encode("utf8")


def main():
    raw = cgi_read_body()
    form = urllib.parse.parse_qs(raw.decode("utf8", "replace"), keep_blank_values=True)

    animal  = form.get("animal", [""])[0]
    comment = form.get("comment", [""])[0]

    animals = cowsay.char_names

    if animal not in animals:
        error_animal_not_found()

    cowstring = cowsay.get_output_string(animal, comment)

    html_res = f"""
{HTTP_HEADER}
<!DOCTYPE html>
<html>
<head>
<meta charset=\"UTF-8\"><title>WebServ</title>
<link rel="stylesheet" href="../css/styles.css">
</head>
<body>
<header>
<h1>Thanks for your comment!</h1>
</header>
{cowstring}
<a href="/html/comment.html" class="btn" >Back</a>
<a href="/" class="home-link">WebServ</a>
</body>
</html>
    """

    print(html_res)
    print(html_res, file=sys.stderr)


if __name__ == "__main__":
    main()
