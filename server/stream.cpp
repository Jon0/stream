#include <stdlib.h>

#include "stream.h"

namespace io {

double randf(double scale) {
	return scale * ((rand() % 20000) - 10000) / 10000.0;
}

std::thread stream::start_thread() {
	this->update_millisec = 400;

	for (unsigned int i = 0; i < 3; ++i) {
		this->objs.push_back(stream_object{i});
	}
	this->dampen_value = 0.95;
	this->force_value = 1.5;

	return std::thread([this]() {
		while (true) {
			std::string update;
			for (auto &o : this->objs) {
				o.du *= dampen_value;
				o.dv *= dampen_value;
				o.dw *= dampen_value;
				o.dx *= dampen_value;
				o.dy *= dampen_value;
				o.dz *= dampen_value;
				o.u += o.du;
				o.v += o.dv;
				o.w += o.dw;
				o.x += o.dx;
				o.y += o.dy;
				o.z += o.dz;
				update += std::to_string(o)+" ";
			}

			serv.broadcast(update);
			std::this_thread::sleep_for(std::chrono::milliseconds(this->update_millisec));
		}	
	});
}

void stream::update(http::str_map &data) {
	for (auto &o : this->objs) {
		o.du += randf(this->force_value);
		o.dv += randf(this->force_value);
		o.dw += randf(this->force_value);
		o.dx += randf(this->force_value);
		o.dy += randf(this->force_value);
		o.dz += randf(this->force_value);
	}
} 

} // namespace io

namespace std {

string to_string(const io::stream_object &o) {
	return to_string(o.id) + " " + to_string(o.u) + " " + to_string(o.v) + " " + to_string(o.w);
}

}
