#pragma once

#include <string>
#include <functional>
#include <sys/epoll.h>


class HttpServer {
		
	public: 
		HttpServer(int port);
		~HttpServer();
		void run();
		void stop();
		void on_request(std::function<void(int, const std::string&)> handler);

	private:
		int server_fd;
		int epoll_fd;
		int port;
		bool running;
		void setup_server();
		void handle_connection(int client_fd);

};
















