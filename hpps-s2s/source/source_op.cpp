#include "fixed_point.hpp"
//
// Created by nicola on 22/05/19.
//

#include <stdio.h>

double clip_rgb(
	fixed_point_t<9,23> val 
)
{
    if (val < 0) val= 0;
    if (val > 255) val= 255;
    return val.getValueFD();
}

typedef struct RGB {
    int r, g, b;
} RGB;


int main()  {
	fixed_point_t<9,23> r;
	fixed_point_t<9,23> g;
	fixed_point_t<9,23> b;

    int y, cr, cb;
    scanf("%d", &y);
    scanf("%d", &cr);
    scanf("%d", &cb);

    RGB pixels[5];
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            r = y + 1.402 * cr + 128;
            g = y - 0.34414 * cb - 0.71414 * cr + 128;
            b = y + 1.772 * cb + 128;
            printf("Pixel: %f %f %f\n",r.getValueF(),g.getValueF(),b.getValueF());

            r = clip_rgb(r);
            g = clip_rgb(g);
            b = clip_rgb(b);


            pixels[i].r = (int ) r;
            pixels[i].g = (int ) g;
            pixels[i].b = (int ) b;
            printf("Pixel: %d %d %d\n", pixels[i].r, pixels[i].g, pixels[i].b);
        }
    }




    return 0;
}
