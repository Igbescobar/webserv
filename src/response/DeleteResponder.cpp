#include "response/DeleteResponder.hpp"
#include "response/ErrorResponseBuilder.hpp"

HttpResponse DeleteResponder::handle(const ServerConfig &config,
                                     const LocationConfig *location,
                                     const HttpRequest &req) {
  (void)location;
  (void)req;

  return ErrorResponseBuilder::build(config, 501);
}