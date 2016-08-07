#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

#include "types.hpp"

namespace GlobalConfig {
    static const unsigned int max_oscs_per_voice = 4;
    static const sample_t envelope_min = 1e-6;
};

#endif // _GLOBAL_CONFIG_H_
