#### This project has been created as part of the 42 curriculum by igngonza marcolop and fdurban-

# Description

#### webserv is a high-performance HTTP/1.1 web server written in C++ that implements non-blocking I/O using the Linux epoll API. The server is designed to handle multiple concurrent connections efficiently through an event-driven architecture, making it suitable for production-like environments while adhering to the constraints of the 42 curriculum.

#### The project implements a complete HTTP server with support for:

#### Non-blocking I/O: Uses epoll for efficient I/O multiplexing to monitor multiple file descriptors
#### HTTP Request Parsing: State-machine-based parser that handles fragmented TCP packets and chunked transfer encoding HttpRequest.cpp:41-53
#### Configuration Management: Flexible configuration file parser supporting multiple server blocks and location-based routing default.conf:1-57
#### CGI Support: Executes CGI scripts for PHP and Python extensions default.conf:41-52
#### Timeout Enforcement: Implements both idle and absolute connection timeouts to prevent resource exhaustion

## Architecture and components

#### The following diagram illustrates how high-level architectural stages map to specific classes and methods within the codebase.

# Instructions

## Compilation

#### The project uses a standard C++ build process. Ensure you have a C++ compiler (g++ or clang++) and make installed.

# make

## Configuration

#### Before running the server, ensure you have a valid configuration file. A default configuration is provided at config/default.conf which demonstrates:

#### Multiple server blocks listening on different ports
#### Location-based routing with different access rules
#### CGI script execution
#### File upload handling
#### Custom error pages default.conf:1-90

## Execution

#### Run the server with the configuration file:

#### ./webserver config/default.conf

#### The server will start listening on the configured ports (default: 8080, 8081, 9000, 8000, 8070).

## Testing

#### The project includes a comprehensive test suite to validate robustness:
```
# Test slow request handling (fragmented headers)  
./tests/slow_request.sh  
  
# Test chunked transfer encoding  
./tests/fer_slow_request.sh  
  
# Test concurrent connections  
./tests/concurrent_slow_requests.sh  
  
# High-speed load testing  
python3 tests/fast_concurrent.py
```

#### These tests validate the server's ability to handle partial data, slow clients, and high-concurrency scenarios concurrent_slow_requests.sh:1-7 .
## Features
#### HTTP Methods

#### GET, POST, DELETE, HEAD HttpRequest.cpp:41-47
#### HTTP/0.9 protocol support HttpRequest.cpp:48-52

## Server Configuration

#### Multiple virtual servers with different IP/port combinations
#### Server name-based virtual hosting
#### Client body size limits per location
#### Custom error pages for different status codes
#### Directory listing (autoindex) control

## Request Handling

#### Chunked transfer encoding support HttpRequest.cpp:219-237
#### Content-Length validation
#### Host header validation
#### Request timeout enforcement

## Response Generation

#### Dynamic response construction based on configuration
#### Error page serving
#### File upload handling
#### CGI script execution with proper environment variable setup

## Technical Choices
## Architecture

## The server follows a reactor pattern using epoll for event notification. The main components are:

#### Server: Orchestrates the event loop and manages client connections Server.hpp:14-39
#### Epoll: Wrapper around Linux epoll API for I/O multiplexing
#### Socket: Handles socket creation, binding, and listening
#### Client: Manages individual connection state and I/O operations
#### HttpRequest: State machine for parsing incoming HTTP requests
#### HttpResponse: Constructs HTTP responses based on request and configuration

## State Machine Design

#### The HTTP parser uses a state-based approach to handle incomplete data gracefully, transitioning between INCOMPLETE, COMPLETE, and ERROR states as data arrives.
## Resources
## Documentation & References

## RFC 7230 - HTTP/1.1 Message Syntax and Routing
#### RFC 7231 - HTTP/1.1 Semantics and Content
#### Linux epoll(7) man page
#### 42 Network System Engineering curriculum guidelines

## AI Usage

#### AI was used during the development of this project for:

#### Code structure optimization: Suggesting improvements to the class hierarchy and separation of concerns
#### Debugging assistance: Helping identify edge cases in HTTP request parsing, particularly around chunked transfer encoding
#### Test case generation: Assisting in creating comprehensive test scenarios for concurrent connections and slow clients
#### Documentation: Helping structure technical documentation and generate architectural diagrams

## AI tools were used as a supplementary resource to accelerate development and ensure code quality, but all core logic, implementation details, and architectural decisions were made by the project authors.
## Notes

## This README is based on the current codebase structure. Some implementation details may vary depending on the specific branch or version of the project. The server is designed to be compliant with the 42 curriculum requirements while demonstrating production-ready coding practices in C++.
