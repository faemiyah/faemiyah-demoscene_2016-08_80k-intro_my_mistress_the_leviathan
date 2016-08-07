#include "voice.hpp"
#include "globalconfig.hpp"
#include "rt_controls.hpp"
#include "utils.hpp"
#include <algorithm>

static const uint32_t notetable[13] = {
    0,
    0x4c5504 * 4,
    0x50defc * 4,
    0x55ae0c * 4,
    0x5ac650 * 4,
    0x602c23 * 4,
    0x65e420 * 4,
    0x6bf32b * 4,
    0x725e71 * 4,
    0x792b6d * 4, 
    0x805ff0 * 4,
    0x880221 * 4,
    0x901886 * 4,
};

Voice::Voice() {
    osc_ctr.resize(GlobalConfig::max_oscs_per_voice);
    params.resize(Instrument::NUM_PARAMS);
    osc_pitches.resize(GlobalConfig::max_oscs_per_voice);
    osc_shapes.resize(GlobalConfig::max_oscs_per_voice);
    for (size_t i = 0; i < osc_shapes.size(); ++i) {
	osc_shapes[i] = Instrument::OscShape::SAW;
    }
    out[0] = out[1] = 0.0f;
}

void Voice::set_on(int _instrument, int _note, int _octave, const Instrument &_instrument_ref) {
    instrument = _instrument;
    note = _note;
    octave = _octave;
    pressed = true;
    sustained = false;
    std::copy(_instrument_ref.params.begin(), _instrument_ref.params.end(),
	      params.begin());
    std::copy(_instrument_ref.osc_pitches.begin(), _instrument_ref.osc_pitches.end(),
	      osc_pitches.begin());
    std::copy(_instrument_ref.osc_shapes.begin(), _instrument_ref.osc_shapes.end(),
	      osc_shapes.begin());
}

void Voice::set_off() {
    fade_ctr = 300;
}

void Voice::run_modulation(const Vector<Controller> &rt_controls) {
    static const int num_modulations = 3;
    static const int mod_dests[] = {
	Instrument::PARAM_FILTER_CUTOFF,
	Instrument::PARAM_AMP_INIT,
	Instrument::PARAM_PITCH
    };
    static const int mod_sources[] = {
	Instrument::PARAM_FILTER_DECAY,
	Instrument::PARAM_AMP_DECAY,
	Instrument::PARAM_PITCH_DECAY
    };

    for (int i = 0; i < num_modulations; ++i) {
	int src = mod_sources[i];
	int dest = mod_dests[i];
	if (params[dest] < GlobalConfig::envelope_min) {
	    params[dest] = 0.0;
	} else {
	    params[dest] = clamp(params[dest] * params[src],
				 static_cast<sample_t>(0.0),
				 static_cast<sample_t>(1.0));
	}
    }
    
    params[Instrument::PARAM_PITCH] = std::max(params[Instrument::PARAM_PITCH],
					       params[Instrument::PARAM_PITCH_FLOOR]);

    for (size_t i = 0; i < osc_pitches.size(); ++i) {
	osc_increments[i] = ((notetable[note + 1]) *
			     osc_pitches[i] *
			     (1 << octave) / 128 *
			     params[Instrument::PARAM_PITCH]);
	// * rt_controls[RT_PITCH_BEND].get_value());
    }

    const sample_t flt_co = params[Instrument::PARAM_FILTER_CUTOFF];
    flt_modulated_cutoff = flt_co * rt_controls[RT_FILTER_CUTOFF].get_value();
    // Hand tuned for stability with fb <= 1.0
    flt_fb_amount = (params[Instrument::PARAM_FILTER_FEEDBACK] *
		     (flt_modulated_cutoff * 3.296875f - 0.00497436523438f));
}

sample_t Voice::filter(sample_t in) {
    const sample_t feedback = flt_fb_amount * (flt_p1 - flt_p2);
    flt_p1 = (in * flt_modulated_cutoff +
	      flt_p1 * (1 - flt_modulated_cutoff) +
	      feedback +
	      std::numeric_limits<sample_t>::min());
    flt_p2 = (flt_p1 * flt_modulated_cutoff * 2 +
	      flt_p2 * (1 - flt_modulated_cutoff * 2) +
	      std::numeric_limits<sample_t>::min());
    
    return flt_p2;
}

sample_t Voice::oscillator() {
    int64_t osc_out = 0;
    static int32_t rnd = 239823982;
    
    for (size_t i = 0; i < osc_pitches.size(); ++i) {
	if (osc_shapes[i] == Instrument::NOISE) {
	    rnd = (rnd * 23498320) + rnd;
	    osc_out += rnd;
	} else {
	    if (osc_increments[i] != 0) {
		osc_ctr[i] += osc_increments[i];
		int32_t osc_int = (osc_ctr[i] >> 1) - (1 << 30);

		if (osc_shapes[i] == Instrument::SQUARE1) {
		    osc_int &= 0x80000000;
		    osc_int += 0x40000000;
		} else if (osc_shapes[i] == Instrument::SQUARE2) {
		    osc_int &= 0xc0000000;
		}
		osc_out += osc_int;
	    }
	}
    }
    return osc_out * params[Instrument::PARAM_AMP_INIT];
}

void Voice::run(const Vector<Controller> &rt_controls, sample_t *out,
		size_t frame_count) {
    const sample_t scale = params[Instrument::PARAM_VOLUME] / OVERSAMPLE_FACTOR / (sample_t(1LL << 32));
    for (size_t frame = 0; frame < frame_count; ++frame) {
	if (fade_ctr > 0) {
	    --fade_ctr;
	    if (fade_ctr == 0) {
		pressed = false;
		break;
	    }
	}
	sample_t sum = 0.0;
	if (mod_ctr <= 0) {
	    run_modulation(rt_controls);
	    // this must divide the number of frames for a tracker row evenly.
	    mod_ctr = 20;
	}
	--mod_ctr;
	for (unsigned int i = 0; i < OVERSAMPLE_FACTOR; ++i) {
	    sample_t tmp = oscillator();
	    tmp = filter(tmp);
	    sum += tmp;
	}
	sum *= scale;
	if (fade_ctr > 0) {
	    sum *= (fade_ctr / 300.0);
	}
	*(out++) += sum;
	*(out++) += sum;
    }
}
