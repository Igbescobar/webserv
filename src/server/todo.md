# new branch

Distinguish `ServerConfig` when `accept()`,
then distinguish `HttpRequest` when `epoll_wait()`

Create a request object with the `ServerConfig` as argument

Call request.append() with the new string chunk read and check what is returned
and act depending of the returned value

Create a response object with the request object as argument and an error
argument if there was an error

# file structure

Directories `request/` and `response/`?

Directory `parser/config/` to `parse_config/`?
