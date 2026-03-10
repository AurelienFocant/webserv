#!/usr/bin/env bash

printf "Content-Type: text/html\n\n"
printf "<!DOCTYPE html>\n"
printf "<html>\n"
printf "<head>\n"
printf "<meta charset=\"UTF-8\"><title>WebServ</title>\n"
printf "<link rel="stylesheet" href="../css/styles.css">\n"
printf "</head>\n"
printf "<body>\n"
printf "<header>\n"
printf "<h1>So many comments Dude!</h1>\n"
printf "</header>\n"

for file in ./data/www/orel_website/comments/*; do
	printf "<p class="container">$(cat $file)</p>\n"
done

printf "<a href="/html/comment.html" class="btn" >Back</a>\n"
printf "<a href="/" class="home-link">WebServ</a>\n"
printf "</body>\n"
printf "</html>\n"
