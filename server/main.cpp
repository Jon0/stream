// asio async server example

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
	session(tcp::socket socket)
		: 
		socket_(std::move(socket)) {}

	void start() {
		do_read();
	}

private:
	void do_read() {
		auto self(shared_from_this());
		std::cout << "wait for input" << std::endl;
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
				if (!ec) {
					std::cout.write(data_, length);
					do_write(length);
				}
			});
	}

	void do_write(std::size_t length) {
		do_read();
		std::string newline = "\r\n";

		std::string content = "data: dsfadsf" + newline;


		std::string header = "";
		header += "HTTP/1.1 200 OK" + newline;
		header += "Content-Type: text/event-stream" + newline;
		//header += "Content-Length: " + std::to_string(content.length()) + newline;
		//header += "Transfer-Encoding: chunked" + newline;
		header += "Connection: keep-alive" + newline;
		header += "Cache-Control: no-cache" + newline;
		header += "retry: 15000" + newline;
		header += newline;
		msg(header);
		
		msg(content);

	}

	void msg(std::string s) {
		std::cout << s;
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(s.c_str(), s.length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {});
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
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