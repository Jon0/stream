#include <iostream>
#include <sstream>
#include <vector>

#include "parser.h"

namespace io {

std::vector<std::string> split(const std::string &str, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

request::request(const char *request_data, int length) {
	auto lines = split(std::string(request_data, length), '\n');

	bool read_data = false;
	for (auto &line : lines) {
		if (read_data) {
			// read request data
			auto items = split(line, '&');
			for (auto &item : items) {
				auto key_value = split(item, '=');
				if (key_value.size() >= 2) {
					this->data[key_value[0]] = key_value[1];
				}
			}
		}
		else {
			// read http header
			auto items = split(line, ' ');
			if (items.size() > 1 && request_str.count(items[0]) > 0) {
				this->type = request_str.at(items[0]);
				this->location = items[1];
				std::cout << items[0] << " request for " << this->location << std::endl;
			}
			else if (items.size() == 1) {
				// end of http header
				read_data = true;
			}
		}
	}

	// print data
	for (auto &item : this->data) {
		std::cout << item.first << " => " << item.second << std::endl;
	}
}

} // namespace io