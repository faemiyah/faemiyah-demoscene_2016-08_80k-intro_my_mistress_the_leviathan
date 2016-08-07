#ifndef _DCFILTER_H_
#define _DCFILTER_H_

#include "types.hpp"

class DCFilter {
    sample_t x[2];
    sample_t y[2];

public:
    sample_t out[2];

    DCFilter();
    void run(sample_t input[2]);
};

#endif // _DCFILTER_H_
