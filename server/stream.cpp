#include "stream.h"

namespace io {

std::thread stream::start_thread() {
	return std::thread([this]() {
		while (true) {
			rotation += 0.02;
			serv.broadcast("rotate "+std::to_string(rotation));
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		}	
	});
}

void stream::update(http::str_map &data) {
	rotation -= 1.0f;
} 

} // namespace io
