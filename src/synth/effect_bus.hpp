#ifndef _EFFECT_BUS_H_
#define _EFFECT_BUS_H_
#include "delay.hpp"
#include "compressor.hpp"

#ifdef WITH_JSON_LOADER
#include <json/json.h>
#else
#include "bus_data.hpp"
#endif

class EffectBus {
public:
    Delay delay;
    Compressor compressor;
    sample_t pan = 0.5;
    sample_t gain = 1.0;

    sample_t out[2];

    EffectBus();
#ifdef WITH_JSON_LOADER
    void load(const Json::Value &json);
#else
    void load(const BusData &bus);
#endif
    void run(sample_t input[2]);
};

#endif // _EFFECT_BUS_H_
