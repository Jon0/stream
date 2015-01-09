#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>

namespace io {

using str_map = std::unordered_map<std::string, std::string>;

enum class request_type {
	http_get,
	http_post,
	http_put,
	http_delete
};

/**
 * parsing an http request header and data
 */
class request {
public:
	request(const char *data, int length);

	request_type type;
	std::string location;
	str_map data;
};

} // namespace io

#endif