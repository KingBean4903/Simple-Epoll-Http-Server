#include "server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

constexpr int MAX_EVENTS = 10000;
constexpr int BUFFER_SIZE = 4096;

Epoll_Server::Epoll_Server(int port) {
	setup_server_socket(port);
	epoll_fd = epoll_create(0);
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = socket_listen;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_listen, &ev);
}

Epoll_Server::~Epoll_Server() {
	close(socket_listen);
	close(epoll_fd);
}

void Epoll_Server::setup_server_socket(int port)
{
	socket_listen = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
  bind(socket_listen, (sockaddr*)&addr, sizeof(addr));
	listen(socket_listen, SOMAXCONN);

	std::cout << "Listening to incoming connections ..." << " \n";
}

void Epoll_Server::run() {

	epoll_event events[MAX_EVENTS];

	while (true) {
		int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		for (int i =0; i < n; i++) {
			if (events[i].data.fd == socket_listen) {
					handle_new_connection();
			} else {
				handle_client_data(events[i].data.fd);
			}
			}
	}
}

void Epoll_Server::handle_new_connection()
{
	
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept4(socket_listen, (sockaddr*)&client_addr, &client_len, SOCK_NONBLOCK);
	if (client_fd > 0) {
			epoll_event ev;
			ev.events = EPOLLIN | EPOLLET;
			ev.data.fd = client_fd;
			epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
			client_buffers[client_fd] = "";
	}

}

void Epoll_Server::handle_client_data(int client_fd)
{
	char buffer[BUFFER_SIZE];
	int count = read(client_fd, buffer, BUFFER_SIZE);
	if (count <= 0) {
			remove_client(client_fd);
			return;
	}
	client_buffers[client_fd].append(buffer, count);
	send_response(client_fd, "ACK: " + client_buffers[client_fd]);
	client_buffers[client_fd].clear();
}

void Epoll_Server::remove_client(int client_fd) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
		close(client_fd);
		client_buffers.erase(client_fd);
}

void Epoll_Server::send_response(int client_fd, const std::string& message)
{
	send(client_fd, message.c_str(), message.length(), 0);
}
















