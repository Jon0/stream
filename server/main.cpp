// asio async server

#include <boost/asio.hpp>

#include "server.h"
#include "stream.h"

int main(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			std::cerr << "Usage: server <port> <root directory>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		// server with port and root web directory
		io::server s(io_service, std::atoi(argv[1]), std::string(argv[2]));

		// start the stream system
		io::stream stream(s);
		auto t = stream.start_thread(); // non-blocking

		// todo: use multiple run threads
		io_service.run(); // blocks until exit

		t.join();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}