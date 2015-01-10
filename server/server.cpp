#include "server.h"

namespace io {

void server::do_accept() {
	auto s = std::make_shared<session>(io_service, context_, root_dir, update_function);

	acceptor_.async_accept(s->socket(),
		[this, s](boost::system::error_code ec) {
			if (!ec) {

				// start session and add to list
				sessions.push_back(s);
				s->start();
			}
			do_accept();
		});
}

}