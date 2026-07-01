#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "XC_DisplayDriver.h"

typedef union __attribute__((packed)) _tagGUI_DEVINFO
{
    uint8_t data[20];

    struct __attribute__((packed))
    {
        uint16_t w;
        uint16_t h;
        uint16_t offset;
        uint32_t drvcode;
        uint8_t id[10];
    } b;

} GUI_DEVINFO, *PGUI_DEVINFO;

/*
 * XC_Display_SSD1306
 *
 * Arduino-style SSD1306 I2C display driver for the latest XC_GUI wrapper.
 *
 * Design:
 * - Keeps a native SSD1306 framebuffer: 1 byte = 8 vertical pixels.
 * - putPixel(x,y,color) writes directly into this framebuffer.
 * - update() flushes the full framebuffer with SSD1306 horizontal addressing.
 * - Suitable for the latest XGUI MONO1 path where XGUI calls pPutPixel().
 */
class XC_Display_SSD1306 : public XC_DisplayDriver
{
public:
    static const uint8_t DEFAULT_I2C_ADDR_7BIT = 0x3C;
    static const int16_t DEFAULT_WIDTH = 128;
    static const int16_t DEFAULT_HEIGHT = 64;

    /* Enough for 128x64. For 128x32 only the first 512 bytes are used. */
    static const uint16_t MAX_WIDTH = 128;
    static const uint16_t MAX_HEIGHT = 64;
    //static const uint16_t MAX_BUFFER_BYTES = MAX_WIDTH * MAX_HEIGHT / 8;

public:
    XC_Display_SSD1306(TwoWire &wire = Wire,
                       uint8_t address = DEFAULT_I2C_ADDR_7BIT,
                       int16_t width = DEFAULT_WIDTH,
                       int16_t height = DEFAULT_HEIGHT);

    bool begin() override;

    int16_t width() const override;
    int16_t height() const override;

    int controllerId() const override;
    XC_PIXEL_FORMAT pixelFormat() const override;

    void reset() override;
    void setMemBuffer(void *pBuf, uint32_t nByte) override;
    void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override;

    /* Latest XGUI MONO1 path calls this directly. color: 0=black, non-zero=white. */
    void putPixel(int16_t x, int16_t y, char color) override;

    /* Optional raw MONO1 block path for older XGUI versions. */
    void writePixels(const void *pData, int nPixel) override;


private:
    bool command(uint8_t c);
    bool command2(uint8_t c0, uint8_t c1);
    bool command3(uint8_t c0, uint8_t c1, uint8_t c2);
    bool sendData(const uint8_t *data, uint16_t len);


private:
    TwoWire *_wire;
    uint8_t _address;
    int16_t _width;
    int16_t _height;

    int16_t _winX0;
    int16_t _winY0;
    int16_t _winX1;
    int16_t _winY1;
    int pageStart;
    int pageEnd;

    uint8_t *_xguiMemBuf;
    uint32_t _xguiMemBufBytes;
    bool _directPixelMode;
    GUI_DEVINFO  g_DevInfo;
};
