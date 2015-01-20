#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "session.h"

namespace io {

using boost::asio::ip::tcp;
using boost::asio::ssl::context;	

/**
 * communicates with clients via http
 */
class server {
public:
	server(boost::asio::io_service &ios, short port, std::string root)
		:
		io_service(ios),
		root_dir(root),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		context_(io_service, context::sslv23) {

		context_.set_options(context::default_workarounds
								| context::no_sslv2
								| context::single_dh_use);

		// todo: the keys are not yet encrypted
		// context_.set_password_callback(
		// 	[](std::size_t max_length,  context::password_purpose purpose) -> std::string {
		// 		return "test"; // very secure
		// 	});

		context_.use_certificate_chain_file("server.crt");
		context_.use_private_key_file("server.key", context::pem);
		context_.use_tmp_dh_file("dh512.pem");

		// start accepting requests
		do_accept();
	}

	/**
	 * send string to each active streaming session
	 */
	void broadcast(std::string message) {
		for (auto &s : sessions) {
			s->send(message);
		}
	}

	void end_session(session *to_remove) {
		auto position_it = std::find_if(
			std::begin(sessions),
			std::end(sessions),
			[to_remove](std::shared_ptr<session> &e) {
				return e.get() == to_remove;
			});
		sessions.erase(position_it);
	}

	// todo have array for multile callbacks
	void add_update_callback(std::function<void(str_map)> func) {
		this->update_function = func;
	}

	boost::asio::io_service &get_io_service() {
		return io_service;
	}

	context &get_context() {
		return context_;
	}

private:

	/*
	 * non-blocking function to accept new connections
	 */
	void do_accept();

	/**
	 * io service
	 */
	boost::asio::io_service &io_service;

	/**
	 * root directory for web server
	 */
	std::string root_dir;

	/**
	 * callback for revieved updated
	 */
	std::function<void(str_map)> update_function;

	tcp::acceptor acceptor_;
	context context_;


	// todo automatically remove completed sessions
	std::vector<std::shared_ptr<session>> sessions;
};

} // namespace io

#endif