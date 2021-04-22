#include "color.h"
#include <stdio.h>
#include <stdlib.h>

rgb_color_t rgb_color_init(){
    rgb_color_t random_color = { 
        .r = (float) rand() / RAND_MAX,
        .g = (float) rand() / RAND_MAX,
        .b = (float) rand() / RAND_MAX,
    };
    return random_color;
}