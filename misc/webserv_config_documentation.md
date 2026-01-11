# Documentation for afocant, agaland and stempels' WebServ Config

## nginx subset

The configuration of this WebServ is a very simple and barebone subset of the NGINX configuration

It includes :
### blocks:
- server blocks
- location blocks

### directives:
- listen
- root
- server_name
- index
- autoindex
- keepalive_timeout


### shortcomings:
- there is no general http context
- locations do not support regex pattern matching
- listen can only do so on a port
- only one server name can be specified
- root can only be an absolute path
