#ifndef _DELAY_H_
#define _DELAY_H_

#include "types.hpp"
#include <cstdlib>

#ifdef WITH_JSON_LOADER
//#warning using standard C++ vector
#include <json/json.h>
#else
#include "bus_data.hpp"
#endif

class Delay {
    static const size_t delay_length_max = 44100;
    sample_t buffer[2 * delay_length_max];
    size_t buffer_pos[2];

public:
    size_t delay[2];
    sample_t feedback = 0.3;

    sample_t dry_gain = 0.5;
    sample_t wet_gain = 0.5;

    sample_t out[2];

    Delay();
#ifdef WITH_JSON_LOADER
    void load(const Json::Value &json);
#else
    void load(const BusData &bus);
#endif
    void run(sample_t input[2]);
};

#endif // _DELAY_H_
