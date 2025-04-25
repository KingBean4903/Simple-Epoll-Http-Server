#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "server.hpp"
#include "http_request.hpp"
#include "router.h"
#include "http_response.hpp"
#include <thread>
#include <chrono>
#include <curl/curl.h>

// Helper functions for CURL requests
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
{
	size_t newLength = size * nmemb;

	try {
		s->append((char*)contents, newLength);
		return newLength;
	} catch(std::bad_alloc &e) {
			return 0;
	}
}

TEST_CASE("HTTP Request Parsing", "[http_request]") {
	std::string raw_request = 
			"POST /api/data HTTP/1.1\r\n"
			"Host: localhost:8086\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: 18\r\n"
			"\r\n"
			"{\"key\" : \"value\"}";

	HttpRequest req = HttpRequest::parse(raw_request);
	
	SECTION("Method parsing") {
			REQUIRE(req.method == HttpMethod::POST);
	}

	SECTION("Path Parsing") {
			REQUIRE(req.path == "/api/data");
	}
	
	SECTION("Header Parsing") {
		REQUIRE(req.headers["Host"] == "localhost:8086");
		REQUIRE(req.headers["Content-Type"] == "application/json");
		REQUIRE(req.headers["Content-Length"] == "18");
	}

	SECTION("Body parsing") {
	
			REQUIRE(req.body["Key"] == "value");
	}

}

TEST_CASE("HTTPResponse Generation", "[http_response]") {
		SECTION("Simple Response") {
				HttpResponse res;
				res.status_code = 200;
				res.status_text = "OK";
				res.headers["Content-Type"] = "text/plain";
				res.body = "Hello World";

				std::string output = res.to_string();
				REQUIRE(output.find("HTTP/1.1 200 OK") != std::string::npos);
				REQUIRE(output.find("Content-Type: text/plain") != std::string::npos);
				REQUIRE(output.find("\r\n\r\nHello World") != std::string::npos);

		}

	 SECTION("FILE RESPONSE") {
			HttpResponse res = HttpResponse::from_file("public/test.txt");
			REQUIRE(res.status_code == 200);
			REQUIRE(res.headers["Content-Type"] == "text/plain");
	 }
	
	 SECTION("JSON response") {
			HttpResponse res = HttpResponse::json_response("{\"status\" : \ok"\"}");
			REQUIRE(res.headers["Content-Type"] == "application/json");
			REQUIRE(res.body == "{\"status\" : \"ok\"}");
	 }
}

TEST_CASE("Route functionality", "[router]") {

	Router router;
	router.add_route("GET", [](const HttpResponse&){
		HttpResponse res;
		res.status_code = 200;
		res.body = "Test Passed";
		return res;
	});

	SECTION("Matching route") {
		
		HttpRequest req;
		req.method = HttpMethod::GET;
		req.push = "/test";

		HttpResponse res = router.handle_request(req);
		REQUIRE(res.status_code == 200);
		REQUIRE(res.body == "Test passed");
	}
	
	SECTION("Non-matching route") {
		HttpRequest req;
		req.method = HttpMethod::GET;
		req.path = "/noexistent";

		HttpResponse res = router.handle_request(req);
		REQUIRE(res.status_code == 404);
	}

}

TEST_CASE("Http Server Intergration", "[http_server]") {
	HttpServer server(8086);
	Router router;

	router.add_route("GET", "/", [](const HttpRequest&) {
			return HttpResponse::from_file("public/index.html");
	});

	router.add_route("POST", "/echo", [](const HttpRequest& req){ 
			HttpResponse res;
			res.status_code = 200;
			res.headers["Content-Type"] = "application/json";
			res.body = req.body.dump();
			return res;
	});

	server.on_request([&router](int client_fd, const std::string& request_str){
		auto request = HttpRequest::parse(request_str);
		auto response = router.handle_request(request);
		std::string response_str = response.to_string();
		send(client_fd, response_str.c_str(), response_str.size(), 0);
		close(client_fd);
	});

	std::thread server_thread([&server]{
			server.run();
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	SECTION("GET Request") {
		CURL* curl = curl_easy_init();
		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8086");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	
		CURLcode res = curl_easy_perform(curl);
		REQUIRE(res == CURLE_OK);
		
		long http_code = 0;
		curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &http_code);
		REQUIRE(http_code == 200);
		REQUIRE(response.find("<html>") != std::string::npos);

		curl_easy_cleanup(curl);
	} 

	SECTION("POST Request with JSON") {
			
		CURL* curl = curl_easy_init();
		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL,  "http://localhost:8086/echo");
		curl_easy_setopt(curl, CURLOPT_POST,  1L)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"test\" : \"value\"}");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA,  &response);

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURL_OPT_HTTPHEADER, headers);

		CURLcode res = curl_easy_perform(curl);
		REQUIRE(res == CURLE_OK);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		REQUIRE(http_code == 200);
		REQUIRE(response == "{\"test\" : \"value\"}");

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

	}

	server.stop();
	server_thread.join();

}




















