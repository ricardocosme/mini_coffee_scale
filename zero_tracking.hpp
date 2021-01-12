#pragma once

#include <stdint.h>
#include <stdlib.h>

inline int32_t zero_tracking(
    int32_t zero,
    const int32_t* diffs,
    uint32_t size)
{
    for(uint8_t i{1}; i < size; ++i) {
        if(labs(diffs[i]) > 10)
            return zero;
    }
    if(diffs[0] > 5 && diffs[0] <= 10) zero += 65; 
    else if(diffs[0] >= -10 && diffs[0] < -5) zero -= 65;
    
    return zero;
}
