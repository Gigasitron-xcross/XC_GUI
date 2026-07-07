#include <Arduino.h>
#include <Wire.h>
#include "XC_GUI.h"
#include "XC_Display_ST7789.h"

#if defined (ARDUINO_ARCH_RP2040)
    #define TFT_MISO  16
    #define TFT_CS    17
    #define TFT_SCK   18
    #define TFT_MOSI  19
    #define TFT_DC    20
    #define TFT_RST   21
    #define TFT_BL    22
    #define SPI_SPEED 25000000UL
#endif

#if defined(ARDUINO_ARCH_STM32)
// ST7789 pins
    #define TFT_CS   PB13
    #define TFT_DC   PE8
    #define TFT_RST  PE6
    #define TFT_BL   PD4
    #define TFT_MISO  PC2
    #define TFT_SCK   PD3
    #define TFT_MOSI  PB15
    #define SPI_SPEED 25000000UL
#endif

// For 240x320 ST7789
#define TFT_WIDTH   240
#define TFT_HEIGHT  320

XC_Display_ST7789 lcd(
    SPI,
    TFT_CS,
    TFT_DC,
    TFT_RST,
    TFT_BL,
    TFT_WIDTH,
    TFT_HEIGHT,
    SPI_SPEED,   // SPI clock
    0,            // x offset
    0             // y offset
);


static double		 g_nCount = 0;
static int		     g_nMove	 = 0;
static int           dir      = 1;
static char          buf[32];
static char          g_bLcd240x320 = false;
extern const XC_FONT g_sFontCalibri10;
extern const XC_FONT g_FontOpenSansLight48;
extern const XC_FONT FONT_FjallaOne48;
extern const uint16_t bmpgiga_splash[];
extern const uint16_t bmpGIGA_240x320[];

class MyGUI : public XC_GUI
{
public:
    void onDraw(bool frameStart) override
    {
        if( true == frameStart )
        {
            g_nCount += 0.1;
            if(g_nCount >= 99)
            {
            g_nCount = 0.0;
            }
            sprintf(buf, "%d", g_nMove);
            g_nMove+=dir;

            if(g_nMove<=1)
            {
                dir = 1;
            }

            if(g_nMove>20)
            {
                dir = -1;
            }
        }
	

        clear(XC_Black);
        if(true == g_bLcd240x320)
        {
            drawBitmap( bmpGIGA_240x320, 0, 0 );
        }
        else
        {
            drawBitmap( bmpgiga_splash, 0, 60 );
        }

        setPenWidth(1);
        setFont( &FONT_FjallaOne48 );
        setFontBackColorEnable(0);
        printString( buf, XC_White, 8 , 10 );

        drawRect(8, 8, 172-8, 72, XC_White);
        drawCircle(77,37,1+g_nMove, XC_White);
        fillCircle(120,37,1+g_nMove, XC_White);
    }
};

MyGUI gui;

/* Small XGUI temp buffer.*/
uint8_t guiBuffer[4096*4]; 
#define LCD_TICK_MS 100

static volatile bool g_bLcdTick = false;
static volatile int g_bLcdTickCount = LCD_TICK_MS;

#if defined(ARDUINO_ARCH_STM32)
static HardwareTimer *MyTimer = nullptr;

void onTimer()
{
    /*
     * This is interrupt context.
     * Keep it short.
     * Do not use Serial.print() here.
     * Do not do SPI LCD drawing here.
     */
    g_bLcdTick = true;
}
#endif

#if defined(ARDUINO_ARCH_AVR)
#include <avr/io.h>
#include <avr/interrupt.h>

void XC_Timer_Init()
{
    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    // 16 MHz / 64 = 250 kHz
    // 250 kHz / 1000 Hz = 250 counts
    OCR1A = 249;

    TCCR1B |= (1 << WGM12);              // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler 64
    TIMSK1 |= (1 << OCIE1A);             // enable compare A interrupt

    sei();
}

ISR(TIMER1_COMPA_vect)
{
    g_bLcdTick = true;
}
#endif

#if defined (ARDUINO_ARCH_RP2040)
#include "pico/time.h"
struct repeating_timer guiTimer;

bool xcGuiTimerCallback(struct repeating_timer *t)
{
    (void)t;
    g_bLcdTick = true;
    return true;   // keep repeating
}
#endif

void setup()
{
    Serial.begin(115200);

    // SPI PINS
    #if defined (ARDUINO_ARCH_RP2040)
    pinMode(25, OUTPUT); // pico LED
    SPI.setTX(TFT_MOSI); //MOSI
    SPI.setRX(TFT_MISO);
    SPI.setSCK(TFT_SCK);
    #endif

    #if defined(ARDUINO_ARCH_STM32)
    SPI.setMOSI(TFT_MOSI);
    SPI.setMISO(TFT_MISO);
    SPI.setSCLK(TFT_SCK);
    #endif

    // Start LCD initialization.
    lcd.begin();
    lcd.setRotation(0);
    lcd.setBacklight(true);

    // Apply LCD Pixel offset from manufacturer. It is stored in Gigasitron LCD that can be read out
    lcd.setOffset(lcd.offset(), 0); 

    if((lcd.width()*lcd.height()) >= (240*230) )
    {
        g_bLcd240x320 = true;
    }

    Serial.println(lcd.controllerId());
    gui.attachDriver(&lcd);
    gui.set16BitPerPixel(false);
    if (!gui.begin(guiBuffer, sizeof(guiBuffer), TFT_WIDTH, TFT_HEIGHT))
    {
        Serial.println("GUI begin failed");
        while (1) {}
    }

#if defined(ARDUINO_ARCH_STM32)
    MyTimer = new HardwareTimer(TIM2);
    MyTimer->setOverflow(100, HERTZ_FORMAT);
    MyTimer->attachInterrupt(onTimer);
    MyTimer->resume();
#elif defined(ARDUINO_ARCH_AVR)

    XC_Timer_Init();

#elif defined(ARDUINO_ARCH_RP2040)
    add_repeating_timer_ms(10, xcGuiTimerCallback, nullptr, &guiTimer);
    digitalWrite(25, HIGH);
#endif
    // Turn on backlite
    digitalWrite(TFT_BL, HIGH);
}

void loop()
{
    if(0 != g_bLcdTick)
    {
        g_bLcdTick = false;

        //Execute GUI periodically
        gui.drawExecute();

        #if defined(ARDUINO_ARCH_RP2040)
            digitalWrite(25, !digitalRead(25));
        #endif
    }
}
