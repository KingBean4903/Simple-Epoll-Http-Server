#include "server.hpp"

int main() { 
	
	Epoll_Server server(8600);
	server.run();

	return 0;
}
