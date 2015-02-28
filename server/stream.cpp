#include <stdlib.h>

#include "stream.h"

namespace io {

double randf(double scale) {
	return scale * ((rand() % 20000) - 10000) / 10000.0;
}

std::thread stream::start_thread() {
	return std::thread([this]() {
		std::vector<stream_object> objs;
		for (unsigned int i = 0; i < 3; ++i) {
			objs.push_back(stream_object{i});
		}
		while (true) {
			std::string update;
			for (auto &o : objs) {
				o.du += randf(0.05);
				o.dv += randf(0.05);
				o.dw += randf(0.05);
				o.dx += randf(0.05);
				o.dy += randf(0.05);
				o.dz += randf(0.05);
				o.u += o.du;
				o.v += o.dv;
				o.w += o.dw;
				o.x += o.dx;
				o.y += o.dy;
				o.z += o.dz;
				update += std::to_string(o)+" ";
			}

			serv.broadcast(update);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}	
	});
}

void stream::update(http::str_map &data) {
	// todo
} 

} // namespace io

namespace std {

string to_string(const io::stream_object &o) {
	return to_string(o.id) + " " + to_string(o.u) + " " + to_string(o.v) + " " + to_string(o.w);
}

}
