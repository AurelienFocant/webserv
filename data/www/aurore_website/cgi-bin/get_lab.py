#!/usr/bin/env python3

import os
import html
import urllib.parse

def main():
    raw_query = os.environ.get("QUERY_STRING", "")
    params = urllib.parse.parse_qs(raw_query)

    name = params.get("name", ["Anonymous"])[0]
    mode = params.get("mode", ["retro"])[0]
    message = params.get("message", ["No message sent."])[0]

    name = html.escape(name)
    mode = html.escape(mode)
    message = html.escape(message)
    raw_query = html.escape(raw_query)
    request_method = html.escape(os.environ.get("REQUEST_METHOD", ""))
    script_name = html.escape(os.environ.get("SCRIPT_NAME", ""))

    print("Content-Type: text/html")
    print("Status: 200")
    print()

    print(f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>GET CGI Lab</title>
  <style>
    :root {{
      --pink:#ff4fa3;
      --rose:#ff7fb3;
      --orange:#ff5a2f;
      --yellow:#FBCB3E;
      --cyan:#259AA1;
      --blue:#203D57;
      --cream:#F8E2B9;
      --text:#1d1820;
    }}

    * {{ box-sizing: border-box; }}

    body {{
      margin: 0;
      min-height: 100vh;
      padding: 24px;
      display: grid;
      place-items: center;
      font-family: Georgia, serif;
      color: var(--text);
      background: radial-gradient(circle at 50% 20%, #ff9266 0%, var(--orange) 40%, #e6452d 100%);
    }}

    .panel {{
      width: min(820px, 100%);
      padding: 34px;
      border-radius: 26px;
      background: linear-gradient(145deg, rgba(255,255,255,0.86), rgba(255,255,255,0.62));
      box-shadow:
        0 18px 42px rgba(0,0,0,0.22),
        inset 0 0 12px rgba(255,255,255,0.45);
      backdrop-filter: blur(8px);
    }}

    .eyebrow {{
      font-family: Arial, sans-serif;
      text-transform: uppercase;
      letter-spacing: .2em;
      font-size: .75rem;
      color: var(--blue);
      margin-bottom: 10px;
    }}

    h1 {{
      margin: 0 0 18px;
      font-family: Arial, sans-serif;
      text-transform: uppercase;
      letter-spacing: .14em;
      color: var(--pink);
      font-size: clamp(2rem, 5vw, 3.6rem);
    }}

    .lead {{
      margin: 0 0 22px;
      font-size: 1.12rem;
      line-height: 1.7;
      color: #2a2430;
    }}

    .grid {{
      display: grid;
      gap: 14px;
      margin-top: 18px;
    }}

    .block {{
      padding: 15px 16px;
      border-radius: 16px;
      background: rgba(248,226,185,0.58);
    }}

    .label {{
      font-family: Arial, sans-serif;
      text-transform: uppercase;
      letter-spacing: .15em;
      font-size: .72rem;
      color: var(--blue);
      margin-bottom: 8px;
    }}

    code {{
      word-break: break-word;
      font-family: monospace;
      font-size: .95rem;
    }}

    .actions {{
      margin-top: 24px;
      display: flex;
      flex-wrap: wrap;
      gap: 12px;
    }}

    .button {{
      display: inline-flex;
      align-items: center;
      justify-content: center;
      padding: 12px 18px;
      border-radius: 999px;
      text-decoration: none;
      color: white;
      font-family: Arial, sans-serif;
      text-transform: uppercase;
      letter-spacing: .15em;
      font-size: .78rem;
      background: linear-gradient(135deg, var(--pink), var(--rose));
    }}
  </style>
</head>
<body>
  <div class="panel">
    <div class="eyebrow">GET CGI result</div>
    <h1>Hello {name}</h1>
    <p class="lead">
      You reached a Python CGI script through a <strong>GET</strong> request with a <strong>query string</strong>.
    </p>

    <div class="grid">
      <div class="block">
        <div class="label">Mode</div>
        <div>{mode}</div>
      </div>

      <div class="block">
        <div class="label">Message</div>
        <div>{message}</div>
      </div>

      <div class="block">
        <div class="label">REQUEST_METHOD</div>
        <code>{request_method}</code>
      </div>

      <div class="block">
        <div class="label">SCRIPT_NAME</div>
        <code>{script_name}</code>
      </div>

      <div class="block">
        <div class="label">QUERY_STRING</div>
        <code>{raw_query if raw_query else '(empty)'}</code>
      </div>

      <div class="block">
        <div class="label">Server proof</div>
        <div>This page was generated dynamically by a Python CGI script executed through <code>cgi_exec</code>.</div>
      </div>
    </div>

    <div class="actions">
      <a class="button" href="/">Back to index</a>
      <a class="button" href="/cgi-lab.html">Back to CGI lab</a>
    </div>
  </div>
</body>
</html>""")

if __name__ == "__main__":
    main()