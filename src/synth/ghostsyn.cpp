#include "ghostsyn.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <cstring>

#ifdef WITH_JSON_LOADER
#include <iostream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

const size_t GhostSyn::MIX_BUF_LEN;

GhostSyn::GhostSyn() {
    instruments.resize(MIDI_NUM_CHANNELS);
    instrument_bus_connections.resize(MIDI_NUM_CHANNELS);
    sustain_controls.resize(MIDI_NUM_CHANNELS);
    for (size_t i = 0; i < sustain_controls.size(); ++i) {
	sustain_controls[i] = false;
    }

    rt_controls.resize(MIDI_NUM_CHANNELS);
    for (auto &ctrl_set : rt_controls) {
	ctrl_set.resize(RT_NUM_TYPES);
	ctrl_set[RT_FILTER_CUTOFF] = Controller(Controller::TYPE_LINEAR,
						Controller::RANGE_8BIT,
						0.1, 1.0,
						63, 0.55,
						63);
	ctrl_set[RT_PITCH_BEND] = Controller(Controller::TYPE_LINEAR,
					     Controller::RANGE_14BIT,
					     0.94387, 1.05946,
					     8192, 1.0,
					     8192);
    }
}

GhostSyn::~GhostSyn() {
}

void GhostSyn::set_instrument_active(int instrument, bool state) {
    if (instrument == -1) {
	for (size_t i = 0; i < active_instruments.size(); ++i) {
	    active_instruments[i] = state;
	}
    } else {
	if (instrument >= 0 && static_cast<size_t>(instrument) < active_instruments.size()) {
	    active_instruments[instrument] = state;
	}
    }
}

void GhostSyn::set_bus_active(int bus, bool state) {
    if (bus == -1) {
	for (size_t i = 0; i < active_buses.size(); ++i) {
	    active_buses[i] = state;
	}
    } else {
	if (bus >= 0 && static_cast<size_t>(bus) < active_buses.size()) {
	    active_buses[bus] = state;
	}
    }
}

#ifdef WITH_JSON_LOADER
void GhostSyn::load_session(std::string filename) {
    fs::path session_file_path(filename);
    std::ifstream infile(filename);
    Json::Reader reader;
    Json::Value parsed;
    if (!reader.parse(infile, parsed)) {
	throw std::string("JSON parse failed");
    }

    unsigned int channel_num = 0;
    for (auto &track : parsed["tracks"]) {
	fs::path patch_file_path;
	patch_file_path /= session_file_path.parent_path();
	patch_file_path /= track["patch"].asString();
	std::ifstream instrument_file(patch_file_path.string());
	instruments[channel_num].load(instrument_file);
	instruments[channel_num].params[Instrument::PARAM_VOLUME] *= track["gain"].asDouble();
	int out_bus = track["out_bus"].asInt();
	instrument_bus_connections[channel_num] = out_bus;
	std::cerr << "instrument to bus " << channel_num << " -> " << out_bus << std::endl;
	++channel_num;
    }

    unsigned int bus_idx = 0;
    for (auto &bus_params : parsed["buses"]) {
	EffectBus new_bus;
	new_bus.load(bus_params);
	effect_buses.push_back(new_bus);
	bus_input_buffers.push_back(new Vector<sample_t>(MIX_BUF_LEN * NUM_CHANNELS));
	int out_bus = bus_params["out_bus"].asInt();
	bus_bus_connections.push_back(out_bus);
	std::cerr << "bus connection " << bus_idx << " -> " << out_bus << std::endl;
	++bus_idx;
    }
    active_instruments = Vector<bool>(instruments.size(), true);
    active_buses = Vector<bool>(effect_buses.size(), true);
}
#else
void GhostSyn::load_session(const BusData buses[],
			    size_t num_buses,
			    const InstrumentData instruments_[],
			    size_t num_instruments) {
    unsigned int instrument_idx = 0;
    for (size_t i = 0; i < num_instruments; ++i) {
	const InstrumentData &instrument = instruments_[i];
	instruments[instrument_idx].load(instrument);
	instrument_bus_connections[instrument_idx] = instrument.out_bus;
	// Note: we don't apply per-track gain here, because compile_song.py has already done it.
	++instrument_idx;
    }
    for (size_t i = 0; i < num_buses; ++i) {
	EffectBus new_bus;
	new_bus.load(buses[i]);
	effect_buses.push_back(new_bus);
	active_buses.push_back(true);
	bus_input_buffers.push_back(new Vector<sample_t>(MIX_BUF_LEN * NUM_CHANNELS));
	int out_bus = buses[i].bus_out_dest;
	bus_bus_connections.push_back(out_bus);
    }
    active_instruments.resize(instruments.size());
    for (size_t i = 0; i < instruments.size(); i++) {
	active_instruments[i] = true;
    }
}
#endif

void GhostSyn::handle_note_on(int channel, int midi_note, int velocity) {
    unused(velocity);

#ifdef WITH_JSON_LOADER
    bool triggered = false;

    if (!active_instruments[channel]) {
	return;
    }
#endif
    
    // TODO: note stealing
    Instrument &instr = instruments[channel];
    int note = midi_note % 12;
    int octave = midi_note / 12;
#if 0
    for (auto &voice : voices) {
	// Don't trigger a second copy of a sustained note
	if (voice.sustained && voice.note == note && voice.octave == octave) {
	    return;
	}
    }
#endif
    for (auto &voice : voices) {
	if (!voice.pressed && !voice.sustained) {
	    voice.set_on(channel, note, octave, instr);
#ifdef WITH_JSON_LOADER
	    triggered = true;
#endif
#if 0
	    if (sustain_controls[channel]) {
		voice.sustained = true;
	    }
#endif
	    break;
	}
    }
#ifdef WITH_JSON_LOADER
    if (!triggered) {
	std::cerr << "out of poly!" << std::endl;
    }
#endif
}

void GhostSyn::handle_note_off(int channel, int midi_note, int velocity) {
    unused(velocity);
    int note = midi_note % 12;
    int octave = midi_note / 12;
    for (auto &voice : voices) {
	if (voice.pressed &&
	    (midi_note < 0 || (voice.note == note &&
	      voice.octave == octave)) &&
	    (voice.instrument == channel || channel == -1)) {
	    voice.set_off();
	}
    }
}

void GhostSyn::handle_control_change(int channel, int control, int value) {
    value = clamp_i(value, 0, 127);
    switch (control) {
    case 0x01:
	rt_controls[channel][RT_FILTER_CUTOFF].update(0, value); // TODO: timestamp in
	break;
    case 0x40:
	if (value >= 64) {
	    sustain_controls[channel] = true;
	    for (auto &voice : voices) {
		if (voice.pressed && voice.instrument == channel) {
		    voice.sustained = true;
		}
	    }
	} else {
	    sustain_controls[channel] = false;
	    for (auto &voice : voices) {
		if (voice.instrument == channel) {
		    voice.sustained = false;
		}
	    }
	}
	break;
    }
}

void GhostSyn::zero_bus_inputs(size_t frame_count) {
    for (auto buffer : bus_input_buffers) {
	memset(buffer->data(), 0, sizeof(sample_t) * frame_count * NUM_CHANNELS);
    }
}

void GhostSyn::zero_master_out(size_t frame_count) {
    memset(master_out, 0, sizeof(sample_t) * frame_count * NUM_CHANNELS);
}

void GhostSyn::process_voices(size_t frame_count) {
    for (auto &voice : voices) {

	if (voice.instrument >= 0 && active_instruments[voice.instrument] &&
	    (voice.pressed || voice.sustained)) {
	    int dest_bus = instrument_bus_connections[voice.instrument];

	    if (dest_bus >= 0 && active_buses[dest_bus]) {
		sample_t *out = bus_input_buffers[dest_bus]->data();
		voice.run(rt_controls[voice.instrument], out, frame_count);
	    }
	}
    }
}

void GhostSyn::process_effect_buses(size_t frame_count) {
    for (size_t source_bus = 0; source_bus < bus_bus_connections.size(); ++source_bus) {

	int dest_bus = bus_bus_connections[source_bus];
	sample_t *in = bus_input_buffers[source_bus]->data();
	sample_t *out;

	if (!active_buses[source_bus] || (dest_bus >= 0 && !active_buses[dest_bus])) {
	    continue;
	}

	if (dest_bus == -1) {
	    out = master_out;
	} else {
	    out = bus_input_buffers[dest_bus]->data();
	}
	    
	for (size_t frame = 0; frame < frame_count; ++frame) {
	    effect_buses[source_bus].run(in);
	    out[0] += effect_buses[source_bus].out[0];
	    out[1] += effect_buses[source_bus].out[1];
	    in += 2;
	    out += 2;
	}
    }
}

void GhostSyn::render(float *out_buf[2], uint32_t offset, uint32_t sample_count) {
    while (sample_count > 0) {
	size_t frames_to_render = std::min(MIX_BUF_LEN, static_cast<size_t>(sample_count));

	zero_master_out(frames_to_render);
	zero_bus_inputs(frames_to_render);
	process_voices(frames_to_render);
	process_effect_buses(frames_to_render);

	for (size_t frame = 0; frame < frames_to_render; ++frame) {
	    out_buf[0][frame + offset] = clamp_f(master_out[frame * 2],
						 static_cast<sample_t>(-1.0),
						 static_cast<sample_t>(1.0));
	    out_buf[1][frame + offset] = clamp_f(master_out[frame * 2 + 1],
						 static_cast<sample_t>(-1.0),
						 static_cast<sample_t>(1.0));
	}

	sample_count -= frames_to_render;
	offset += frames_to_render;
    }
}

void GhostSyn::render_interleaved(float *out_buf, uint32_t frame_count) {
    while (frame_count > 0) {
	size_t frames_to_render = std::min(MIX_BUF_LEN, static_cast<size_t>(frame_count));

	zero_master_out(frames_to_render);
	zero_bus_inputs(frames_to_render);
	process_voices(frames_to_render);
	process_effect_buses(frames_to_render);

	for (size_t frame = 0; frame < frames_to_render * 2; ++frame) {
	    *(out_buf++) = clamp_f(master_out[frame],
				   static_cast<sample_t>(-1.0),
				   static_cast<sample_t>(1.0));
	}

	frame_count -= frames_to_render;
    }
}
