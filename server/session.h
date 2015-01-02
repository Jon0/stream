#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "parser.h"

namespace io {

using boost::asio::ip::tcp;

const std::string newline = "\r\n";

class session : public std::enable_shared_from_this<session> {
public:
	session(tcp::socket socket)
		:
		root_dir("/home/asdf/git/stream/client"),
		socket_(std::move(socket)) {}

	~session() {
		std::cout << "session ended" << std::endl;
	}

	void start() {
		do_read();
	}

private:
	void do_read() {
		auto self(shared_from_this());
		std::cout << "wait for request\n" << socket_.available() << std::endl;
		socket_.async_read_some(boost::asio::buffer(data, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
				std::cout << "request recieved (" << length << " bytes)" << std::endl;
				if (!ec) {
					io::request request(data, length);
					if (request.location == "/") {
						write_page("/index.html");
					}
					else if (request.location == "/stream") {
						write_stream(length);
					}
					else {
						write_page(request.location);
					}

					// read next request in same session
					do_read();
				}
				else {
					std::cout << "error" << std::endl;
				}
			});
	}

	void write_page(const std::string &filename) {
		std::string content = "";

		std::size_t ind = filename.find(".");
		std::string file_type = filename.substr(ind + 1);
		std::string mime_type;

		// todo use mapping
		if (file_type == "html") {
			mime_type = "text/html";
		}
		else if (file_type == "js") {
			mime_type = "text/javascript";
		}
		else {
			mime_type = "text/plain";
		}
		std::cout << "extension = " << file_type << " mime type = " << mime_type << std::endl;


		// open and read file lines
		std::ifstream file(root_dir + filename);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				content += line + newline;
			}
		}
		else {
			// todo 404
			std::cout << "cannot open " << root_dir << filename << std::endl;
		}
		content += newline;

		std::string header = "";
		header += "HTTP/1.1 200 OK" + newline;
		header += "Content-Type: " + mime_type + newline;
		header += "Cache-Control: no-cache" + newline;
		header += "Content-Length: " + std::to_string(content.size()) + newline;
		header += newline;
		header += content;
		msg(header);
	}

	void write_stream(std::size_t length) {
		std::string header = "";
		header += "HTTP/1.1 200 OK" + newline;
		header += "Content-Type: text/event-stream" + newline;
		//header += "Transfer-Encoding: chunked" + newline;
		header += "Connection: keep-alive" + newline;
		header += "Cache-Control: no-cache" + newline;
		header += "retry: 15000" + newline;
		header += newline;
		msg(header); // use blocking write here?

		while (true) {
			std::string in;
			getline(std::cin, in); // read a whole line
			std::string content = "data: " + in + newline;
			content += newline;
			msg(content);
		}
	}

	void msg(std::string s, bool read=false) {
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(s.c_str(), s.size()),
			[this, self, read](boost::system::error_code ec, std::size_t transferred) {
				std::cout << "sent reply (" << transferred << " bytes)" << std::endl;
			});
	}

	std::string root_dir;
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data [max_length];
};

} // namespace io

#endif