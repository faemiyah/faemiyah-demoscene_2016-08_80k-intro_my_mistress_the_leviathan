#include "compressor.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Compressor::Compressor() {
    out[0] = out[1] = 0.0f;
}

#ifdef WITH_JSON_LOADER
void Compressor::load(const Json::Value &json) {
    threshold = clamp(json["threshold"].asDouble(), 0.0, 1.0);
    attack = clamp(json["attack"].asDouble(), 0.0, 1.0);
    release = std::max(json["release"].asDouble(), 1.0);
}
#else
void Compressor::load(const BusData &bus) {
    threshold = bus.comp_threshold;
    attack = bus.comp_attack;
    release = bus.comp_release;
}
#endif

void Compressor::run(sample_t input[2]) {
    out[0] = clamp_f(input[0] * amp,
		     static_cast<sample_t>(-1.0),
		     static_cast<sample_t>(1.0));
    out[1] = clamp_f(input[1] * amp,
		     static_cast<sample_t>(-1.0),
		     static_cast<sample_t>(1.0));
    if ((fabs(out[0]) + fabs(out[1])) / 2 > threshold) {
	amp *= attack;
    } else {
	amp *= release;
	if (amp > 1.0) {
	    amp = 1.0;
	}
    }
}
