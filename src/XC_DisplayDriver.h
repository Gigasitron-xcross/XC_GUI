#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "XC_GUI_Types.h"

/*
 * XC_DisplayDriver
 *
 * Hardware display abstraction used by the Arduino C++ wrapper.
 *
 * The latest XGUI C core uses a driver table with:
 *   - init
 *   - set memory buffer
 *   - get size + controller id
 *   - reset
 *   - set window
 *   - write pixel block
 *   - put single pixel
 *   - transfer-done callback
 *
 * This C++ base class mirrors those features in an Arduino-friendly way.
 */
class XC_DisplayDriver
{
public:
    enum
    {
        XC_CONTROLLER_UNKNOWN = 0,
        XC_CONTROLLER_SSD1306 = 1306,
        XC_CONTROLLER_ST7789  = 7789
    };

    virtual ~XC_DisplayDriver() {}

    virtual bool begin() = 0;

    virtual int16_t width() const = 0;
    virtual int16_t height() const = 0;

    /*
     * Your latest XGUI.c uses this value in pGetSize() to authorize/select
     * the controller. Override this in each real display driver if needed.
     */
    virtual int controllerId() const
    {
        return XC_CONTROLLER_SSD1306;
    }

    /*
     * RGB565 for TFT drivers, MONO1 for SSD1306/SH1106 style drivers.
     */
    virtual XC_PIXEL_FORMAT pixelFormat() const
    {
        return XC_PIXEL_RGB565;
    }

    virtual void reset()
    {
    }

    /*
     * Called by XGUI through pSetMemBuf().
     * For TFT drivers this can be ignored.
     * For MONO1/page-buffer drivers this can be used to keep the active
     * XGUI RAM block pointer.
     */
    virtual void setMemBuffer(void *pBuf, uint32_t nByte)
    {
        (void)pBuf;
        (void)nByte;
    }

    /* Inclusive address window. */
    virtual void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) = 0;

    /*
     * Single-pixel path. Useful for monochrome displays with native RAM
     * framebuffers. color is 0 or 1 for MONO1.
     */
    virtual void putPixel(int16_t x, int16_t y, char color)
    {
        (void)x;
        (void)y;
        (void)color;
    }

    /*
     * RGB565 block path. count is number of 16-bit pixels.
     * TFT drivers should override this.
     */
    virtual void pushPixels(const uint16_t *pixels, uint32_t count)
    {
        (void)pixels;
        (void)count;
    }

    /*
     * Raw byte block path. MONO1 drivers can override this if they still want
     * to receive XGUI's packed RAM block. Default maps to RGB565 pushPixels().
     */
    virtual void pushBuffer(const void *buffer, uint32_t bytes)
    {
        if ((buffer == nullptr) || (bytes == 0U))
        {
            return;
        }

        pushPixels((const uint16_t *)buffer, bytes / sizeof(uint16_t));
    }

    /*
     * Called by the bridge from XGUI pWritePixel().
     * nPixel is the logical pixel count supplied by XGUI.
     */
    virtual void writePixels(const void *pData, int nPixel)
    {
        if ((pData == nullptr) || (nPixel <= 0))
        {
            return;
        }

        if (pixelFormat() == XC_PIXEL_MONO1)
        {
            uint32_t bytes = ((uint32_t)nPixel + 7U) / 8U;
            pushBuffer(pData, bytes);
        }
        else
        {
            pushPixels((const uint16_t *)pData, (uint32_t)nPixel);
        }
    }

    /* Optional final flush hook for framebuffer-based displays. */
    virtual void update()
    {
    }

    virtual void fillPixels(uint16_t color, uint32_t count)
    {
        uint16_t block[64];

        for (uint8_t i = 0; i < 64; i++)
        {
            block[i] = color;
        }

        while (count)
        {
            uint32_t n = (count > 64U) ? 64U : count;
            pushPixels(block, n);
            count -= n;
        }
    }

    virtual void setRotation(uint8_t rotation)
    {
        (void)rotation;
    }

    virtual void setBacklight(bool on)
    {
        (void)on;
    }
};
