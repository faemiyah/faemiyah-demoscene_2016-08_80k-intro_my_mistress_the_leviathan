#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "types.hpp"
#include <limits>

class Controller {
public:
    typedef enum {TYPE_LINEAR} Type;
    typedef enum {RANGE_8BIT, RANGE_14BIT} Range;
private:
    unsigned long long prev_timestamp = std::numeric_limits<unsigned long long>::max();

    Type type = TYPE_LINEAR;

    sample_t value_min = 0.0;
    sample_t value_max = 1.0;
    
    int midi_midpoint = 63;
    sample_t value_midpoint = 0.5;
       
    sample_t range_divisor_upper = 127.0;
    sample_t range_divisor_lower = 127.0;

    // Current output value of controller
    sample_t value = 0.0;
    int prev_midi_value = std::numeric_limits<int>::lowest();
public:
    Controller();
    Controller(Type _type, Range _range,
	       sample_t _value_min, sample_t _value_max,
	       int _midi_midpoint, sample_t _value_midpoint,
	       sample_t _initial_midi);
    void update(unsigned long long timestamp, int midi_value);
    sample_t get_value() const { return value; }
};

#endif // _CONTROLLER_H_
