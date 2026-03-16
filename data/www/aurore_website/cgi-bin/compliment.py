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

    print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Result</title>
    <style>
    :root {
        --pink:#ff4fa3;
        --rose:#ff7fb3;
        --orange:#ff5a2f;
        --yellow:#FBCB3E;
        --cyan:#259AA1;
        --blue:#203D57;
        --cream:#F8E2B9;
    }

    * { box-sizing: border-box; }

    body {
    margin: 0;
    min-height: 100vh;
    font-family: Georgia, serif;
    background: radial-gradient(circle at 50% 20%, #ff9266 0%, var(--orange) 40%, #e6452d 100%);
    color: #1d1820;
    display: grid;
    place-items: center;
    padding: 24px;
    }

    .panel {
    width: min(760px, 100%);
    border-radius: 28px;
    padding: 36px;
    background: linear-gradient(145deg, rgba(255,255,255,0.86), rgba(255,255,255,0.62));
    box-shadow:
        0 18px 42px rgba(0,0,0,0.22),
        inset 0 0 12px rgba(255,255,255,0.45);
    backdrop-filter: blur(8px);
    }

    .eyebrow {
    font-family: Arial, sans-serif;
    text-transform: uppercase;
    letter-spacing: .2em;
    font-size: .75rem;
    color: var(--blue);
    margin-bottom: 10px;
    }

    h1 {
    margin: 0 0 18px;
    font-family: Arial, sans-serif;
    text-transform: uppercase;
    letter-spacing: .14em;
    color: var(--pink);
    font-size: clamp(2rem, 5vw, 3.6rem);
    }

    .compliment {
    font-size: 1.35rem;
    line-height: 1.7;
    color: #2a2430;
    margin: 0 0 24px;
    }

    .meta {
    display: grid;
    gap: 12px;
    margin-top: 22px;
    padding-top: 22px;
    border-top: 1px solid rgba(32,61,87,0.18);
    }

    .meta-block {
    padding: 14px 16px;
    border-radius: 16px;
    background: rgba(248,226,185,0.55);
    }

    .label {
    font-family: Arial, sans-serif;
    text-transform: uppercase;
    letter-spacing: .15em;
    font-size: .72rem;
    color: var(--blue);
    margin-bottom: 8px;
    }

    .back {
    display: inline-block;
    margin-top: 26px;
    padding: 12px 18px;
    border-radius: 999px;
    text-decoration: none;
    background: linear-gradient(135deg, var(--pink), var(--rose));
    color: white;
    font-family: Arial, sans-serif;
    text-transform: uppercase;
    letter-spacing: .15em;
    font-size: .78rem;
    }

    pre {
    margin: 0;
    white-space: pre-wrap;
    word-break: break-word;
    font: inherit;
    }
</style>
</head>
<body>
""")

    print(f"""
<div class="panel">
    <div class="eyebrow">CGI result</div>
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