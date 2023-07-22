#pragma once

#include <stdio.h>
#include <stdint.h>
#include "mathematics.h"

class Frame {
    public:
        uint32_t* Pixels;
        vec2i16 Size;
        BoundingBox2D<int16_t> Bounds;

        Frame(uint32_t* pixels, vec2i16 size) : 
            Pixels(pixels), Size(size), Bounds(BoundingBox2D<int16_t>(vec2i16(0,0), size)) {};

        void Fill(uint32_t color){
            for(int i = 0; i < Size.x * Size.y / 2; i++){
                Pixels[i] = color;
            }
        }

        void PutPixel(vec2i16 pos, uint32_t color){
            if(pos.x >= 0 && pos.x < Size.x && pos.y >= 0 && pos.y < Size.y){
                Pixels[pos.y * Size.x + pos.x] = color;
            }
        }

        void DrawLine(vec2i16 start, vec2i16 end, uint32_t color){
            // Bresenham's line algorithm
            int16_t x0 = start.x;
            int16_t y0 = start.y;
            int16_t x1 = end.x;
            int16_t y1 = end.y;

            int16_t dx = abs(x1 - x0);
            int16_t dy = abs(y1 - y0);

            int16_t sx = x0 < x1 ? 1 : -1;
            int16_t sy = y0 < y1 ? 1 : -1;

            int16_t err = dx - dy;

            while(true){
                if(x0 >= 0 && x0 < Size.x && y0 >= 0 && y0 < Size.y){
                    Pixels[y0 * Size.x + x0] = color;
                    printf("Drawing pixel at %d %d\n", x0, y0);
                }

                if(x0 == x1 && y0 == y1) break;

                int16_t e2 = 2 * err;

                if(e2 > -dy){
                    err -= dy;
                    x0 += sx;
                }

                if(e2 < dx){
                    err += dx;
                    y0 += sy;
                }
            }
        }

};