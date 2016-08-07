#ifndef _INSTRUMENT_DATA_H_
#define _INSTRUMENT_DATA_H_

#include "types.hpp"

struct InstrumentData {
    int num_oscs;
    float osc_pitches[4];
    int osc_shapes[4];
    float filter_cutoff;
    float filter_decay;
    float filter_feedback;
    float amp_init;
    float amp_decay;
    float pitch;
    float pitch_decay;
    float pitch_min;
    float gain;
    int out_bus;
};

#endif // _INSTRUMENT_DATA_H_
