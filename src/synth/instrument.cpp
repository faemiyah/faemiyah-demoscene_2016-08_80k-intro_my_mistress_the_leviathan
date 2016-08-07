#include "instrument.hpp"
#include "globalconfig.hpp"

#ifdef WITH_JSON_LOADER
#include <json/json.h>
#include <cmath>
#endif

Instrument::Instrument() {
    params.resize(NUM_PARAMS);
}

#ifdef WITH_JSON_LOADER
Instrument::OscShape Instrument::str_to_shape(const std::string &osc_shape_str) {
    if (osc_shape_str == "saw") {
	return SAW;
    } else if (osc_shape_str == "square1") {
	return SQUARE1;
    } else if (osc_shape_str == "square2") {
	return SQUARE2;
    } else if (osc_shape_str == "noise") {
	return NOISE;
    } else if (osc_shape_str == "triangle") {
	return TRIANGLE;
    } else {
	throw std::string("unknown osc shape");
    }
}

void Instrument::load(std::istream &in_stream) {
    Json::Reader reader;
    Json::Value parsed;
    if (!reader.parse(in_stream, parsed)) {
	throw std::string("JSON parse failed");
    }
    for (auto &param : name_param_map) {
	double value = parsed.get(param.first, param.second.default_value).asDouble();
	// hack for reduced envelope update interval
	if (param.second.type == PARAM_FILTER_DECAY ||
	    param.second.type == PARAM_AMP_DECAY ||
	    param.second.type == PARAM_PITCH_DECAY) {
	    value = pow(value, 20.0);
	}
	params[param.second.type] = value;
    }
    Json::Value default_osc_shape = parsed["osc_shape"];
    for (auto &osc : parsed["oscillators"]) {
	Json::Value osc_shape = osc["osc_shape"];
	if (osc_shape == Json::Value::null) {
	    osc_shape = default_osc_shape;
	}
	osc_pitches.push_back(osc["pitch"].asDouble());
	osc_shapes.push_back(str_to_shape(osc_shape.asString()));
    }
    osc_pitches.resize(GlobalConfig::max_oscs_per_voice);
}
#else
void Instrument::load(const InstrumentData &data) {
    osc_pitches.resize(GlobalConfig::max_oscs_per_voice);
    osc_shapes.resize(GlobalConfig::max_oscs_per_voice);
    for (int i = 0; i < data.num_oscs; ++i) {
	osc_pitches[i] = data.osc_pitches[i];
	osc_shapes[i] = static_cast<OscShape>(data.osc_shapes[i]);
    }

    params[Instrument::PARAM_FILTER_CUTOFF] = data.filter_cutoff;
    params[Instrument::PARAM_FILTER_DECAY] = data.filter_decay;
    params[Instrument::PARAM_FILTER_FEEDBACK] = data.filter_feedback;

    params[Instrument::PARAM_AMP_INIT] = data.amp_init;
    params[Instrument::PARAM_AMP_DECAY] = data.amp_decay;

    params[Instrument::PARAM_PITCH] = data.pitch;
    params[Instrument::PARAM_PITCH_DECAY] = data.pitch_decay;
    params[Instrument::PARAM_PITCH_FLOOR] = data.pitch_min;

    params[Instrument::PARAM_VOLUME] = data.gain;
}
#endif
