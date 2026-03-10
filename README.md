*This project has been created as part of the 42 curriculum by vblanc, yafahfou, yabokhar.*

## Description

This project consists of an **HTTP server written in C++ (C++98 standard)**. Its main goal is to recreate the core behavior of a real web server by handling client connections, parsing HTTP requests, and generating appropriate HTTP responses.

The server is configured using a **configuration file inspired by nginx**, allowing flexible setup of server blocks, routes, ports, error pages, and other directives. It supports the three main HTTP methods: **GET**, **POST**, and **DELETE**.

Additionally, the server implements **CGI (Common Gateway Interface)** support, enabling the execution of dynamic scripts such as **.pl (Perl)**, **.py (Python)**, **.php (PHP)**, and **.cgi** files. This allows the server to handle dynamic content alongside static files.

The project focuses on **low-level network programming**, **process management**, **file handling**, and strict compliance with the **HTTP/1.1 protocol**, while respecting the constraints of the **C++98 standard**.

## Instructions

The project is compiled using the provided Makefile:

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and the executable
make re     # Recompile everything
```

The executable generated is:

```bash
./webserv
```

To run the server:

```bash
./webserv [configuration_file] (if no argument is given, the server will use the default configuration file named "config/good/webserv.conf")
```


## Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- ["99% of Developers Don't Get Sockets"](https://www.youtube.com/watch?v=D26sUZ6DHNQ)
- [Socket Programming in C++](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)
- [HTTP Status Codes — Wikipedia (FR)](https://fr.wikipedia.org/wiki/Liste_des_codes_HTTP)
- [Common Gateway Interface](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
- [HTTP Status Codes — MDN](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status)
- [HTTP Semantics — RFC 9110](https://datatracker.ietf.org/doc/html/rfc9110)
- [HTTP Messages — MDN](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages)
- [HTTP/1.1 — RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112)
- [Cookie Header — MDN](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/Cookie)
- AI was used to generate HTML pages, server tests and to introduce us to Github Actions

### AI Usage

AI tools were used for:
- Improving documentation wording
- Debugging specific errors
- Generating HTML pages, server tests (introduction to Github Actions)

***All code was written, reviewed, and validated by team members.</br>
AI was not used to generate complete features without understanding.***
