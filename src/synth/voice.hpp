#ifndef _VOICE_H_
#define _VOICE_H_
#include "types.hpp"
#include "instrument.hpp"
#include "controller.hpp"
#include "globalconfig.hpp"
#include <cstdint>

class Voice {
    static const unsigned int OVERSAMPLE_FACTOR = 4;

    Vector<uint32_t> osc_ctr;
    // Current voice params. Initialized from instrument on note on,
    // modified by envelopes.
    Vector<sample_t> params;
    Vector<sample_t> osc_pitches;
    int32_t osc_increments[GlobalConfig::max_oscs_per_voice];

    sample_t oscillator();
    sample_t filter(sample_t in);
    void run_modulation(const Vector<Controller> &rt_controls);

    int mod_ctr = 0;
    int fade_ctr = 0; // if > 0, we're doing a quick fade at note cut for this many samples

public:
    int instrument = -1;
    int note = -1;
    int octave = -1;
    bool pressed = false;
    bool sustained = false;

    Vector<Instrument::OscShape> osc_shapes;

    // Filter state for this voice
    sample_t flt_p1 = 0.0;
    sample_t flt_p2 = 0.0;

    // Filter parameters, updated once per modulation update cycle.
    sample_t flt_modulated_cutoff = 0.0;
    sample_t flt_fb_amount = 0.0;

    sample_t out[2];

    Voice();
    void set_on(int _instrument, int _note, int _octave, const Instrument &instrument_ref);
    void set_off();
    void set_param(size_t param_idx, sample_t value);

    void run(const Vector<Controller> &rt_controls, sample_t *out, const size_t frame_count);
};

#endif // _VOICE_H_
