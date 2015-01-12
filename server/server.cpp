#include "server.h"

namespace io {

void server::do_accept() {
	auto s = std::make_shared<session>(*this, root_dir, update_function);

	acceptor_.async_accept(s->socket(),
		[this, s](boost::system::error_code ec) {
			if (!ec) {

				// start session and add to list
				sessions.push_back(s);
				s->start();
				std::cout << "number of sessions: " << sessions.size() << std::endl;
			}
			do_accept();
		});
}

}