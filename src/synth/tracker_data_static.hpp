#ifndef _TRACKER_DATA_STATIC_H_
#define _TRACKER_DATA_STATIC_H_

#include "types.hpp"

struct TrackerCell {
    int note;
    int octave;
    int instrument;
    int fx;
    int param;
};

struct TrackerRow {
    TrackerCell cells[8];
};

struct TrackerPattern {
    TrackerRow rows[96];
};

#endif // _TRACKER_DATA_STATIC_H_
