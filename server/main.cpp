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

		io::server s(io_service, std::atoi(argv[1]), std::string(argv[2]));

		io::stream stream(s);
		s.update_callback([&](io::str_map data) {
			stream.update(data);
		});
		auto t = stream.run(); // non-blocking

		// todo: use multiple run threads
		io_service.run();

		t.join();

	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}