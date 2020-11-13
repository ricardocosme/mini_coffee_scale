#include <util/delay.h>
#include <att85/ssd1306/display.hpp>
#include <att85/ssd1306/font/8x8/chars.hpp>
#include <stdlib.h>

using namespace att85::ssd1306;

#define SCK PB4
#define DT PB3

bool is_ready() {
    return !(PORTB & (1<<DT));
}

uint32_t read() {
    uint32_t v{0};
    PORTB = PORTB & ~(1<<SCK);
    while(PINB & (1<<DT));
    for (auto i=0;i<24;i++){
        PORTB = PORTB | (1<<SCK);
        v = v << 1;
        PORTB = PORTB & ~(1<<SCK);
        if(PINB & (1<<DT)) ++v;
    }
    PORTB = PORTB | (1<<SCK);
    PORTB = PORTB & ~(1<<SCK);
    // PORTB = PORTB | (1<<SCK);
    // PORTB = PORTB & ~(1<<SCK);
    // PORTB = PORTB | (1<<SCK);
    // PORTB = PORTB & ~(1<<SCK);
    return v;
}

int main() {
    display_128x64<> disp;
    disp.on();
    disp.clear();
    
    DDRB = DDRB | (1<<SCK);
    
    DDRB = DDRB & ~(1<<DT);
    PORTB = PORTB | (1<<DT);

    uint32_t tare{0};
    for(auto i=0; i < 15; ++i)
        tare += read();
    tare /= 15;
    disp.out<font::_8x8>(0, 0, tare);

    uint32_t weight{0};
    uint32_t r0{0};
    while(true) {
        uint32_t sample{0};
        uint32_t r1{0};
        for(auto i=0; i < 3; ++i) {
            r1 += read();
            sample += (fabs(fabs(r0) - tare) / 1075) * 10;
        }
        sample /= 3;
        r1 /= 3;
        uint32_t diff = labs(r1 - r0);
        r0 = r1;
        disp.out<font::_8x8>(4, 0, ATT85_SSD1306_STR("       "));
        disp.out<font::_8x8>(4, 0, diff);
        if(abs(sample - weight) == 1) continue;
        weight = sample;
        disp.out<font::_8x8>(2, 0, ATT85_SSD1306_STR("     "));
        disp.out<font::_8x8>(2, 0, weight);
    }    
}
