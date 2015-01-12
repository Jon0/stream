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
			s.add_update_callback([this](io::str_map data) {
				this->update(data);
			});
		}

	/**
	 * create and return main thread
	 */
	std::thread run() {
		return std::thread([this]() {
			while (true) {
				rotation += 0.02;
				serv.broadcast("rotate "+std::to_string(rotation));
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			}	
		});
	}

	/**
	 * update state of stream
	 */
	void update(str_map &data) {
		rotation -= 1.0f;
	}

private:
	server &serv;

	float rotation = 0.0f;
};

} // namespace io

#endif
