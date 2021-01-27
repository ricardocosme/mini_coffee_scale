#pragma once

#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>

template<typename HX711, typename Display>
auto tare(HX711& adc, uint8_t samples, Display& disp) {
    using namespace att85::ssd1306;
    int32_t zero{0};
    bool add{true};
    for(auto i{0}, c{0}; i < samples; ++i) {
        zero += adc.read();
        if(c == 10) {
            add = false;
            disp.template out<font::_8x8>(
                5, 0, ATT85_SSD1306_STR("                "));
        } else if(c == 0) {
            add = true;
            disp.template out<font::_8x8>(
                5, 0, ATT85_SSD1306_STR("                "));
        }
        disp.template out<font::_8x8>(5, c*8, ATT85_SSD1306_STR("."));
        if(add) ++c;
        else --c;
    }
    zero /= samples;
    disp.clear();
    return zero;
}
