#!/usr/bin/env python3
# remove_comment.py (CGI)
import os, sys, re
from urllib.parse import parse_qs

DB_ROOT = "./data/dB"
USER_RE = re.compile(r"^[A-Za-z0-9_-]+$")
ID_RE   = re.compile(r"^[0-9]{10,}-[0-9a-f]{8}$")  # matches unixms-8hex from add_comment.py

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

def safe_id(cid: str) -> bool:
    return bool(ID_RE.match(cid))

def main():
    raw = cgi_read_body()
    form = parse_qs(raw.decode("utf-8", "replace"), keep_blank_values=True)

    username = (form.get("username", [""])[0]).strip()
    cid      = (form.get("id", [""])[0]).strip()

    if not safe_username(username):
        respond("400 Bad Request", "Invalid username\n")
    if not safe_id(cid):
        respond("400 Bad Request", "Invalid id\n")

    user_dir = os.path.join(DB_ROOT, username)
    if not os.path.isdir(user_dir):
        respond("404 Not Found", "User not found\n")

    comments_file = os.path.join(user_dir, "comment.txt")
    if not os.path.isfile(comments_file):
        respond("404 Not Found", "No comment file\n")

    tmp_path = comments_file + ".tmp"

    removed = False
    with open(comments_file, "r", encoding="utf-8", errors="replace") as src, \
         open(tmp_path, "w", encoding="utf-8") as dst:
        for line in src:
            # expected: id \t comment
            if not line.strip():
                continue
            parts = line.rstrip("\n").split("\t", 1)
            line_id = parts[0] if parts else ""
            if (not removed) and line_id == cid:
                removed = True
                continue
            dst.write(line)

    if not removed:
        try: os.unlink(tmp_path)
        except OSError: pass
        respond("404 Not Found", "Comment id not found\n")

    os.replace(tmp_path, comments_file)

    # Destroy file if empty (size == 0 or only whitespace lines)
    try:
        # Check if any non-empty line remains
        keep = False
        with open(comments_file, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                if line.strip():
                    keep = True
                    break
        if not keep:
            os.unlink(comments_file)
            respond("200 OK", "Comment removed; file deleted (empty)\n")
    except OSError:
        pass

    respond("200 OK", "Comment removed\n")

if __name__ == "__main__":
    main()
