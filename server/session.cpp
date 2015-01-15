#include "server.h"
#include "session.h"

namespace io {

int session::next_id = 1;

session::session(server &s, 
		std::string root, 
		std::function<void(str_map)> &func)
	:
	id(next_id++),
	create_server(s),
	state(session_state::starting),
	root_dir(root),
	update_function(func),
	write_queue(),
	queue_lock(),
	socket_(s.get_io_service(), s.get_context()) {

	write_thread = std::thread([this]() {
		std::mutex write_lock;
		while (true) {

			// ensure a locked state
			this->queue_lock.lock();
			if (this->write_queue.empty()) {

				// wait for unlock elsewhere
				this->queue_lock.lock();
			}

			// if stoping then leave
			if (this->state == session_state::stoping) {
				return;
			}

			write_lock.lock();
			std::string &s = this->write_queue.front();
			boost::asio::async_write(socket_, boost::asio::buffer(s.c_str(), s.size()),
				[this, &write_lock](boost::system::error_code ec, std::size_t transferred) {
					write_lock.unlock();
					//std::cout << "sent reply (" << transferred << " bytes)" << std::endl;
				});
			this->write_queue.pop();
			this->queue_lock.unlock();
		}
	});
}

session::~session() {
	std::cout << "session " << id << " ended" << std::endl;
}

void session::end() {

	// end the write thread
	this->state = session_state::stoping;
	this->queue_lock.unlock();
	this->write_thread.join();

	// close socket
	this->socket().cancel();
	this->state = session_state::stopped;
	this->create_server.end_session(this);
	std::cout << "session " << id << " stopped" << std::endl;
}

void session::write_string(const std::string &str) {
	std::string header = "";
	header += "HTTP/1.1 200 OK" + newline;
	header += "Content-Type: text/plain" + newline;
	header += "Cache-Control: no-cache" + newline;
	header += "Content-Length: " + std::to_string(str.size()) + newline;
	header += newline;
	header += str;
	msg(header);
}

void session::write_page(const std::string &filename) {
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

	std::string header = "";
	header += "HTTP/1.1 200 OK" + newline;
	header += "Content-Type: " + mime_type + newline;
	header += "Cache-Control: no-cache" + newline;
	header += "Content-Length: " + std::to_string(content.size()) + newline;
	header += newline;
	msg(header);

	// todo define in header
	int i = 0;
	int blocksize = 65536;
	while (i < content.size()) {
		msg(content.substr(i, blocksize));
		i += blocksize;
	}
	
}

void session::write_stream(std::size_t length) {
	std::string header = "";
	header += "HTTP/1.1 200 OK" + newline;
	header += "Content-Type: text/event-stream" + newline;
	//header += "Transfer-Encoding: chunked" + newline;
	header += "Connection: keep-alive" + newline;
	header += "Cache-Control: no-cache" + newline;
	header += "retry: 15000" + newline;
	header += newline;
	msg(header); // use blocking write here?
}

} // namespace io