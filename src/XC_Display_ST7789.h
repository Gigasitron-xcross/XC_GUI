#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "XC_DisplayDriver.h"

/*
 * XC_Display_ST7789 / ST7789Driver
 *
 * ST7789 RGB565 SPI display driver for XC_GUI.
 * Compatible with the updated XC_DisplayDriver interface:
 *   - controllerId()
 *   - pixelFormat()
 *   - reset()
 *   - setWindow()
 *   - pushPixels()
 *   - fillPixels()
 *   - setRotation()
 *   - setBacklight()
 *
 */

#pragma pack( push, 1)
 typedef union _tagGUI_DEVINFO
 {
    uint8_t data[20];
    struct
    {
        uint16_t w;
        uint16_t h;
        uint16_t offset;
        uint32_t drvcode;
        uint8_t id[10];
    }b;
 }GUI_DEVINFO, *PGUI_DEVINFO;
#pragma pack(pop)



class XC_Display_ST7789 : public XC_DisplayDriver
{
public:
    XC_Display_ST7789(
        SPIClass &spi,
        int csPin,
        int dcPin,
        int rstPin,
        int blPin,
        int width,
        int height,
        uint32_t spiHz = 25000000UL,
        int xOffset = 0,
        int yOffset = 0
    );

    bool begin() override;

    int16_t width() const override;
    int16_t height() const override;
    int16_t offset() const ;
    int controllerId() const override;
    XC_PIXEL_FORMAT pixelFormat() const override;

    void reset() override;
    void setOffset(int xOffset, int yOffset);
    void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override;

    void pushPixels(const uint16_t *pixels, uint32_t count) override;
    //void fillPixels(uint16_t color, uint32_t count) override;

    void setRotation(uint8_t rotation) override;
    void setBacklight(bool on) override;

private:
    SPIClass *_spi;
    SPISettings _settings;

    int _cs;
    int _dc;
    int _rst;
    int _bl;

    int _width;
    int _height;
    int _xOffset;
    int _yOffset;
    uint8_t _rotation;
    uint32_t _spiHz;


    void select();
    void deselect();

    void writeCommand(uint8_t cmd);
    void writeData8(uint8_t data);
    void writeData(const uint8_t *data, uint32_t len);
    void writeCommandData(uint8_t cmd, const uint8_t *data, uint32_t len);

    GUI_DEVINFO  g_DevInfo;
    uint8_t EEPROM_readStatus( void );
    uint8_t EEPROM_readByte(uint16_t address);
    uint8_t EEPROM_readBytes(uint16_t address, uint8_t *pBuf, uint32_t n);
    void EEPROM_writeEnable( void );
    void EEPROM_writeByte(uint16_t address, uint8_t payload);
    void EEPROM_writeBytes(uint16_t address, uint8_t *payload, uint32_t n);
};


