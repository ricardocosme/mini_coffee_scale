#include "measure.hpp"
#include "round.hpp"
#include "tare.hpp"

#include <hx711.hpp>
#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>
#include <stdlib.h>
#include <math.h>

using namespace att85::ssd1306;

constexpr uint8_t zero_samples{60};
constexpr int32_t calibration{1075};
constexpr uint8_t resolution{10}; //0.1g

int main() {
    display_128x64<> disp;
    disp.on();
    disp.clear();

    hx711::adc<PB4, PB3> scale;

    disp.out<font::_8x8>(3, 0, ATT85_SSD1306_STR("measuring"));
    disp.out<font::_8x8>(4, 0, ATT85_SSD1306_STR("tare weight"));
    
    auto zero = tare(scale, zero_samples, disp);
    auto fst_zero = zero;
    
    int32_t rounded_weight{0};
    int32_t curr_sample{0};
    
    int32_t pzero{0}, pdiffs{0}, pfst_zero{0}, pwdiffs{0},
        pweight{0}, pfw{0};
    while(true) {
        if(zero != pzero) {
            disp.out<font::_8x8>(0, 0, (uint32_t)zero);
            pzero = zero;
        }
        if(fst_zero != pfst_zero) {
            disp.out<font::_8x8>(0, 55, (uint32_t)fst_zero);
            pfst_zero = fst_zero;
        }
        
        auto sample = measure(scale, disp, zero, calibration, curr_sample).median;
        curr_sample = sample;
            
        int32_t weight = ((sample - zero) * 100) / calibration;
        auto diff = weight - rounded_weight;

        if(labs(diff) > resolution) rounded_weight = round(weight);

        if(diff != pdiffs) {
            disp.out<font::_8x8>(2, 60, ATT85_SSD1306_STR("       "));
            disp.out<font::_8x8>(2, 60, diff);
            pdiffs = diff;
            
        }
        auto wdiffs = diff * calibration / 100;
        if(wdiffs != pwdiffs) {
            disp.out<font::_8x8>(4, 60, ATT85_SSD1306_STR("       "));
            disp.out<font::_8x8>(4, 60, wdiffs);
            pwdiffs = wdiffs;
        }
        
        if(weight != pweight) {
            disp.outf2<font::_8x8>(2, 0, weight);
            pweight = weight;
        }
        
        disp.outf<font::_8x8>(4, 0, rounded_weight);
        
        auto fw = ((sample - fst_zero) * 100) / calibration;
        if(fw != pfw) {
            disp.outf2<font::_8x8>(6, 0, fw);
            pfw = fw;
        }
    }
}
