# XC_GUI Arduino Library

XC_GUI is a lightweight Arduino graphics library for small embedded displays. It provides buffered drawing primitives, text rendering, bitmap support, and a display-driver abstraction for TFT and OLED modules.

This release is distributed as a precompiled Arduino library. Only the public headers are included; the GUI core and bundled display drivers are supplied as static libraries.

## LCD Compatibility

XC_GUI is designed to work with Gigasitron-designed LCD modules.

Although the driver names may match common LCD controller names such as `ST7789` or `SSD1306`, compatibility with third-party LCD modules is not guaranteed. LCD modules using the same controller can still require different initialization commands, offsets, scan direction, color order, timing, backlight control, or EEPROM/device information handling.

XC_GUI helps users configure supported Gigasitron LCD modules automatically based on the LCD design and stored display information. For best results, use XC_GUI with Gigasitron LCD hardware.

## Features

- Buffered drawing through `XC_GUI`
- Public C++ API for Arduino sketches
- Display abstraction through `XC_DisplayDriver`
- Built-in support for RGB565 TFT and 1-bit monochrome OLED display paths
- ST7789 SPI display driver
- SSD1306 I2C display driver
- Drawing primitives: line, horizontal line, vertical line, rectangle, filled rectangle, circle, filled circle
- Text rendering with `XC_FONT`
- Bitmap drawing support
- Precompiled archives for Cortex-M0+, Cortex-M3, Cortex-M4, Cortex-M7, and ESP32-family targets

## Library Layout

```text
XC_GUI/
├── library.properties
├── README.md
├── src/
│   ├── XC_GUI.h
│   ├── XC_GUI_Types.h
│   ├── XC_DisplayDriver.h
│   ├── XC_Display_SSD1306.h
│   ├── XC_Display_ST7789.h
│   ├── cortex-m0plus/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── cortex-m3/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── cortex-m4/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── cortex-m7/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── esp32/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── esp32c3/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── esp32s2/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   ├── esp32s3/
│   │   ├── libXC_GUI.a
│   │   ├── libXC_SSD1306.a
│   │   └── libXC_ST7789.a
│   └── esp32h2/
│       ├── libXC_GUI.a
│       ├── libXC_SSD1306.a
│       └── libXC_ST7789.a
└── examples/
    ├── SSD1306_XGUI_Basic_XC_96A/
    └── SSD1306_XGUI_Basic_XC_IPS_2A/
```

Do not rename the `src/cortex-*` or `src/esp32*` folders. Arduino uses these folder names to select the correct precompiled archive for the selected board.

## Supported Targets

This package currently includes archives for these ARM Cortex and ESP32-family targets:

| Folder | Typical target |
|---|---|
| `cortex-m0plus` | RP2040, SAMD21, and other Cortex-M0+ boards |
| `cortex-m3` | Cortex-M3 Arduino-compatible boards |
| `cortex-m4` | STM32F4, STM32G4, STM32L4, and other Cortex-M4 boards |
| `cortex-m7` | STM32F7, STM32H7, and other Cortex-M7 boards |
| `esp32` | ESP32 boards |
| `esp32c3` | ESP32-C3 boards |
| `esp32s2` | ESP32-S2 boards |
| `esp32s3` | ESP32-S3 boards |
| `esp32h2` | ESP32-H2 boards |

The precompiled libraries are built using the default Arduino IDE board settings. Select the correct board in Arduino IDE before compiling the examples.

## Not Supported

Classic Arduino AVR boards, such as Arduino Uno and Arduino Mega, are not considered for this precompiled release.

These boards have much smaller RAM and flash memory, and their clock speed is significantly lower than the supported ARM Cortex and ESP32-family targets. For this reason, XC_GUI is focused on boards with enough memory and processing performance for buffered graphics.

## Installation

1. Download or clone this repository.
2. Copy the `XC_GUI` folder into your Arduino libraries folder.
3. Restart Arduino IDE if it was already open.
4. Open one of the examples from `File > Examples > XC_GUI`.
5. Select the correct board and upload.

## Basic Usage

Include the GUI header and the display driver header required by your module:

```cpp
#include <XC_GUI.h>
#include <XC_Display_SSD1306.h>
```

Create the display driver, attach it to `XC_GUI`, then call `drawExecute()` whenever you want the next frame to be rendered.

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <XC_GUI.h>
#include <XC_Display_SSD1306.h>

XC_Display_SSD1306 oled(Wire, 0x3C, 128, 64);
XC_GUI gui;

uint8_t guiBuffer[1024];

void setup()
{
    Wire.begin();
    Wire.setClock(400000);

    gui.attachDriver(&oled);

    if (!gui.begin(guiBuffer, sizeof(guiBuffer), 128, 64))
    {
        while (1) {}
    }

    gui.clear(XC_Black);
    gui.drawRect(0, 0, 127, 63, XC_White);
    gui.drawCircle(64, 32, 20, XC_White);
    gui.printString("XC_GUI", XC_White, 20, 24);
    gui.drawExecuteBlocking();
}

void loop()
{
}
```

## Drawing With `onDraw()`

For animation or repeated drawing, subclass `XC_GUI` and override `onDraw()`:

```cpp
class MyGUI : public XC_GUI
{
public:
    void onDraw(bool frameStart) override
    {
        clear(XC_Black);
        drawRect(0, 0, 127, 63, XC_White);
        drawLine(0, 0, 127, 63, XC_White);
    }
};
```

Then call:

```cpp
gui.drawExecute();
```

from `loop()` or from a timer-driven flag. Do not draw directly inside interrupt context.

## ST7789 SPI Example

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <XC_GUI.h>
#include <XC_Display_ST7789.h>

#define TFT_CS   PB13
#define TFT_DC   PE8
#define TFT_RST  PE6
#define TFT_BL   PE0

XC_Display_ST7789 lcd(
    SPI,
    TFT_CS,
    TFT_DC,
    TFT_RST,
    TFT_BL,
    240,
    320,
    25000000UL
);

XC_GUI gui;
uint8_t guiBuffer[4096 * 3];

void setup()
{
    SPI.setMOSI(PB15);
    SPI.setMISO(PB14);
    SPI.setSCLK(PD3);

    lcd.begin();
    lcd.setRotation(0);
    lcd.setBacklight(true);

    gui.attachDriver(&lcd);
    gui.begin(guiBuffer, sizeof(guiBuffer), 240, 320);
}
```

Change the pin names to match your selected board.

## Public API Summary

Main class:

```cpp
XC_GUI
```

Important methods:

```cpp
attachDriver()
begin()
drawExecute()
drawExecuteBlocking()
onDraw()
clear()
setColor()
setColor16()
setPenWidth()
drawLine()
drawHLine()
drawVLine()
drawRect()
fillRect()
drawCircle()
fillCircle()
setFont()
setFontBackColorEnable()
setFontBackColor()
setFontBackColor16()
printChar()
printString()
drawBitmap()
```

Display driver base class:

```cpp
XC_DisplayDriver
```

Included display drivers:

```cpp
XC_Display_SSD1306
XC_Display_ST7789
```

These drivers are intended for Gigasitron-designed LCD modules. A third-party display with the same controller name may not work without a compatible driver or configuration.

## Fonts

Fonts use the public `XC_FONT` structure. The examples include `FONT_FjallaOne48.c` as a sketch-local font file.

A free Gigasitron Font Converter will be provided in this GitHub repository. You can use it to convert your own fonts into the `XC_FONT` format used by XC_GUI.

Please make sure you only convert fonts that you have the right to use. For most projects, it is recommended to use Google Fonts or other open-source fonts with a clear license that allows your intended use.

Example declaration:

```cpp
extern const XC_FONT FONT_FjallaOne48;
```

Example usage:

```cpp
gui.setFont(&FONT_FjallaOne48);
gui.printString("123", XC_White, 1, 10);
```

## Images

XC_GUI supports bitmap drawing through `drawBitmap()`.

A free `XC_IMG_Converter` will also be provided in this GitHub repository. You can use it to convert your own image files into the XC_GUI image format for use in Arduino sketches.

Please make sure you only convert images that you own, created yourself, purchased with the correct license, or are otherwise legally allowed to use in your project.

## Font and Image Copyright

Users are responsible for checking and following the license terms of any fonts, icons, logos, photos, graphics, or images converted and used with XC_GUI.

Gigasitron provides the font and image conversion tools only as utilities. Gigasitron does not grant any rights to third-party fonts or images, and is not responsible for any copyright infringement, license violation, illegal use, or unauthorized distribution caused by user-provided fonts or images.

When in doubt, use Google Fonts or other open-source fonts and image assets with clear licensing terms, and keep a copy of the license for your project records.

## Notes for Precompiled Releases

- The source implementation is not included in this package.
- Public headers define the supported API.
- The static libraries are built using the default Arduino IDE board settings for each supported target folder.
- Keep the library folder name and archive names unchanged unless you also update `library.properties`.

## License

Copyright (c) Gigasitron.

See the repository license file for terms of use.
