#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "parser.h"

namespace io {

using boost::asio::ip::tcp;

const std::string newline = "\r\n";

/**
 * tcp connection with a single client
 */
class session : public std::enable_shared_from_this<session> {
public:

	/**
	 * create session with socket and root directory of served files
	 * and a callback function for any updates from the client
	 */
	session(tcp::socket socket, std::string root, std::function<void(str_map)> &func)
		:
		active(false),
		root_dir(root),
		update_function(func),
		socket_(std::move(socket)) {}

	~session() {
		std::cout << "session ended" << std::endl;
	}

	/**
	 * start responding to http requests
	 */
	void start() {
		do_read();
	}

	/**
	 * write to the out stream when it is active
	 */
	void send(std::string in) {
		if (active) {
			std::string content = "data: " + in + newline;
			content += newline;
			msg(content);
		}
	}

private:

	/**
	 * read incoming http requests
	 */
	void do_read() {
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
				std::cout << "request recieved (" << length << " bytes)" << std::endl;
				if (!ec) {
					io::request request(data, length);
					if (request.location == "/stream") {
						write_stream(length);
						this->active = true;
					}
					else if (request.type == request_type::http_get) {
						write_page(get_location(request.location));
					}
					else if (request.type == request_type::http_post) {
						if (update_function) {
							update_function(request.data);
						}
						write_string("wot m8");
					}

					// read next request in same session
					do_read();
				}
				else {
					std::cout << "error" << std::endl;
				}
			});
	}

	/**
	 * write a string with an http header to socket
	 */
	void write_string(const std::string &str);

	/**
	 * write a file with an http header to socket
	 */
	void write_page(const std::string &filename);

	/**
	 * write stream http header to socket
	 */
	void write_stream(std::size_t length);

	/**
	 * async socket writing function
	 */
	void msg(std::string s, bool read=false) {
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(s.c_str(), s.size()),
			[this, self, read](boost::system::error_code ec, std::size_t transferred) {
				std::cout << "sent reply (" << transferred << " bytes)" << std::endl;
			});
	}

	/**
	 * map requested locations to files
	 */
	std::string get_location(std::string in_location) {
		if (in_location == "/") {
			return "/index.html";
		}
		else {
			return in_location;
		}
	}

	/**
	 * should content be pushed
	 */
	bool active;

	/**
	 * root directory for the web server
	 */
	std::string root_dir;

	std::function<void(str_map)> &update_function;

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data [max_length];
};

} // namespace io

#endif