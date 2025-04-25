#include "server.hpp"
#include "router.h"
#include "http_response.h"
#include <iostream>


int main() { 
	
	try {
			
		HttpServer server(8060);
		Router router;

		router.add_route("GET", "/", [](const HttpRequest&) {
			return HttpResponse::from_file("public/index.html");
		});

		router.add_route("GET", "/", [](const HttRequest&){
			return HttpResponse::from_file("public/dashboard.html");
		});

		router.add_route("POST", "/api/data", [](const HttpRequest& req){
				if (req.body.empty()) {
							return HttpResponse::json_response("{\"error\" : \" No data provided\"}", 400);
				}
				return HttpResponse::json_response("{\"status\" : \"success\"}");
		});

		server.on_request([&router](int client_fd, const std::string& request_str) {
				
				auto request = HttpRequest::parse(request_str);
				auto response = route.handle_request(request);
				std::string response_str = response.to_string();
				send(client_fd, response_str.c_str(), 0);
				close(client_fd);
		});
	
		server.run();

	} catch(const std::exception& e) {

			std::cerr << "Error: " << e.what() << std::endl;
			return 1;
	}

	return 0;
}
