#pragma once

#include <stdio.h>
#include <stdint.h>
#include <cfloat>
#include "mathematics.h"
#include "rendering/font.h"

class Frame {
    public:
        uint32_t* Pixels;
        vec2i16 Size;
        BoundingBox2D Bounds;

        Frame(uint32_t* pixels, vec2i16 size) : 
            Pixels(pixels),
            Size(size),
            Bounds(BoundingBox2D(vec2f(0.0f), vec2f(size.x(), size.y()))){
                zbuffer = (float*)malloc(size.x() * size.y() * sizeof(float));

                // TODO: if zbuffer is null, panic

                // set zbuffer to highest possible floating point value
                for(int i = 0; i < size.x() * size.y(); i++){
                    zbuffer[i] = FLT_MAX;
                }
            };

        void Clear(uint32_t color){
            for(int i = 0; i < Size.x() * Size.y(); i++){
                Pixels[i] = color;
                zbuffer[i] = FLT_MAX;
            }
        }

        inline void PutPixel(vec2i16 pos, uint32_t color){
            if(pos.x() >= 0 && pos.x() < Size.x() && pos.y() >= 0 && pos.y() < Size.y()){
                Pixels[pos.y() * Size.x() + pos.x()] = color;
            }
        }

        void DrawBox(BoundingBox2D box, uint32_t color){
            BoundingBox2D bbi = Bounds.Intersect(box);

            for(int y = bbi.Min.y(); y < static_cast<int>(bbi.Max.y()); y++){
                for(int x = bbi.Min.x(); x < static_cast<int>(bbi.Max.x()); x++){
                    Pixels[y * Size.x() + x] = color;
                }
            }
        }

        void DrawBorder(BoundingBox2D box, uint8_t width, uint32_t color){
            BoundingBox2D bbi = box;// Bounds.Intersect(box);

            int16_t startX = static_cast<int16_t>(bbi.Min.x());
            int16_t startY = static_cast<int16_t>(bbi.Min.y());
            int16_t endX = std::ceil(bbi.Max.x());
            int16_t endY = std::ceil(bbi.Max.y());

            for(int y = startY; y < startY + width; y++){
                for(int x = startX; x < endX; x++){
                    // Pixels[y * Size.x() + x] = color;
                    PutPixel(vec2i16(x, y), color);
                }
            }

            for(int y = startY + width; y < endY - width; y++){
                for(int x = startX; x < startX + width; x++){
                    // Pixels[y * Size.x() + x] = color;
                    PutPixel(vec2i16(x, y), color);
                }

                for(int x = endX - width; x < endX; x++){
                    // Pixels[y * Size.x() + x] = color;
                    PutPixel(vec2i16(x, y), color);
                }
            }

            for(int y = endY - width; y < endY; y++){
                for(int x = startX; x < endX; x++){
                    // Pixels[y * Size.x() + x] = color;
                    PutPixel(vec2i16(x, y), color);

                }
            }
        }

        void DrawLine(vec2i16 start, vec2i16 end, uint32_t color){
            // Bresenham's line algorithm
            int16_t x0 = start.x();
            int16_t y0 = start.y();
            int16_t x1 = end.x();
            int16_t y1 = end.y();

            int16_t dx = abs(x1 - x0);
            int16_t dy = abs(y1 - y0);

            int16_t sx = x0 < x1 ? 1 : -1;
            int16_t sy = y0 < y1 ? 1 : -1;

            int16_t err = dx - dy;

            while(true){
                PutPixel(vec2i16(x0, y0), color);

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

        void DrawTriangle(vec3f t1, vec3f t2, vec3f t3, uint32_t color){
            BoundingBox2D bb = BoundingBox2D::FromTriangle(t1.xy(), t2.xy(), t3.xy());
            BoundingBox2D bbi = Bounds.Intersect(bb);

            DrawBorder(bbi, 2, 0xFFFF0000);

            printf("Screen bounding box: %f, %f, %f, %f\n", Bounds.Min.x(), Bounds.Min.y(), Bounds.Max.x(), Bounds.Max.y());
            printf("Triangle bounding box: %f, %f, %f, %f\n", bb.Min.x(), bb.Min.y(), bb.Max.x(), bb.Max.y());
            printf("Bounding intersection: %f, %f, %f, %f\n", bbi.Min.x(), bbi.Min.y(), bbi.Max.x(), bbi.Max.y());

            if(bbi.IsEmpty()) {
                printf("Triangle is offscreen");
                return;
            }

            for(int16_t x = bbi.Min.x(); x < bbi.Max.x(); x++){
                for(int16_t y = bbi.Min.y(); y < bbi.Max.y(); y++){
                    if(pointInTriangle(vec3f(x, y, 0), t1, t2, t3)){
                        // calculate a mock uv for the pixel
                        vec3f uvw = barycentric(vec2f(x, y), t1.xy(), t2.xy(), t3.xy());
                        // calculate the z-coordinate of the pixel
                        float z = t1.z() * uvw.x() + t2.z() * uvw.y() + t3.z() * uvw.z();
                        if(z < zbuffer[y * Size.x() + x]){
                            Pixels[y * Size.x() + x] = 0xFF << 24 | static_cast<uint8_t>(uvw.x() * 255) << 16 | static_cast<uint8_t>(uvw.y() * 255) << 8 | static_cast<uint8_t>(uvw.z() * 255);
                            zbuffer[y * Size.x() + x] = z;
                            // Pixels[y * Size.x() + x] = 0xFF << 24 | static_cast<uint8_t>(z/0.1 * 255) << 16;
                        }
                    }
                }
            }
        }

        void DrawText(const char* text, vec2i16 pos, Font* font, uint32_t color){
            // use the font to draw the text
            int x = pos.x();
            int y = pos.y();

            while(*text){
                uint8_t* glyph = font->GetGlyph(*text);

                if(glyph){
                    for(int i = 0; i < font->GlyphSize.y(); i++){
                        for(int j = 0; j < font->GlyphSize.x(); j++){
                            if(glyph[i] & (0x80 >> j)){
                                PutPixel(vec2i16(x + j, y + i), color);
                            }
                        }
                    }
                }

                x += font->GlyphSize.x();
                text++;
            }
        }

    private:
        float* zbuffer;
};