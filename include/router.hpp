#pragma once
#include "http_request.hpp"
#include "http_response.hpp"
#include <functional>
#include <unordered_map>

using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
	public:
			void add_route(const std::string& method,
					const std::string& path, RouterHandler handler);

			HttpResponse handle_request(const HttpRequest& request) const;

	private:
			std::unordered_map<str::string, 
					std::unordered_map<std::string, RouteHandler>> router;

};

