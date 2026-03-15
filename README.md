---
geometry: margin=0.5in
---

# README

---


_This project has been created as part of the 42 curriculum by Agaland, Stempels and Afocant_

## Description

The Webserv project is the implementation of a simple HTTP1.0/1.1 server.

The goal is twofold:

- learning the HTTP protocol in depth by implementing most of its specifications.
- understanding the challenges of using blocking or non-blocking IO in terms of treating client requests.


The model is one of server-client architecture.
The typical workflow is:

- the server is listening to incoming connections on one or multiple TCP ports.
- a client, called user-agent, makes a request to the server, following the HTTP grammar, i.e. with HTTP headers.
- the server parses the request, determine its validity.
- if the client is requesting some content, the server checks for the existence of the requested file
- and if all conditions are met, responds with the file as text, along with HTTP headers to signal the status of the request.


### Nginx reference
---
Our Webserv implements a small subset of Nginx functionalities.
The server is to be configured using a configuration file, of which syntax is very similar to Nginx's.


The following features and directives are implemented:

1. server and location blocks

2. nginx directives:
- listen    (port);
- root      (root);
- alias     (alias);
- index     (index index*);
- server_name   (name);
- autoindex     (on|off);
- keepalive_time    (time);
- keepalive_timeout (time);
- max_body_size     (size);
- error_pages       (code path);
- allowed_methods   (method method*)
- return            (redirect_code location);

3. adapted directives:
- cgi           (on|off);
- cgi_exec      (path);
- cgi_timeout   (time);


## Instructions
make and blahblahblah

## Resources
RFC + Mozilla
