#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <boost/asio.hpp>

#include "session.h"

using boost::asio::ip::tcp;

namespace io {

class session_list {
public:
	void push(std::string message) {
		// send to each session
	}

private:

	// todo automatically remove completed sessions
	std::vector<std::shared_ptr<session>> sessions;
};

class server {
public:
	server(boost::asio::io_service &io_service, short port)
		: 
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service) {

		boost::system::error_code ec;
		//socket_.set_option( tcp::socket::keep_alive( true ), ec );
		do_accept();
	}

private:

	/*
	 * non-blocking function to accept new connections
	 */
	void do_accept() {
		std::cout << "wait for connection" << std::endl;
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "start session" << std::endl;
					std::make_shared<session>(std::move(socket_))->start();
				}
				do_accept();
			});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

} // namespace io

#endif