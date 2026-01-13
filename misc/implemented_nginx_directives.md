---
geometry: "left=25mm,right=25mm,top=10mm,bottom=25mm"
output: pdf_document
fontfamily: roboto
fontsize: 14pt
monofont: "Courier New"
toc: false
---

## Implemented

1. Contexts:
- http {}
    - (cannot be duplicated)
- server {}
    - (can be duplicated)
- location {}
    - (can be duplicated)

2. Directives:
- location  --> server (no embedded locations)
    - (can be duplicated)
- listen  --> server
    - (can be duplicated)
    - !! has to implement port AND ip address ?
- root    --> http, server, location
    - (cannot be duplicated)
- server_name


## Should be implemented
- index                 --> http, server, location (default to index.html)
- autoindex             --> http, server, location (show as directory listing)
### - limit_except           --> location
- keepalive_time
    - (absolute time of a open connection)
    - (default 1h)
- keepalive_timeout
    - (timeout before idle connection is closed)
    - (default 75s)
- client_body_timeout   --> http, server, location
    - (default 60s)


## Could be implemented

- alias

## Question Mark ?

- chunked_transfer_encoding
