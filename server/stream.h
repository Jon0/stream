#ifndef STREAM_H
#define STREAM_H 

#include <iostream>
#include <chrono>
#include <thread>

#include "server.h"

namespace io {

struct stream_object {
	unsigned int id;
	double u, v, w, x, y, z;
	double du, dv, dw, dx, dy, dz;
};

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
	std::thread start_thread();

	/**
	 * update state of stream
	 */
	void update(http::str_map &data);

private:
	server &serv;

};

} // namespace io

namespace std {

string to_string(const io::stream_object &);

}

#endif
