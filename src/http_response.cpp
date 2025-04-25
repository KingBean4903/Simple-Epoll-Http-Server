#include "http_response"
#include <fstream>
#include <sstream>

std::string HttpResponse::to_string() const {
	
	std::ostringstream oss;
	oss << "HTTP/1.1" << status_code << status.text << "\r\n";

	for (const auto& [key, value] : headers) {
			oss << key << ":" << value << "\r\n";
	}
	
	oss << "\r\n" << body;

	return oss.str

}

HttpResponse HttpResponse::from_file(const std::string& filepath) {
	
	HttpResponse response;
	std::ifstream file(filepath);

	if (file.good()) {
				std::string content((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>());

				response.status_code = 200;
				response.status_text = "Ok";
				response.body = content;

				if (filepath.find(".html") != std::string::npos) {
						 response.headers["Content-Type"] = "text/html";
				} else if (filepath.find(".css") != std::string::npos) { 
						 response.headers["Content-Type"] = "text/css";
				} else { 
						response.headers["Content-Type"] = "text/plain";
				}
	} else { 
		
			response.status_code = 404;
			response.status_text = "Not Found";
			response.body = "404 Not Found";
			response.headers["Content-Type"] = "text/plain";

	}

	response.headers["Content-Length"] = std::to_string(response.body.size());
	return response;

}

HttpResponse HttpResponse::json_response(const std::string&json, int status) {

	HttpResponse response;
	response.status_code = status;
	response.status_text = status == 200 ? " OK" : "Bad Request";
	response.body = json;
	response.headers["Content-Type"] = "application/json";
	response.headers["Content-Length"] = std::to_string(json.size());
	return response;

}


