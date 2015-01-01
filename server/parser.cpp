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

request::request(const char *data, int length) {
	auto lines = split(std::string(data, length), '\n');

	for (auto &line : lines) {
		auto items = split(line, ' ');
		if (items.size() > 1 && items[0] == "GET") {
			this->location = items[1];
			std::cout << "get request for " << this->location << std::endl;
		}
	}
}

} // namespace server