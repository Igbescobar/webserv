_This project has been created as part of the 42 curriculum by igngonza, marcolop, fdurban-._
# webserv
## Description
A high-performance HTTP/1.0 web server written in C++98, implementing non-blocking I/O with the Linux `epoll` API.

The server handles multiple concurrent connections through an event-driven, reactor-pattern architecture, aiming for production-like robustness within the constraints of the curriculum.

## Features

- **Non-blocking I/O** — `epoll`-based multiplexing to monitor multiple file descriptors without blocking
- **HTTP request parsing** — state-machine parser that handles fragmented TCP packets and chunked transfer encoding
- **Configuration management** — flexible config file parser supporting multiple server blocks and location-based routing
- **CGI support** — executes CGI scripts for Python and Bash
- **Timeout enforcement** — idle and absolute connection timeouts to prevent resource exhaustion

### HTTP methods and protocol support

- `GET`, `POST`, `DELETE`, `HEAD`
- HTTP/1.0 compatibility mode

### Server configuration

- Multiple virtual servers across different IP/port combinations
- Per-location client body size limits
- Custom error pages per status code
- Directory listing (autoindex) toggle

### Request handling

- Chunked transfer encoding
- `Content-Length` validation
- Request timeout enforcement

### Response generation

- Dynamic responses based on configuration
- Error page serving
- File upload handling
- CGI execution with proper environment variable setup

## Architecture

The server follows a reactor pattern built around `epoll` for event notification.

| Component | Responsibility |
|---|---|
| `Server` | Orchestrates the event loop and manages client connections |
| `Epoll` | Wrapper around the Linux `epoll` API for I/O multiplexing |
| `Socket` | Socket creation, binding, and listening |
| `Client` | Per-connection state and I/O operations |
| `HttpRequest` | State machine for parsing incoming HTTP requests |
| `HttpResponse` | Builds HTTP responses from request + configuration |

### Parser state machine

The HTTP parser transitions between `INCOMPLETE`, `COMPLETE`, and `ERROR` states as data arrives, so it can handle partial reads and slow clients without blocking the event loop.

## Instructions

### Build

Requires a C++ compiler (`g++`) and `make`.

For object files cleaning use `make clean` and for full cleaning including execution file use `make fclean`

To do a full clean and recompiling use `make re`

```bash
make

make clean

make fclean

make re
```

### Configure

A default configuration is provided at `config/default.conf`, demonstrating:

- Multiple server blocks on different ports
- Location-based routing with distinct access rules
- CGI script execution
- File upload handling
- Custom error pages

### Run

```bash
./webserver config/default.conf
```

By default the server listens on ports `8080`, `8081`, `9000`, `8000`, and `8070`.

## Testing

```bash
# Fragmented headers / slow request handling
./tests/slow_request.sh

# Concurrent connections
./tests/concurrent_slow_requests.sh

# High-concurrency load test
python3 tests/fast_concurrent.py
```

These validate the server's handling of partial data, slow clients, and high-concurrency scenarios.

## References

- [RFC 9110: STD 97: HTTP Semantics](https://www.rfc-editor.org/info/rfc9110/)
- `epoll(7)` man page
- [Webserver guideline and basic concepts](https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7)
- [I/O multiplexing: Doing I/O with many sources using select, poll and epoll calls in Linux.](https://youtu.be/dEHZb9JsmOU?si=EBSWWza2JPiB15fi)
- [Linux's epoll explained](https://www.youtube.com/watch?v=eaT6XtfyGHQ&t=190s)
- [99% of Developers Don't Get Sockets](https://www.youtube.com/watch?v=D26sUZ6DHNQ&t=165s)

## AI usage disclosure

AI tools were used as a supplementary aid during development, specifically for:

- Suggesting improvements to class hierarchy and separation of concerns
- Helping identify edge cases in HTTP request parsing (particularly chunked transfer encoding)
- Assisting in generating test scenarios for concurrent connections and slow clients
- Structuring this documentation

## Notes

This README reflects the current codebase structure; some details may vary by branch or version. The server targets full compliance with 42 curriculum requirements while following production-oriented C++ practices.
