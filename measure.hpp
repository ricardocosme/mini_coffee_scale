#pragma once

#include "round.hpp"

#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>
#include <hx711.hpp>
#include <stdint.h>
#include <stdlib.h>

struct weight { int32_t min, median, max; };
    
template<typename HX711, typename Display>
auto measure(HX711& sensor, Display& disp, int32_t zero, int32_t calibration, int32_t curr_sample) {
    using namespace att85::ssd1306;
    constexpr uint8_t size{3};
    int32_t samples[size];
    int32_t preview{0};
    for(uint8_t i{0}; i < size; ++i) {
        samples[i] = hx711::sync_read(sensor);
        auto diff = labs(samples[i] - curr_sample);
        if(diff > 323 /*0.3g*/ && diff > preview) {
            auto weight = ((samples[i] - zero) * 100) / calibration;
            disp.template outf<font::_8x8>(4, 0, round(weight));
            preview = diff;
        }
    }
    for(uint8_t i{0}; i < size - 1; ++i) {
        if(samples[i] > samples[i+1]) {
            auto tmp = samples[i+1];
            samples[i+1] = samples[i];
            samples[i] = tmp;                
        }
    }
    return weight{samples[0], samples[1], samples[2]};
}
