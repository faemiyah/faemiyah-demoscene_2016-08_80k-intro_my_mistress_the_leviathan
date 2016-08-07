#include "dcfilter.hpp"

DCFilter::DCFilter() {
    out[0] = out[1] = 0.0f;
    x[0] = x[1] = 0.0f;
    y[0] = y[1] = 0.0f;
}

void DCFilter::run(sample_t input[2]) {
    for (unsigned int i = 0; i < 2; ++i) {
	out[i] = input[i] - x[i] + 0.995 * y[i];
	x[i] = input[i];
	y[i] = out[i];
    }
}
