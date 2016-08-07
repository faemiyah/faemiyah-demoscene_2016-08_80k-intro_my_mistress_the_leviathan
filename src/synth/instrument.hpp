#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_
#include <map>
#include <string>
#include <istream>
#include "types.hpp"
#include "delay.hpp"
#include "compressor.hpp"
#include "globalconfig.hpp"

#ifndef WITH_JSON_LOADER
#include "instrument_data.hpp"
#endif

class Instrument {
public:
    enum ParameterType {
	PARAM_VOLUME,
	PARAM_AMP_INIT,
	PARAM_AMP_DECAY,

	PARAM_FILTER_CUTOFF,
	PARAM_FILTER_DECAY,
	PARAM_FILTER_FEEDBACK,

	PARAM_PITCH,
	PARAM_PITCH_DECAY,
	PARAM_PITCH_FLOOR,

	NUM_PARAMS
    };

    struct Parameter {
	ParameterType type;
	float default_value;
    };
    
#ifdef WITH_JSON_LOADER
    // Map parameter names to parameters (for patch loader).
    const std::map<std::string, Parameter> name_param_map {
	{"gain", {PARAM_VOLUME, 1.0} },
	    
	{"filter_cutoff", {PARAM_FILTER_CUTOFF, 1.0} },
	{"filter_decay", {PARAM_FILTER_DECAY, 1.0} },
        {"filter_feedback", {PARAM_FILTER_FEEDBACK, 0.0} },

	{"amp_init", {PARAM_AMP_INIT, 1.0} },
        {"amp_decay", {PARAM_AMP_DECAY, 1.0} },
	   
	{"pitch", {PARAM_PITCH, 1.0} },
	{"pitch_decay", {PARAM_PITCH_DECAY, 1.0} },
	{"pitch_min", {PARAM_PITCH_FLOOR, 0.0} },
    };
#endif // WITH_JSON_LOADER

    Vector<float> params;

    enum OscShape {SAW, TRIANGLE, NOISE, SQUARE1, SQUARE2};
    Vector<OscShape> osc_shapes;
    Vector<float> osc_pitches;

    Instrument();
#ifdef WITH_JSON_LOADER
    static OscShape str_to_shape(const std::string &osc_shape_str);
    void load(std::istream &in_stream);
#else
    void load(const InstrumentData &data);
#endif
};

#endif // _INSTRUMENT_H_
