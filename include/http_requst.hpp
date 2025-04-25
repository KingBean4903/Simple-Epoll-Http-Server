#pragma once
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"

enum class HttpMethod {
		GET,
		PUT,
		POST,
		DELETE,
		UNKNOWN
};

struct HttpRequest {
	
	HttpMethod method;
	std::string path;
	std::string version;
	std::unordered_map<std::string, std::string>
		 headers;

	static HttpRequest parse(const std::string& raw_request);

};
