#!/bin/sh
# get_all_comments.sh (CGI) - list all comments from all users
# Output: text/plain TSV: username \t id \t comment \n
# DB root relative to /webserv:
DB_ROOT="./data/dB"

printf "Status: 200 OK\r\nContent-Type: text/plain\r\n\r\n"

# If DB missing: return empty
[ -d "$DB_ROOT" ] || exit 0

# Iterate user dirs (only directories)
for userdir in "$DB_ROOT"/*; do
  [ -d "$userdir" ] || continue
  username="$(basename "$userdir")"
  comments="$userdir/comment.txt"
  [ -f "$comments" ] || continue

  # Each line is: id \t comment
  # Output: username \t id \t comment
  # Sanitize comment to one line + no tabs (defensive)
  while IFS= read -r line; do
    [ -n "$line" ] || continue
    id="${line%%	*}"
    rest="${line#*	}"

    # If no tab in line, skip (corrupted)
    [ "$id" != "$line" ] || continue

    # sanitize rest: remove CR, convert tabs/newlines to spaces
    rest="$(printf '%s' "$rest" | tr -d '\r' | tr '\t' ' ')"
    printf "%s\t%s\t%s\n" "$username" "$id" "$rest"
  done < "$comments"
done
