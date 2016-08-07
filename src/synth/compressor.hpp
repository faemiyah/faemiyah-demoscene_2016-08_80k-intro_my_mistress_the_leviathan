#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include "types.hpp"

#ifdef WITH_JSON_LOADER
#include <json/json.h>
#else
#include "bus_data.hpp"
#endif

class Compressor {
    sample_t threshold = 0.5;
    sample_t attack = 0.99;
    sample_t release = 1.001;

    sample_t amp = 1.0;

public:
    sample_t out[2];
    Compressor();
#ifdef WITH_JSON_LOADER
    void load(const Json::Value &json);
#else
    void load(const BusData &bus);
#endif
    void run(sample_t input[2]);
};

#endif // _COMPRESSOR_H_
