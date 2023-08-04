#pragma once

#include <stdint.h>
#include "mathematics.h"

extern "C" const uint16_t main_font[];

struct psf2_header {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyph;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} __attribute__((packed));

class Font {
    public:
        vec2i16 GlyphSize = vec2i16(0);

        Font(const uint8_t* data){
            header = (psf2_header*)data;

            if(header->magic != 0x864ab572){
                printf("Invalid magic number: %x\n", header->magic);
                return;
            }

            // printf("Magic: %x", header->magic);


            glyphs = (uint8_t*)(data + header->header_size);
            bytesPerLine = (header->width + 7) / 8;
            // TODO: make a u32 vector
            GlyphSize = vec2i16((int16_t)header->width, (int16_t)header->height);
        }

        uint8_t* GetGlyph(int c){
            if(c < 0 || c >= header->num_glyph){
                return NULL;
            }
            return glyphs + (c * header->bytes_per_glyph);
        }
    private:
        psf2_header* header;
        uint8_t* glyphs;
        int bytesPerLine;
};

