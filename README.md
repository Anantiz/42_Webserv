# Webserv

## Description

Project part of 42-school common curriculum.

Group-project: 2 students

Using C++ (std-98) write a simple web-server inspired from Nginx with HTTP/1.1 RFC-2616 compliance.

#### Features:
- Supports config files akin to nginx.conf
- Supports CGI (python, bash, php, etc...)
- Supported methods: `GET` `POST` `DELETE`

#### Allowed libraries:
- **None**

## Technical overview

0. Parse an advanced config file that defines the webserver properties.

1. Accept connections with a single-thread; Use unix file descriptors for I/O polling using the poll.h header and poll(), be non-blocking.

2. Stream-Parse the incoming network data according to RFC, including multi-parts payloads.

3. Dispatch the request to the appropriate virtual server, execute CGI and send the appropriate response in a non-blocking stream.

### How to run

Requirements: [c++, make]

```
# Build
make
# Run
./webserv [Optional: config_path]
```

The default config is "configs/webserv-default.conf" from the project root.
In order to serve files, you should edit the absolute paths accordingly.
___

### Skills & Lesson learned

- Understanding and Implementing protocols (I legit read the whole RFC-2616)
- Unix networking, Sockets
- I/O polling, stream-parsing
- Project file hierarchy & organization
- C++98 (working under constraints)

___

### Notes:

The provided example configs might not work by default as they only support absolute paths.

Also features a cursed typedef used for virtual servers:

```c++
/** @brief
*  It is a list of ports, each port has a list of names
*    these names are associated with a server, hardcore type but straightforward
*  e.i:
*   [(80, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)]),
*   (443, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)])]
*/
std::vector<std::pair<u_int16_t, std::vector< std::pair<std::string, Server*>>>> evil_typedef_t;
```