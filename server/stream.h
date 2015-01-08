#ifndef STREAM_H
#define STREAM_H 

#include <iostream>
#include <chrono>
#include <thread>

namespace io {

class stream {
public:
	stream(server &s)
		:
		serv(s) {}

	std::thread run() {
		return std::thread([this]() {
			while (true) {
				serv.broadcast("testing");
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			}	
		});
	}

private:
	server &serv;


	
};

} // namespace io

#endif
