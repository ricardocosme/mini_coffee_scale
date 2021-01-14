#include "measure.hpp"
#include "round.hpp"
#include "tare.hpp"
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
    
    auto zero = tare(scale, zero_samples, disp);
    auto fst_zero = zero;
    
    int32_t rounded_weight{0};
    constexpr uint8_t n_diffs{15};
    int32_t diffs[n_diffs];
    int32_t curr_sample{0};
    
    int32_t pzero{0}, pdiffs{0}, pfst_zero{0}, pwdiffs{0},
        pweight{0}, prounded_weight{0}, pfw{0};
    while(true) {
        if(zero != pzero) {
            disp.out<font::_8x8>(0, 0, (uint32_t)zero);
            pzero = zero;
        }
        if(fst_zero != pfst_zero) {
            disp.out<font::_8x8>(0, 55, (uint32_t)fst_zero);
            pfst_zero = fst_zero;
        }
        
        auto samples = measure(scale, disp, zero, calibration, curr_sample);
        curr_sample = samples.median;
            
        int32_t weight = ((samples.median - zero) * 100) / calibration;
        // Save the difference between the current weight and the
        // previous one and shift to the right all differences that
        // were previously calculated.
        diffs[0] = weight - rounded_weight;
        for(uint8_t i{n_diffs -1 }; i > 0; --i)
            diffs[i] = diffs[i-1];

        if(labs(diffs[0]) > resolution) rounded_weight = round(weight);
        else zero = zero_tracking(zero, diffs, n_diffs);

        if(diffs[0] != pdiffs) {
            disp.out<font::_8x8>(2, 60, ATT85_SSD1306_STR("       "));
            disp.out<font::_8x8>(2, 60, diffs[0]);
            pdiffs = diffs[0];
            
        }
        auto wdiffs = diffs[0] * calibration / 100;
        if(wdiffs != pwdiffs) {
            disp.out<font::_8x8>(4, 60, ATT85_SSD1306_STR("       "));
            disp.out<font::_8x8>(4, 60, wdiffs);
            pwdiffs = wdiffs;
        }
        
        if(weight != pweight) {
            disp.outf2<font::_8x8>(2, 0, weight);
            pweight = weight;
        }
        
        if(rounded_weight != prounded_weight) {
            disp.outf<font::_8x8>(4, 0, rounded_weight);
            prounded_weight = rounded_weight;
        }
        
        auto fw = ((samples.median - fst_zero) * 100) / calibration;
        if(fw != pfw) {
            disp.outf2<font::_8x8>(6, 0, fw);
            pfw = fw;
        }
    }
}
