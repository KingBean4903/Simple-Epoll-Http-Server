#pragma once
#include <string>
#include <unordered_map>

struct HttpResponse {

	int status_code;
	std::string status_text;
	std::unordered_map<std::string, std::string> headers;
	std::string body;

	std::string to_string() const;

	static HttpResponse from_file(const std::string& filepath);
	static HttpResponse json_response(const std::string& json, 
			int status = 200);

};




