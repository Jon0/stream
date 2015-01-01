// asio async server example

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "parser.h"

const std::string newline = "\r\n";

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
	session(tcp::socket socket)
		:
		root_dir("/home/asdf/git/stream/client/"),
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
		std::cout << "wait for input" << std::endl;
		socket_.async_read_some(boost::asio::buffer(data, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
				std::cout << "request recieved " << length << std::endl;
				if (!ec) {
					io::request request(data, length);
					if (request.location == "/") {
						write_page("index.html");
					}
					else if (request.location == "/stream") {
						write_stream(length);
					}
					else {
						write_page(request.location);
					}
				}
				else {
					std::cout << "error" << std::endl;
				}
			});
		std::cout << "..." << std::endl;
	}

	void write_page(const std::string &filename) {
		std::string content = "";

		std::ifstream file(root_dir + filename);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				content += line + newline;
			}
		}
		else {
			std::cout << "cannot open " << root_dir << filename << std::endl;
		}
		content += newline;

		std::string header = "";
		header += "HTTP/1.1 200 OK" + newline;
		header += "Content-Type: text/html" + newline;
		header += newline;
		msg(header);
		msg(content);
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
		msg(header);

		//while (true) {
			std::string in;
			std::cin >> in;
			std::string content = "data: " + in + newline;
			content += newline;
			msg(content);
		//}
	}

	void msg(std::string s) {
		std::cout << s;
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(s.c_str(), s.length()),
			[this, self](boost::system::error_code ec, std::size_t) {});
	}

	std::string root_dir;
	tcp::socket socket_;
	enum { max_length = 2048 };
	char data [max_length];
};

class server {
public:
	server(boost::asio::io_service& io_service, short port)
		: 
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service) {

		boost::system::error_code ec;
		socket_.set_option( tcp::socket::keep_alive( true ), ec );
		do_accept();
	}

private:
	void do_accept() {
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

int main(int argc, char* argv[]) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: async_tcp_echo_server <port>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		server s(io_service, std::atoi(argv[1]));

		io_service.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}