// asio async server

#include <boost/asio.hpp>

#include "server.h"

int main(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			std::cerr << "Usage: server <port> <root directory>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		io::server s(io_service, std::atoi(argv[1]), std::string(argv[2]));

		io_service.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}