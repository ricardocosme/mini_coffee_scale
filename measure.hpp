#pragma once

#include <stdint.h>

#include <hx711.hpp>

struct weight { int32_t min, median, max; };
    
template<typename HX711>
auto measure(HX711& sensor) {
    constexpr uint8_t size{3};
    int32_t samples[size];
    for(uint8_t i{0}; i < size; ++i) 
        samples[i] = hx711::sync_read(sensor);
    for(uint8_t i{0}; i < size - 1; ++i) {
        if(samples[i] > samples[i+1]) {
            auto tmp = samples[i+1];
            samples[i+1] = samples[i];
            samples[i] = tmp;                
        }
    }
    return weight{samples[0], samples[1], samples[2]};
}
