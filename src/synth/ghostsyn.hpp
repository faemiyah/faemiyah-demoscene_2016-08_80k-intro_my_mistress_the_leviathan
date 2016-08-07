#ifndef _GHOSTSYN_H_
#define _GHOSTSYN_H_

#include "types.hpp"
#include "compressor.hpp"
#include "dcfilter.hpp"
#include "delay.hpp"
#include "controller.hpp"
#include "instrument.hpp"
#include "voice.hpp"
#include "rt_controls.hpp"
#include "effect_bus.hpp"
#include <stdint.h>
#include <string>

#ifndef WITH_JSON_LOADER
#include "instrument_data.hpp"
#include "bus_data.hpp"
#endif

class GhostSyn {
private:
    static const unsigned int POLYPHONY = 16;

    static const int MIDI_STATUS_MASK_EVENT = 0xf0;
    static const int MIDI_EV_NOTE_OFF = 0x80;
    static const int MIDI_EV_NOTE_ON = 0x90;
    static const int MIDI_EV_POLY_AFTERTOUCH = 0xa0;
    static const int MIDI_EV_CC = 0xb0;
    static const int MIDI_EV_AFTERTOUCH = 0xd0;
    static const int MIDI_EV_PITCH_BEND = 0xe0;
    
    static const int MIDI_STATUS_MASK_CHANNEL = 0x0f;

    static const size_t MIX_BUF_LEN = 8192;
    static const unsigned int NUM_CHANNELS = 2;

    Vector<Instrument> instruments;
    Vector<unsigned int> instrument_bus_connections;
    Voice voices[POLYPHONY];

    // By default all instruments and buses are processed. These can
    // be used to toggle off some/all.
    Vector<bool> active_instruments;
    Vector<bool> active_buses;
    // Force this bus to be the output bus. If OUT_BUS_DEFAULT, use the
    // output bus specified in session file.
    int force_out_bus = GhostSyn::OUT_BUS_DEFAULT;

    // Realtime performance controls, one set per MIDI channel. These affect
    // all notes playing on a channel, and do not modify instruments.
    typedef Vector<Controller> RtControlSet;
    Vector<RtControlSet> rt_controls;
    Vector<bool> sustain_controls;

    Vector<EffectBus> effect_buses;
    Vector<int> bus_bus_connections;
    Vector<Vector<sample_t> *> bus_input_buffers;

    sample_t master_out[MIX_BUF_LEN * NUM_CHANNELS];

    void zero_bus_inputs(size_t frame_count);
    void zero_master_out(size_t frame_count);
    void process_voices(size_t frame_count);
    void process_effect_buses(size_t frame_count);
    
public:
    GhostSyn();
    ~GhostSyn();

    void set_instrument_active(int instrument, bool state);
    void set_bus_active(int bus, bool state);
    void set_force_out_bus(int bus);
    static const int OUT_BUS_DEFAULT = -1;

#ifdef WITH_JSON_LOADER
    void load_session(std::string filename);
#else
    void load_session(const BusData buses[],
		      size_t num_buses,
		      const InstrumentData instruments[],
		      size_t num_instruments);
#endif

    void render(float *out_buf[2], uint32_t offset, uint32_t sample_count);
    void render_interleaved(float *out_buf, uint32_t sample_count);

    void handle_note_on(int channel, int midi_note, int velocity);
    void handle_note_off(int channel, int midi_note, int velocity);
    void handle_control_change(int channel, int control, int value);
    void handle_pitch_bend(int channel, int value_1, int value_2);
    static const int MIDI_NUM_CHANNELS = 16;
};

#endif // _GHOSTSYN_H_
