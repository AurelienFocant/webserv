#!/usr/bin/env python3

import os
import sys
import html
import random
import urllib.parse


def read_post_data():
    try:
        length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        length = 0

    return sys.stdin.read(length)


def pick_compliment(mood):
    compliments = {
        "curious": [
            "You investigate bugs like a true digital detective.",
            "Your curiosity gives this server its spark.",
            "You turn protocol details into elegant behavior."
        ],
        "confident": [
            "You deploy confidence like a production-grade server.",
            "Your webserv energy is absolutely unstoppable.",
            "You make CGI look effortless."
        ],
        "dramatic": [
            "You bring majestic intensity to every HTTP response.",
            "Even your 200 OK has cinematic presence.",
            "Your server enters like the final scene of a cult classic."
        ],
        "sleepy": [
            "Even half-awake, you parse requests with style.",
            "You serve bytes more gracefully than most people serve coffee.",
            "Your calm debugging aura is unmatched."
        ]
    }

    pool = compliments.get(mood, compliments["curious"])
    return random.choice(pool)


def main():

    raw_data = read_post_data()
    params = urllib.parse.parse_qs(raw_data)

    name = params.get("name", ["Anonymous visitor"])[0]
    mood = params.get("mood", ["curious"])[0]
    message = params.get("message", [""])[0]

    name = html.escape(name)
    mood = html.escape(mood)
    message = html.escape(message)

    compliment = html.escape(pick_compliment(mood))

    print("Content-Type: text/html")
    print()

    print(f"""<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>CGI Result</title>
</head>

<body>

<h1>Hello {name}</h1>

<p>{compliment}</p>

<p><b>Mood:</b> {mood}</p>
<p><b>Message:</b> {message if message else "No message sent."}</p>

<a href="/">Back to index</a>

</body>
</html>
""")


if __name__ == "__main__":
    main()