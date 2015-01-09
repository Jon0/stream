#include "session.h"

namespace io {

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

	int i = 0;
	int blocksize = 4096;
	while (i < content.size()) {
		msg(content.substr(i, blocksize));
		i += blocksize;
	}
	//header += content;
	
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