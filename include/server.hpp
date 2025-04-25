#ifndef EPOLL_SERVER_HPP
#define EPOLL_SERVER_HPP

#include <string>
#include <functional>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unordered_map>
#include <string>
#include <vector>

#define SOCKET int

class Epoll_Server {
		
	public: 
		Epoll_Server(int port);
		~Epoll_Server();
		void run();

	private:
		SOCKET socket_listen;
		int epoll_fd;

		std::unordered_map<int, std::string> client_buffers;
		void handle_new_connection();
		void setup_server_socket(int port);
		void handle_client_data(int client_fd);
		void remove_client(int client_fd);
		void send_response(int client_fd, const std::string& message);

};

#endif

















