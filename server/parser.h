#ifndef PARSER_H
#define PARSER_H

#include <string>

namespace io {

class request {
public:
	request(const char *data, int length);

	std::string location;
};

} // namespace io

#endif