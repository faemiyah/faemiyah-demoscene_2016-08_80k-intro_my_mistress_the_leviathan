#include "effect_bus.hpp"

EffectBus::EffectBus() {
    out[0] = out[1] = 0.0f;
}

#ifdef WITH_JSON_LOADER
void EffectBus::load(const Json::Value &json) {
    delay.load(json["delay"]);
    compressor.load(json["compressor"]);
    pan = json["pan"].asDouble();
    gain = json["gain"].asDouble() * 2;
}
#else
void EffectBus::load(const BusData &bus) {
    delay.load(bus);
    compressor.load(bus);
    pan = bus.bus_pan;
    gain = bus.bus_gain * 2;
}
#endif

void EffectBus::run(sample_t input[2]) {
    delay.run(input);
    compressor.run(delay.out);
    out[0] = compressor.out[0] * (1 - pan) * gain;
    out[1] = compressor.out[1] * pan * gain;
}
