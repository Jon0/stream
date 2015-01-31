#include "server.h"
#include "session.h"

namespace io {

int session::next_id = 1;

session::session(server &s,
		std::function<void(http::str_map)> &func)
	:
	id(next_id++),
	create_server(s.root_directory()),
	state(session_state::initial),
	root_dir(root),
	update_function(func),
	write_queue(),
	queue_lock(),
	socket_(s.get_io_service(), s.get_context()) {}

session::~session() {
	std::cout << "rm session crash!!" << id << std::endl;
}

void session::end() {

	// end the write thread
	this->state = session_state::stoping;
	this->write_thread.join();

	// close socket
	this->socket().cancel();
	this->state = session_state::stopped;

	// remove from servers list of active clients
	this->create_server.end_session(this);
	std::cout << "end session with " << socket().remote_endpoint().address().to_string() 
			<< " (id: " << id << ")" << std::endl;
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

	std::string file_path = get_location(filename);
	std::string file_type = file_path.substr(file_path.find(".") + 1);
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
	std::ifstream file(root_dir + file_path);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			content += line + newline;
		}
	}
	else {
		// todo 404
		std::cout << "cannot open " << file_path << std::endl;
	}

	std::string header = "";
	header += "HTTP/1.1 200 OK" + newline;
	header += "Content-Type: " + mime_type + newline;
	header += "Cache-Control: no-cache" + newline;
	header += "Content-Length: " + std::to_string(content.size()) + newline;
	header += newline;
	header += content;
	msg(header);
}

void session::write_stream() {
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

void session::start_write_thread() {
	write_thread = std::thread([this]() {
		std::mutex write_lock;
		while (true) {

			// ensure a message on top
			while (this->write_queue.empty()) {

				// if stoping then leave
				if (this->state == session_state::stoping) {
					return;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			// lock modifying the queue
			this->queue_lock.lock();

			// copy the front item to a container
			auto &str = this->write_queue.front();
			std::vector<char> data;
			std::copy(str.begin(), str.end(), back_inserter(data));

			this->write_queue.pop();
			this->queue_lock.unlock();

			// lock sending on the socket
			write_lock.lock();
			boost::asio::async_write(socket_, boost::asio::buffer(data),
				[&write_lock](boost::system::error_code ec, std::size_t transferred) {
					// todo: queue again if failed to send
					//std::cout << "sent reply (" << transferred << " bytes)" << std::endl;

					// unlocked once message is sent
					write_lock.unlock();
				});
		}
	});
}

} // namespace io