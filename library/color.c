#include "color.h"
#include <stdio.h>
#include <stdlib.h>

// creates yellow
rgb_color_t rgb_color_init(){
    rgb_color_t random_color = { 
        .r = 1.0,
        .g = 1.0,
        .b = 0.0
    };
    return random_color;
}