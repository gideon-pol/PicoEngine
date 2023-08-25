#pragma once

#include <stdint.h>
#include "common.h"
#include "mathematics.h"


struct PSF2Header {
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
            header = (PSF2Header*)data;

            if(header->magic != 0x864ab572){
                printf("PSF2: Invalid magic number: %x\n", header->magic);
                return;
            }

            printf("PSF2: Font header size: %d\n", header->header_size);
            printf("PSF2: Flags: %d\n", header->flags);
            if(header->flags & 0x1){
                printf("Font contains unicode characters\n");
            }
            glyphs = (uint8_t*)(data + header->header_size);
            bytesPerLine = (header->width + 7) / 8;
            GlyphSize = vec2i16((int16_t)header->width, (int16_t)header->height);
            printf("PSF2: Glyph size: %d, %d\n", GlyphSize(0), GlyphSize(1));

        }

        FORCE_INLINE uint8_t* GetGlyph(int c){
            if(c < 0 || c >= header->num_glyph){
                return NULL;
            }
            return glyphs + (c * header->bytes_per_glyph);
        }
    private:
        PSF2Header* header;
        uint8_t* glyphs;
        int bytesPerLine;
};

