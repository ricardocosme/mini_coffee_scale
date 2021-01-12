#include "measure.hpp"
#include "round.hpp"
#include "zero_tracking.hpp"

#include <hx711.hpp>
#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>
#include <stdlib.h>
#include <math.h>

using namespace att85::ssd1306;

constexpr uint8_t zero_samples{60};
constexpr uint8_t n_samples{3};
constexpr int32_t calibration{1075};
constexpr uint8_t resolution{10}; //0.1g

int main() {
    display_128x64<> disp;
    disp.on();
    disp.clear();

    hx711::sensor<PB4, PB3> scale;

    disp.out<font::_8x8>(3, 0, ATT85_SSD1306_STR("measuring"));
    disp.out<font::_8x8>(4, 0, ATT85_SSD1306_STR("tare weight"));
    int32_t zero{0};
    bool add{true};
    for(auto i{0}, c{0}; i < zero_samples; ++i) {
        zero += hx711::sync_read(scale);
        if(c == 10) {
            add = false;
            disp.out<font::_8x8>(
                5, 0, ATT85_SSD1306_STR("                "));
        } else if(c == 0) {
            add = true;
            disp.out<font::_8x8>(
                5, 0, ATT85_SSD1306_STR("                "));
        }
        disp.out<font::_8x8>(5, c*8, ATT85_SSD1306_STR("."));
        if(add) ++c;
        else --c;
    }
    zero /= zero_samples;
    disp.clear();

    auto fst_zero = zero;
    
    int32_t rounded_weight{0};

    constexpr uint8_t n_diffs{15};
    int32_t diffs[n_diffs];
    
    while(true) {
        disp.out<font::_8x8>(0, 0, (uint32_t)zero);
        disp.out<font::_8x8>(0, 55, (uint32_t)fst_zero);
        
        auto samples = measure(scale);
        
        int32_t weight = ((samples.median - zero) * 100) / calibration;
        // Save the difference between the current weight and the
        // previous one and shift to the right all differences that
        // were previously calculated.
        diffs[0] = weight - rounded_weight;
        for(uint8_t i{n_diffs -1 }; i > 0; --i)
            diffs[i] = diffs[i-1];

        if(labs(diffs[0]) > resolution) rounded_weight = round(weight);
        else zero = zero_tracking(zero, diffs, n_diffs);

        disp.out<font::_8x8>(2, 60, ATT85_SSD1306_STR("       "));
        disp.out<font::_8x8>(2, 60, diffs[0]);
        disp.out<font::_8x8>(4, 60, ATT85_SSD1306_STR("       "));
        disp.out<font::_8x8>(4, 60, diffs[0] * calibration / 100);
        
        disp.out<font::_8x8>(2, 0, ATT85_SSD1306_STR("       "));
        disp.outf2<font::_8x8>(2, 0, weight);
        disp.out<font::_8x8>(4, 0, ATT85_SSD1306_STR("       "));
        disp.outf<font::_8x8>(4, 0, rounded_weight);
        auto fw = ((samples.median - fst_zero) * 100) / calibration;
        disp.out<font::_8x8>(6, 0, ATT85_SSD1306_STR("       "));
        disp.outf2<font::_8x8>(6, 0, fw);
    }
}
