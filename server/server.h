#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <boost/asio.hpp>

#include "session.h"

using boost::asio::ip::tcp;

namespace io {

class server {
public:
	server(boost::asio::io_service &io_service, short port, std::string root)
		:
		root_dir(root),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service) {

		boost::system::error_code ec;
		socket_.set_option( tcp::socket::keep_alive( true ), ec );
		do_accept();
	}


	void broadcast(std::string message) {
		// send to each session
		for (auto &s : sessions) {
			s->send(message);
		}
	}

	void update_callback(std::function<void(str_map)> func) {
		this->update_function = func;
	}

private:

	/*
	 * non-blocking function to accept new connections
	 */
	void do_accept();

	/**
	 * root directory for web server
	 */
	std::string root_dir;

	/**
	 * callback for revieved updated
	 */
	std::function<void(str_map)> update_function;

	tcp::acceptor acceptor_;
	tcp::socket socket_;

	// todo automatically remove completed sessions
	std::vector<std::shared_ptr<session>> sessions;
};

} // namespace io

#endif