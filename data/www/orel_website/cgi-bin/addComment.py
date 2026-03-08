#!/usr/bin/env python3
# add_comment.py (CGI)
import os
import sys
import re
import time
import secrets
from urllib.parse import parse_qs

DB_ROOT = "./data/dB"
USER_RE = re.compile(r"^[A-Za-z0-9_-]+$")


def cgi_read_body() -> bytes:
    try:
        n = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        n = 0
    return sys.stdin.buffer.read(n) if n > 0 else b""


def respond(status: str, body: str) -> None:
    sys.stdout.write(f"Status: {status}\r\nContent-Type: text/plain\r\n\r\n{body}")
    sys.exit(0)


def safe_username(u: str) -> bool:
    return bool(USER_RE.match(u))


def next_comment_id() -> str:
    # Stable, sortable id: unixms-rand
    return f"{int(time.time() * 1000)}-{secrets.token_hex(4)}"


def main():
    raw = cgi_read_body()
    form = parse_qs(raw.decode("utf-8", "replace"), keep_blank_values=True)

    username = (form.get("username", [""])[0]).strip()
    comment  = (form.get("comment",  [""])[0])

    if not safe_username(username):
        respond("400 Bad Request", "Invalid username\n")
    if comment.strip() == "":
        respond("400 Bad Request", "Empty comment\n")

    user_dir = os.path.join(DB_ROOT, username)
    if not os.path.isdir(user_dir):
        respond("404 Not Found", "User not found\n")

    # Force single-line comment to keep storage simple and deletion robust
    comment = comment.replace("\r", "").replace("\n", " ").strip()

    comment_id = next_comment_id()
    comments_file = os.path.join(user_dir, "comment.txt")

    # File format: <id>\t<comment>\n
    # (tab separator; easy to parse, comment can contain commas safely)
    os.umask(0o077)
    with open(comments_file, "a", encoding="utf-8") as f:
        f.write(f"{comment_id}\t{comment}\n")

    respond("200 OK", f"Comment added\nid={comment_id}\n")


if __name__ == "__main__":
    main()
