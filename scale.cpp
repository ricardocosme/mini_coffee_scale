#include <hx711.hpp>
#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>
#include <stdlib.h>
#include <math.h>

using namespace att85::ssd1306;

constexpr uint8_t zero_samples{60};
constexpr uint8_t n_samples{3};
constexpr int32_t calibration{1075};

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
    
    int32_t prev_cw{0}, cw{0};
    constexpr uint8_t n_last{10};
    uint32_t diffs[n_last];
    
    while(true) {
        disp.out<font::_8x8>(0, 0, (uint32_t)zero);
        disp.out<font::_8x8>(0, 55, (uint32_t)fst_zero);
        
        int32_t samples[n_samples];
        for(uint8_t i{0}; i < n_samples; ++i) 
            samples[i] = hx711::sync_read(scale);
        for(uint8_t i{0}; i < n_samples - 1; ++i) {
            if(samples[i] > samples[i+1]) {
                auto tmp = samples[i+1];
                samples[i+1] = samples[i];
                samples[i] = tmp;                
            }
        }

        auto median = samples[1];
        
        int32_t w = ((median - zero) * 100) / calibration;
        auto diff = w - cw;
        
        for(uint8_t i{n_last-1}; i > 0; --i)
            diffs[i] = diffs[i-1];

        diffs[0] = diff;

        if(labs(diff) > 10) {
            if(w % 10 > 5) cw = (w / 10 + 1) * 10;
            else cw = (w / 10) * 10;
        } else {
            bool skip{false};
            for(uint8_t i{1}; i < n_last; ++i) {
                if(labs(diffs[i]) > 10) {
                    skip = true;
                    break;
                }
            }
            if(!skip) {  
                if(diff > 5 && diff <= 10) zero += 65; 
                else if(diff >= -10 && diff < -5) zero -= 65;
            }
            skip = false;
        }

        if(prev_cw != cw) {
            disp.out<font::_8x8>(7, 36, ATT85_SSD1306_STR("      "));
            prev_cw = cw;
        }
        
        disp.out<font::_8x8>(2, 60, ATT85_SSD1306_STR("       "));
        disp.out<font::_8x8>(2, 60, diff);
        disp.out<font::_8x8>(4, 60, ATT85_SSD1306_STR("       "));
        disp.out<font::_8x8>(4, 60, samples[1]);
        
        disp.out<font::_8x8>(2, 0, ATT85_SSD1306_STR("       "));
        disp.outf2<font::_8x8>(2, 0, w);
        disp.out<font::_8x8>(4, 0, ATT85_SSD1306_STR("       "));
        disp.outf<font::_8x8>(4, 0, cw);
        auto fw = ((median - fst_zero) * 100) / calibration;
        disp.out<font::_8x8>(6, 0, ATT85_SSD1306_STR("       "));
        disp.outf2<font::_8x8>(6, 0, fw);
    }
}
