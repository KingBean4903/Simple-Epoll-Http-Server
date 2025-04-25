#include "router.hpp"

void Router::add_route(const std::string& method,
		const std::string& path,
		RouteHandler handler) {

	routes[method][path] = handler;

}

HttpResponse Router::handle_request(const HttpRequest& request) const {

	std::string method_str;

	switch(request.method) { 

		case HttpMethod::GET: method_str = "GET"; break;
		case HttpMethod::POST: method_str = "POST"; break;
		case HttpMethod::PUT: method_str = "PUT"; break;
		case HttpMethod::DELETE: method_str = "DELETE"; break;
		default: return HttpResponse{400, "Bad Request", {}, "Invalid Method"};
	}

	if (routes.count(method_str) && routes.at(method_str).count(request_path)) {
			return routes.at(method_str).at(request.path)(request);
	}

	return HttpResponse{404, "Not Found", {}, "404 Not Found"};

}
