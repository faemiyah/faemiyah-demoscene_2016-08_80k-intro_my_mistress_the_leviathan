#include "compiled_song.hpp"
#include "ghostsyn.hpp"

#include "SDL_thread.h"

#ifdef WITH_WRITER_MAIN
#include <sndfile.h>
#endif // WITH_WRITER_MAIN

#define USE_THREADS 0

#define SONG_FRAMES (190 * 44100)
#define SONG_SAMPLES (SONG_FRAMES * 2)
#define BUFFER_LENGTH (SONG_SAMPLES * sizeof(float))

static const size_t frames_per_row = 5000;

struct thread_settings {
#if 0
    std::vector<bool> active_instruments;
    std::vector<bool> active_buses;
#endif
    float *out_buf;
    size_t bytes;
};

int generate_audio_thread(void *ctx) {
    struct thread_settings *settings = reinterpret_cast<struct thread_settings *>(ctx);
#if 0
    const std::vector<bool> &active_instruments = settings->active_instruments;
    const std::vector<bool> &active_buses = settings->active_buses;
#endif
    float *dst = static_cast<float *>(settings->out_buf);
    size_t bytes = settings->bytes;

    GhostSyn synth;
    synth.load_session(buses, num_buses, instruments, num_instruments);

#if 0
    for (size_t i = 0; i < active_instruments.size(); ++i) {
	synth.set_instrument_active(i, active_instruments[i]);
    }

    for (size_t i = 0; i < active_buses.size(); ++i) {
	synth.set_bus_active(i, active_buses[i]);
    }
#endif

    int playing_notes[16] = { 0 };
    int playing_instruments[16] = { 0 };
    size_t bytes_written = 0;

    for (int order_pos = 0; order[order_pos] >= 0; ++order_pos) {
	const TrackerPattern &current_pattern = patterns[order[order_pos]];
	for (auto &row : current_pattern.rows) {

	    if (bytes_written + (frames_per_row * 2 * sizeof(float))>= bytes) {
		return 0;
	    }
	    
	    size_t track_idx = 0;
	    for (auto &cell : row.cells) {
		if (cell.note >= 0) {
		    int midi_note = cell.note + cell.octave * 12;
		    if (playing_notes[track_idx] >= 0) {
			synth.handle_note_off(playing_instruments[track_idx],
					      playing_notes[track_idx],
					      127);
			playing_notes[track_idx] = -1;
		    }
		    if (midi_note > 0) {
			synth.handle_note_on(cell.instrument, midi_note, 127);
			playing_instruments[track_idx] = cell.instrument;
			playing_notes[track_idx] = midi_note;
		    }
		}
		switch (cell.fx) {
		case 0x01:
		    synth.handle_control_change(playing_instruments[track_idx],
						0x01, cell.param);
		    break;
		default:
		    break;
		}

		++track_idx;
	    }
	    synth.render_interleaved(dst, frames_per_row);
	    dst += frames_per_row * 2;
	    bytes_written += frames_per_row * 2 * sizeof(float);
	}
    }
    return 0;
}

void generate_audio(void *data, const size_t length) {
    (void)length; // TODO
#if USE_THREADS
    float *master_out = reinterpret_cast<float *>(data);
    struct thread_settings settings[4] = {
#if 1
	{ {true, true, true, true, true, true, true, true, true, true},
	  {true, true, true, true, true, true, true, true} },
	{ {false, false, false, false, false, false, false, false, false, false},
	  {false, false, false, false, false, false, false, false} },
	{ {false, false, false, false, false, false, false, false, false, false},
	  {false, false, false, false, false, false, false, false} },
	{ {false, false, false, false, false, false, false, false, false, false},
	  {false, false, false, false, false, false, false, false} }
#else

	{ {true, true, false, false, false, false, false, false, false, false},
	  {true, true, false, false, false, false, false, false} },
	
	{ {false, false, true, true, false, false, false, false, false, false,},
	  {false, false, true, true, false, false, false, false} },
	
	{ {false, false, false, false, true, true, false, false, false, false},
	{false, false, false, false, true, true, true, true} },
	
	{ {false, false, false, false, false, false, true, true, true, true},
	  {false, false, false, false, false, false, true, true} }
#endif
    };

    SDL_Thread *audio_threads[4];
    
    for (size_t i = 0; i < 4; ++i) {
	settings[i].out_buf = new float[BUFFER_LENGTH];
	audio_threads[i] = SDL_CreateThread(generate_audio_thread, "x", &settings[i]);
    }
    for (size_t i = 0; i < 4; ++i) {
	SDL_WaitThread(audio_threads[i], NULL);
    }
    
    for (size_t i = 0; i < 4; ++i) {
	float *out = master_out;
	float *in = settings[i].out_buf;
	for (size_t k = 0; k < SONG_SAMPLES; ++k) {
	    *(out++) += *(in++);
	}
    }
    // run master effects

#else // USE_THREADS
    struct thread_settings settings[1] = {
	{
#if 0
    {{true, true, true, true, true, true, true, true, true, true},
     {true, true, true, true, true, true, true, true},
#endif
     NULL, 0}};
    settings[0].out_buf = reinterpret_cast<float *>(data);
    settings[0].bytes = length;
    generate_audio_thread(settings);
#endif
}

#ifdef WITH_WRITER_MAIN
int main(int argc, char *argv[]) {
    if (argc != 2) {
	return 1;
    }
    
    SF_INFO info;
    info.samplerate = 44100;
    info.channels = 2;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SNDFILE *outfile = sf_open(argv[1], SFM_WRITE, &info);

    float *buf = new float[BUFFER_LENGTH];
    generate_audio(buf, BUFFER_LENGTH);
    sf_writef_float(outfile, buf, SONG_FRAMES);
    delete [] buf;

    sf_close(outfile);
    return 0;
}
#endif // WITH_WRITER_MAIN
