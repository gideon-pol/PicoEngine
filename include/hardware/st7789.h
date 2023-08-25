#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/vreg.h"

#include "common.h"
#include "rendering/color.h"
#include "screen_double.pio.h"

#define SWRESET 0x01
#define COLMOD 0x3A
#define MADCTL 0x36
#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C
#define RAMRD 0x2E
#define INVON 0x21
#define SLPOUT 0x11
#define DISPON 0x29
#define DISPOFF 0x28
#define NORON 0x13
#define INVOFF 0x20
#define TEON 0x35
#define TEOFF 0x34
#define RDID1 0xDA
#define RDID2 0xDB
#define RDID3 0xDC
#define FRMCTR1 0xB1
#define FRMCTR2 0xB2
#define FRMCTR3 0xB3
#define INVCTR 0xB4
#define PWCTR1 0xC0
#define PWCTR2 0xC1
#define PWCTR3 0xC2
#define GAMSET 0x26

namespace ST7789 {
    enum Pin {
        SCK = 18,
        MOSI = 19,
        CS = 17,
        DC = 20,
        RST = 21,
        BL = 22
    };

    namespace {
        uint32_t dmaChannel = 0;
        uint screenSM = 0;
        PIO screenPIO = pio0;
        
        Color16* fb;

        volatile int16_t dma_scanline = 0;

        volatile bool dmaBusy = false;

        void transmitScanline() {
            uint32_t* s = (uint32_t*)&fb[(dma_scanline == 119 ? 118 : dma_scanline) * FRAME_WIDTH];// [((dma_scanline - 1) < 0 ? 0 : (dma_scanline - 1)) * FRAME_WIDTH];
            dmaBusy = true;
            dma_channel_transfer_from_buffer_now(dmaChannel, s, FRAME_WIDTH);
        }

        void __isr dmaComplete() {
            if(dma_channel_get_irq0_status(dmaChannel)) {
                dma_channel_acknowledge_irq0(dmaChannel);
            }

            if(++dma_scanline >= FRAME_HEIGHT) {
                dma_scanline = 0;
                dmaBusy = false;
                return;
            }

            transmitScanline();
        };

        void sendCommand(uint8_t c, size_t len = 0, const char *data = NULL) {
            gpio_put(Pin::CS, 0);
            gpio_put(Pin::DC, 0);
            spi_write_blocking(spi0, &c, 1);

            if(data) {
                gpio_put(DC, 1);
                spi_write_blocking(spi0, (const uint8_t*)data, len);
            }
            gpio_put(CS, 1);
        }
    };

    void Init(){
        pwm_config cfg = pwm_get_default_config();
        uint slice = pwm_gpio_to_slice_num(Pin::BL);
        pwm_set_wrap(slice, 65535);
        pwm_init(slice, &cfg, true);
        gpio_set_function(Pin::BL, GPIO_FUNC_PWM);

        spi_init(spi0, 8000000);
        
        gpio_set_function(Pin::SCK, GPIO_FUNC_SPI);
        gpio_set_function(Pin::MOSI, GPIO_FUNC_SPI);

        gpio_set_function(Pin::DC, GPIO_FUNC_SIO);
        gpio_set_function(Pin::CS, GPIO_FUNC_SIO);
        gpio_set_function(Pin::RST, GPIO_FUNC_SIO);
        
        gpio_set_dir(Pin::DC, GPIO_OUT);
        gpio_set_dir(Pin::CS, GPIO_OUT);
        gpio_set_dir(Pin::RST, GPIO_OUT);

        gpio_put(Pin::RST, 0);
        sleep_ms(10);
        gpio_put(Pin::RST, 1);

        // ST7789 documentation: https://www.rhydolabz.com/documents/33/ST7789.pdf
        sendCommand(SWRESET);
        sleep_ms(10);

        /*
            -- 12 bits per pixel
        */

        /*
            -- Top to bottom page address order
            -- Left to right column address order
            -- Page/column order normal mode
            -- Top to bottom refresh
            -- RGB order
            -- Left to right refresh
        */
        sendCommand(MADCTL, 1, "\x04");
        // sendCommand(TEON, 1,"\x00");
        sendCommand(COLMOD, 1, "\x03");
        sendCommand(GAMSET, 1, "\x02");
        // sendCommand(TEON, 1, "\b00000000");
        sendCommand(INVON);

        sleep_ms(200);

        // Take display out of sleep mode and turn on display
        sendCommand(SLPOUT);
        sendCommand(NORON);

        sendCommand(CASET, 4, "\x00\x00\x00\xEF");
        sendCommand(RASET, 4, "\x00\x00\x00\xEF");

        sendCommand(DISPON);

        // Accept data transfer from controller
        sendCommand(RAMWR);

        gpio_put(Pin::CS, 0);
        gpio_put(Pin::DC, 1);


        uint offset = pio_add_program(screenPIO, &screen_double_program);
        pio_sm_config c = screen_double_program_get_default_config(offset);
        pio_sm_set_consecutive_pindirs(pio0, screenSM, Pin::MOSI, 2, true);

#ifdef ENABLE_OVERCLOCK
        sm_config_set_clkdiv_int_frac(&c, 2, 1);
#endif
        sm_config_set_out_shift(&c, false, false, 32);

        sm_config_set_out_pins(&c, Pin::MOSI, 1);
        sm_config_set_sideset_pins(&c, Pin::SCK);

        pio_sm_set_pins_with_mask(pio0, screenSM, 0, (1u << Pin::SCK) | (1u << Pin::MOSI));

        pio_sm_set_pindirs_with_mask(pio0, screenSM, (1u << Pin::SCK) | (1u << Pin::MOSI), (1u << Pin::SCK) | (1u << Pin::MOSI));

        sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

        pio_gpio_init(screenPIO, Pin::MOSI);
        pio_gpio_init(screenPIO, Pin::SCK);

        pio_sm_init(pio0, screenSM, offset, &c);
        pio_sm_set_enabled(pio0, screenSM, true);


        dma_channel_config config = dma_channel_get_default_config(dmaChannel);
        channel_config_set_bswap(&config, true);
        channel_config_set_dreq(&config, pio_get_dreq(screenPIO, screenSM, true));
        dma_channel_configure(dmaChannel, &config, &screenPIO->txf[screenSM], nullptr, 0, false);
        // channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
        dma_channel_set_irq0_enabled(dmaChannel, true);
        irq_set_enabled(pio_get_dreq(screenPIO, screenSM, true), true);

        irq_set_exclusive_handler(DMA_IRQ_0, dmaComplete);
        irq_set_enabled(DMA_IRQ_0, true);

        printf("Set up DMA\n");
    };

    void Flip(Color16* data){
        // gpio_put(Pin::CS, 0);
        // gpio_put(Pin::DC, 1);
        // spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        // spi_write16_blocking(spi0, data, len);
        // gpio_put(Pin::CS, 1);

        fb = data;
        transmitScanline();
    };

    bool IsFlipping() {
        return dmaBusy;
    }

    FORCE_INLINE void SetBrightness(uint16_t brightness) {
        pwm_set_gpio_level(Pin::BL, brightness);
    }
};