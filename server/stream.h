#ifndef STREAM_H
#define STREAM_H 

#include <iostream>
#include <chrono>
#include <thread>

#include "server.h"

namespace io {

/**
 * updates the state of some 3d objects and dispatches to clients
 */
class stream {
public:
	stream(server &s)
		:
		serv(s) {
			s.add_update_callback([this](http::str_map data) {
				this->update(data);
			});
		}

	/**
	 * create and return main thread
	 */
	std::thread run();

	/**
	 * update state of stream
	 */
	void update(http::str_map &data);

private:
	server &serv;

	float rotation = 0.0f;
};

} // namespace io

#endif
