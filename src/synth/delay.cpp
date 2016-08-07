#include "delay.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>

const size_t Delay::delay_length_max;

Delay::Delay() {
    memset(buffer, 0, 2 * delay_length_max * sizeof(sample_t));
    out[0] = out[1] = 0.0f;
    buffer_pos[0] = buffer_pos[1] = 0.0f;
    delay[0] = delay[1] = 10000;
}

#ifdef WITH_JSON_LOADER
void Delay::load(const Json::Value &json) {
    feedback = json["feedback"].asDouble();
    delay[0] = std::min(delay_length_max,
			static_cast<size_t>(json["time_l"].asDouble() * 44100));
    delay[1] = std::min(delay_length_max,
			static_cast<size_t>(json["time_r"].asDouble() * 44100));
    sample_t dry_wet_balance = json["dry_wet_balance"].asDouble();
    dry_gain = clamp(static_cast<sample_t>(1.0) - dry_wet_balance,
		     static_cast<sample_t>(0.0),
		     static_cast<sample_t>(1.0));
    wet_gain = clamp(dry_wet_balance,
		     static_cast<sample_t>(0.0),
		     static_cast<sample_t>(1.0));
}
#else
void Delay::load(const BusData &bus) {
    feedback = bus.delay_feedback;
    delay[0] = std::min(delay_length_max,
			static_cast<size_t>(bus.delay_time_l * 44100));
    delay[1] = std::min(delay_length_max,
			static_cast<size_t>(bus.delay_time_r * 44100));
    sample_t dry_wet_balance = bus.delay_dry_wet_balance;
    dry_gain = clamp_f(1.0f - dry_wet_balance,
		       static_cast<sample_t>(0.0),
		       static_cast<sample_t>(1.0));
    wet_gain = clamp_f(dry_wet_balance,
		       static_cast<sample_t>(0.0),
		       static_cast<sample_t>(1.0));
}
#endif
   
void Delay::run(sample_t input[2]) {
    if (wet_gain == 0.0) {
	// If wet gain is zero, assume dry gain 1
	out[0] = input[0];
	out[1] = input[1];
    } else {
	for (size_t i = 0; i < 2; i++) {
	    int other = (i ? 0 : 1);
	    buffer[buffer_pos[i] + i * delay_length_max] = (buffer[buffer_pos[other] +
								   other * delay_length_max] * feedback +
							    input[i]);
	    out[i] = (dry_gain * input[i] +
		      wet_gain * buffer[buffer_pos[i] + i * delay_length_max]) + 0.00000001;
	    if (++buffer_pos[i] >= delay[i]) {
		buffer_pos[i] = 0;
	    }
	}
    }
}

