#ifndef _BUS_DATA_H_
#define _BUS_DATA_H_

struct BusData {
    float delay_time_l;
    float delay_time_r;
    float delay_dry_wet_balance;
    float delay_feedback;

    float comp_threshold;
    float comp_attack;
    float comp_release;

    float bus_pan;
    float bus_gain;

    int bus_out_dest;
};

#endif // _BUS_DATA_H_
