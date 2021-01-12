#pragma once

#include <stdint.h>

inline auto round(int32_t weight) {
    if(weight % 10 > 5)  weight = (weight / 10 + 1) * 10;
    else weight = (weight / 10) * 10;
    if(weight == -10) weight = 0;
    return weight;
}
