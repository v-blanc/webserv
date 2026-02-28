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
