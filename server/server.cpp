#include "server.h"

namespace io {

void server::do_accept() {
	acceptor_.async_accept(socket_,
		[this](boost::system::error_code ec) {
			if (!ec) {
				std::cout << "start session with " << socket_.remote_endpoint().address().to_string() << std::endl;

				// create session and add to list
				auto s = std::make_shared<session>(std::move(socket_), root_dir);
				sessions.push_back(s);
				s->start();
			}
			do_accept();
		});
}

}