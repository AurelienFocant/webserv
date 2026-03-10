#!/usr/bin/env python3

import cowsay
import urllib.parse
import os
import sys
import html


SEAL="""
       _
    (\\/ )
      \\ |
       )|
      /  \\
   ,-     \\
  /6 6     \\
&/(_x_ ),_/%)"""


HTTP_HEADER = "Content-Type: text/html\n"


def make_html_body(content):
    html_res = f"""\
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
{content}
<a href="/html/comment.html" class="btn" >Back</a>
<a href="/" class="home-link">WebServ</a>
</body>
</html>
"""
    return(html_res)


def cgi_read_body() -> bytes:
    qs = os.environ.get("QUERY_STRING", "")
    return qs.encode("utf8")


def main():
    raw = cgi_read_body()
    form = urllib.parse.parse_qs(raw.decode("utf8", "replace"), keep_blank_values=True)

    animal  = form.get("animal", [""])[0]
    comment = form.get("comment", [""])[0]

    animals = cowsay.char_names
    if animal == "aurore":
        html_body = make_html_body(f"<div class=\"asciiart\">{SEAL}</div>")
    elif animal not in animals:
        html_body = make_html_body("<p>Well tried motherfucker</p>")
    else:
        cowstring = cowsay.get_output_string(animal, comment)
        cowstring = html.escape(cowstring)
        html_body = make_html_body(f"<div class=\"asciiart\">{cowstring}</div>")

    print(f"""\
{HTTP_HEADER}
{html_body}""")


if __name__ == "__main__":
    main()
