#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "parser.h"

namespace io {

using boost::asio::ip::tcp;

using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

const std::string newline = "\r\n";

class server;

enum class session_state {
	starting,
	idle,
	streaming,
	stoping,
	stopped
};

/**
 * tcp connection with a single client
 */
class session : public std::enable_shared_from_this<session> {
public:

	/**
	 * create session with socket and root directory of served files
	 * and a callback function for any updates from the client
	 */
	session(server &s,
			std::string root,
			std::function<void(str_map)> &func);

	~session();

	ssl_socket::lowest_layer_type &socket() {
		return socket_.lowest_layer();
	}

	/**
	 * start responding to http requests
	 */
	void start() {
		std::cout << "start session " << id << " with " << socket().remote_endpoint().address().to_string() << std::endl;

		socket_.async_handshake(boost::asio::ssl::stream_base::server,
			[this](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "handshake success" << std::endl;
					this->state = session_state::idle;
					start_write_thread();
					do_read();
				}
				else {
					std::cout << "handshake failure" << std::endl;
					end();
				}
			});
	}

	/**
	 * write to the out stream when it is active
	 */
	void send(std::string in) {
		if (this->state == session_state::streaming) {
			std::string content = "data: " + in + newline;
			content += newline;
			msg(content);
		}
	}

	void end();

private:

	/**
	 * read incoming http requests
	 */
	void do_read() {
		socket_.async_read_some(boost::asio::buffer(data, max_length),
			[this](boost::system::error_code ec, std::size_t length) {

				// debug output
				std::cout << "request recieved (" << length << " bytes)" << std::endl;

				if (!ec) {
					io::request request(data, length);
					if (request.location == "/stream") {

						// write header and set connection to streaming
						write_stream();
						this->state = session_state::streaming;
					}
					else if (request.type == request_type::http_get) {

						// write a page response
						write_page(request.location);
					}
					else if (request.type == request_type::http_post) {

						// respond to post headers
						if (update_function) {
							update_function(request.data);
						}
						write_string("wot m8");
					}

					// read next request in same session
					do_read();
				}
				else if (ec != boost::asio::error::operation_aborted) {
					std::cout << "closing connection " << id << std::endl;
					this->end();
				}
				else {
					std::cout << "operation aborted" << std::endl;
				}
			});
	}

	/**
	 * write a string with an http header to socket
	 */
	void write_string(const std::string &str);

	/**
	 * write a file with an http header to socket, using the requested filename
	 */
	void write_page(const std::string &filename);

	/**
	 * write stream http header to socket
	 */
	void write_stream();

	void start_write_thread();

	/**
	 * async socket writing function
	 */
	void msg(std::string s) {
		this->queue_lock.lock();
		this->write_queue.push(s);
		this->queue_lock.unlock();
	}

	/**
	 * map requested locations to actual files
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
	 * unique id for the session
	 */
	static int next_id;
	int id;

	/**
	 * server which created this
	 */
	server &create_server;

	/**
	 * current stat of connection
	 */
	session_state state;

	/**
	 * root directory for the web server
	 */
	std::string root_dir;

	std::function<void(str_map)> &update_function;

	/**
	 * only allow one write operation at a time
	 */
	std::thread write_thread;
	std::queue<std::string> write_queue;
	std::mutex queue_lock;

	ssl_socket socket_;
	enum { max_length = 1024 };
	char data [max_length];
};

} // namespace io

#endif