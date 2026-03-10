#!/usr/bin/env python3

import cowsay
import urllib.parse
import os
import sys
import html


HTTP_HEADER = "Content-Type: text/html\n"


def error_animal_not_found():
    html_res = f"""\
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
<p>Well tried motherfucker</p>
<a href="/html/comment.html" class="btn" >Back</a>
<a href="/" class="home-link">WebServ</a>
</body>
</html>
"""
    print(html_res)
    return 2
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
        return error_animal_not_found()

    cowstring = cowsay.get_output_string(animal, comment)
    cowstring = html.escape(cowstring)

    html_res = f"""\
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
<div class="asciiart">{cowstring}</div>
<a href="/html/comment.html" class="btn" >Back</a>
<a href="/" class="home-link">WebServ</a>
</body>
</html>
    """

    print(html_res)


if __name__ == "__main__":
    main()
