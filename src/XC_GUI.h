#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "XC_GUI_Types.h"
#include "XC_DisplayDriver.h"

/*
 * XC_GUI
 *
 * Public Arduino C++ API.
 *
 * This header intentionally does NOT include "XGUI.h".
 * The C core remains hidden inside XC_GUI.cpp.
 */
class XC_GUI
{
public:
    XC_GUI();

    void attachDriver(XC_DisplayDriver *driver);

    bool auth(const char *password = "www.gigasitron.com");

    /*
     * Simple Arduino-style begin().
     *
     * User supplies only the drawing buffer.
     * The old GUI_HANDLE is kept internally by this class.
     *
     * buffer is void* so it works with both:
     *   uint16_t rgb565Buffer[]
     *   uint8_t  monoPageBuffer[]
     */
    bool begin(void *buffer,
               int bufferBytes,
               int width = -1,
               int height = -1);

    void drawExecute();
    void drawExecuteBlocking();

    /* Optional C++ drawing callback. Subclass XC_GUI and override this. */
    virtual void onDraw(bool frameStart);

    void onFrameEnd(XC_FRAME_END_CALLBACK *callback);

    void set16BitPerPixel(bool enable);

    void clear(XC_COLOR color);

    void setColor(XC_COLOR color);
    void setColor16(uint16_t color565);
    void setPenWidth(int width);

    void drawLine(int x0, int y0, int x1, int y1, XC_COLOR color);
    void drawHLine(int y, int left, int right, XC_COLOR color);
    void drawVLine(int x, int top, int bottom, XC_COLOR color);

    void drawRect(int left, int top, int right, int bottom, XC_COLOR color);
    void fillRect(int left, int top, int right, int bottom, XC_COLOR color);

    void drawCircle(int x, int y, int radius, XC_COLOR color);
    void fillCircle(int x, int y, int radius, XC_COLOR color);

    void setFont(const XC_FONT *font);
    void setFontBackColorEnable(uint8_t enable);
    void setFontBackColor(XC_COLOR color);
    void setFontBackColor16(uint16_t color565);

    uint8_t printChar(char c, XC_COLOR color, int x, int y);
    void printString(const char *str, XC_COLOR color, int x, int y);

    void drawBitmap(const void *bmp, int x, int y);

#ifdef __FF_EN
    void drawBitmapFile(char *path, int x, int y);
#endif

    XC_DisplayDriver *driver() const;

    static XC_GUI *active();

private:
    int calculateBlockRows() const;

private:
    static XC_GUI *_active;

    /*
     * Latest XGUI.h uses:
     *     typedef struct { uint32_t array[23]; } GUI_HANDLE;
     */
    uint32_t _handleStorage[23];

    XC_DisplayDriver *_driver;
    void *_buffer;

    int _width;
    int _height;
    int _bufferBytes;
    XC_PIXEL_FORMAT _pixelFormat;
    bool _authorized;
};
