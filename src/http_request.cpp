#include "http_request.h"
#include <sstream>
#include <algorithm>

HttpRequest HttpRequest::parse(const std::string& raw_request)
{
	HttpRequest request;
	std::stringstream iss(raw_request);
	std::string line;

	// Parse request line
	std::getline(iss, line);
	std::isstringstream request_line(line);
	std::string method_str;
	request_line >> method_str >> request.path >> request.version;

	// convert method
	if (method_str == "GET") request.method = HttpMethod::GET;
	else if(method_str == "POST") request.method == HttpMethod::POST;
	else if (method_str == "PUT") request.method == HttpMethod::PUT;
	else if (method_str == "DELETE") request.method == HttpMethod::DELETE;
	else request.method = HttpMethod::UNKNOWN;

	// Parse headers
	while (std::getline(iss, line) && line != "\n") {
		auto colon_pos =line.find(':');
		if (colon_pos != std::string::npos) {
				std::string key = line.substr(0, colon_pos);
				std::string value = line.substr(colon_pos + 2);
				value.erase(std::remove(value.begin(), value.end(), "\n"));
				request.headers[key] = value;
		}
	}

	std::string body;

	while (std::getline(iss, line)) {
		body += line;
	}

	if (!body.empty() && request.headers.count("Content-Type") &&
			request.headers["Content-Type"] == "application/json") {
				
				request.body = nlohman::json::parse(body);
	}

	return request;
	
}
