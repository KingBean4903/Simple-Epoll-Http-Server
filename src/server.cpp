#include "http_server.h"
#include <unistd.c>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

HttpServer::HttpServer(int port) : port(port), server_fd(-1),
							epoll_fd(-1), running(false) {}

HttpServer::~HttpServer() {
	if (server_fd != -1) close(server_fd);
	if (epoll_fd != -1) close(epoll_fd);
}

void HttpServer::setup_server() {

	// Create socket
	server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server_fd == -1) {
				throw std::runtime_error("Failed to create socket: " + 
						std::string(strerror(errno)));
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) {
				throw std::runtime_error("Failed to set socket options: " + 
						std::string(strerror(errno)));
	}

	// Bind socket
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*)& address, sizeof(address)) < 0) {
			throw std::runtime_error("Failed to bind socket: " + 
					std::string(strerror(errno));
	}

	if (listen(server_fd, 128) < 0) {

			throw std::runtime_error("Failed to listen on socket: " + 
					std::string(sterror(errno));
	}

	epoll_fd = epoll_create(0);
	if (epoll_fd == -1) {
			throw std::runtime_error("Failed to create epoll instance: " +
					std::string(strerror(errno));
	}

	struct epoll_event event;
	event.events = EPOLL_IN;
	event.data.fd = server_fd;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
	{
		 throw std::runtime_error("Failed to add server socket to epoll: " +
				 std::string(strerror(errno)));
	}
	}

	void httpServer::run() {
		setup_server();
		running = true;

		struct epoll_event events[64];
		std::cout << "Server running on port " << port << std::endl;

		while (running) {
				
				int num_events  = epoll_wait(epoll_fd, events, 64 - 1);
				if (num_events == -1) {
							if (errno == EINTR) continue;
							throw std::runtime_error("epoll_wait failed: " +
									std::string(sterror(errno)));
				}

				for (int  i = 0; i < num_events; i++) {
							if (events[i].data.fd == server_fd) {
									struct sockaddr_in client_addr;
									socklent_t client_len = sizeof(client_addr);
									int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
									if (client_fd == -1) {
											continue;
									}

									// Make client non blocking
									int flags =fcntl(client_fd, FGETFL, 0);
									fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

									struct epoll_event client_event;
									client_event.events = EPOLLIN | EPOLLET;
									client_event.data.fd = client_fd;
									
									if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cliet_fd, &client_event) == -1) {
											close(client_fd);
											continue;
									}
							} else {
									
									handle_connection(events[i].data.fd);
							}
				}
		}
	
	}

	void HttpServer::handle_connection(int client_fd) {
			char buffer[4096];
			ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

			if (bytes_read <= 0) {
					close(client_fd);
					return;
			}

			buffer[bytes_read] = '\0';
			std::string request_str(buffer);

			std::string response = "HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nAutobots, rollout!";
			send(client_fd, response.c_str(), response.size(), 0);
			close(client_fd);
	}

	void HttpServer::stop() {
			running = false;
	}




