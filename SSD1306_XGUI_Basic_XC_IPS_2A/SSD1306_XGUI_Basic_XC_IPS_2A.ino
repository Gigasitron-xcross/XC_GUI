#include <Arduino.h>
#include <Wire.h>
#include "XC_GUI.h"
#include "XC_Display_ST7789.h"


// ST7789 pins
#define TFT_CS   PB13
#define TFT_DC   PE8
#define TFT_RST  PE6
#define TFT_BL   PE0

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
    25000000UL,   // SPI clock
    0,            // x offset
    0             // y offset
);


static double				g_nCount = 0;
static int8_t				g_nMove	 = 0;
static int8_t               dir      = 1;
static char buf[32];

extern const XC_FONT g_sFontCalibri10;
extern const XC_FONT g_FontOpenSansLight48;
extern const XC_FONT FONT_FjallaOne48;

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
        setPenWidth(1);
         setFont( &FONT_FjallaOne48 );
        setFontBackColorEnable(0);
        printString( buf, XC_White, 1 , 10 );

        drawRect(0, 0, 127, 63, XC_White);
        drawCircle(64,32,1+g_nMove, XC_White);
        fillCircle(100,32,1+g_nMove, XC_White);
        //drawLine(0, 0, 127, 63, XC_White);
        //drawLine(127, 0, 0, 63, XC_White);
        //printString("SSD1306", XC_White, 10, 24);
    }
};

MyGUI gui;

/* Small XGUI temp buffer. Latest SSD1306 driver uses native full-screen buffer internally. */
uint8_t guiBuffer[4096*3];  // 1 SSD1306 page = 128 bytes = 8 rows
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
    //Serial.begin(115200);
    SPI.setMOSI(PB15);
SPI.setMISO(PB14);
SPI.setSCLK(PD3);
    lcd.begin();
    lcd.setRotation(0);
    lcd.setBacklight(true);
    

    gui.attachDriver(&lcd);

    if (!gui.begin(guiBuffer, sizeof(guiBuffer), TFT_WIDTH, TFT_HEIGHT))
    {
        //Serial.println("GUI begin failed");
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
    add_repeating_timer_ms(50, xcGuiTimerCallback, nullptr, &guiTimer);
#endif
}

void loop()
{
    if(0 != g_bLcdTick)
    {
         
        g_bLcdTick = false;
        gui.drawExecute();
       
    }
}
