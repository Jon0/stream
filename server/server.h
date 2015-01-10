#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "session.h"

using boost::asio::ip::tcp;

namespace io {

class server {
public:
	server(boost::asio::io_service &ios, short port, std::string root)
		:
		io_service(ios),
		root_dir(root),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		context_(io_service, boost::asio::ssl::context::sslv23) {

		context_.set_options(boost::asio::ssl::context::default_workarounds
								| boost::asio::ssl::context::no_sslv2
								| boost::asio::ssl::context::single_dh_use);
		context_.set_password_callback(
			[](std::size_t max_length,  boost::asio::ssl::context::password_purpose purpose) -> std::string {
				return "test"; // very secure
			});
		context_.use_certificate_chain_file("server.pem");
		context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
		context_.use_tmp_dh_file("dh512.pem");

		// start accepting requests
		do_accept();
	}


	void broadcast(std::string message) {
		// send to each session
		for (auto &s : sessions) {
			s->send(message);
		}
	}

	// todo have array for multile callbacks
	void add_update_callback(std::function<void(str_map)> func) {
		this->update_function = func;
	}

private:
	boost::asio::io_service &io_service;

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
	boost::asio::ssl::context context_;


	// todo automatically remove completed sessions
	std::vector<std::shared_ptr<session>> sessions;
};

} // namespace io

#endif