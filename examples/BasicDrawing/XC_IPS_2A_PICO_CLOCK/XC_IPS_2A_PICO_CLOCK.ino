#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <atomic>
#include <Wire.h>
#include "XC_GUI.h"
#include "XC_Display_ST7789.h"
#include <sys/time.h>
#include "pico/time.h"

#ifndef ARDUINO_ARCH_RP2040
#error "This library only supports RP2040/RP2350 Arduino boards"
#endif

bool core1_separate_stack = true; // Core0 and Core1 have seperate stacks.

#define TFT_MISO  16
#define TFT_CS    17
#define TFT_SCK   18
#define TFT_MOSI  19
#define TFT_DC    20
#define TFT_RST   21
#define TFT_BL    22
#define SPI_SPEED 30000000UL


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
    0,            // x offset, it will be read from Gigasitron LCD Register
    0             // y offset, it will be read from Gigasitron LCD Register
);

const char *ssid     = "<Your SSID>";
const char *password = "<Your Password>";
std::atomic<bool> timeReady(false);


extern const XC_FONT g_FontSevenSegment32;
extern const uint16_t bmp_clock[];
extern const uint16_t bmpneedle_hour120[];
extern const uint16_t bmpneedle_min120[];
extern const uint16_t bmpneedle_sec120[];

static char          g_strLcdBuf[32];
static float secondAngle = 1.0f;
static float minuteAngle = 2.0f;
static float hourAngle   = 3.0f;

static volatile float curr_secondAngle = 0.0f;
static volatile float curr_minuteAngle = 0.0f;
static volatile float curr_hourAngle   = 0.0f;

static uint16_t g_HandTemp[11*120];
static uint16_t g_HandMinTemp[10*100];
static uint16_t g_HandHourTemp[10*80];
static struct tm localTime;
/* Small XGUI temp buffer.*/
static uint16_t guiBuffer[4096*4]; 

static volatile bool g_bLcdTick = false;
static volatile int g_RtcTick = 50;
static volatile bool g_bRtcTickReady = false;




struct repeating_timer guiTimer;

class MyGUI : public XC_GUI
{
public:
    void onDraw(bool frameStart) override
    {
        if( true == frameStart )
        {
            int sec  = localTime.tm_sec;
            int min  = localTime.tm_min;
            int hour = localTime.tm_hour;
            
            sprintf(g_strLcdBuf, "%02d:%02d:%02d", hour, min, sec);
            secondAngle = curr_secondAngle;

            minuteAngle = curr_minuteAngle;

            hourAngle   = curr_hourAngle;
        }
	
        clear(XC_Black);
 
        drawBitmap( bmp_clock, 0, 0 );

        drawBitmapRotatedTransparentAt(
            bmpneedle_hour120,
            118,
            124,
            5,
            12,
            hourAngle + 180.0f,
            0x0000,
            2,
            g_HandHourTemp,
            10*80 );

        drawBitmapRotatedTransparentAt(
            bmpneedle_min120,
            118,
            124,
            4,
            14,
            minuteAngle + 180.0f,
            0x0000,
            2,
            g_HandMinTemp,
            10*100);

        drawBitmapRotatedTransparentAt(
            bmpneedle_sec120,
            118,
            124,
            5,
            15,
            secondAngle + 180.0f,
            0x0000,
            2,
            g_HandTemp,
            11*120 );

        printString( g_strLcdBuf, XC_White, 60 , 260);
    }
};

MyGUI gui;


bool xcGuiTimerCallback(struct repeating_timer *t)
{
    (void)t;
    g_bLcdTick = true;

    
        if(0 != g_RtcTick)
        {
            g_RtcTick--;
            if(0 == g_RtcTick)
            {
                g_RtcTick = 50;
                g_bRtcTickReady = true;
            }
        }

    return true;   // keep repeating
}


void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected");
    // Singapore timezone: UTC+8
    // POSIX TZ uses the reversed sign.
    setenv("TZ", "SGT-8", 1);
    tzset();

    // Connect to NTP Time Server
    NTP.begin( "pool.ntp.org", "time.nist.gov" );

    Serial.println("Waiting for NTP...");

    if (NTP.waitSet(15000))
    {
        Serial.println("NTP time synchronized");

        time_t now = time(nullptr);
        Serial.printf("Epoch time: %lld\n", (long long)now);

        timeReady.store(true, std::memory_order_release);
    }
    else
    {
        Serial.println("NTP synchronization failed");
    }

    // SPI PINS

   // pinMode(25, OUTPUT); // pico LED
    SPI.setTX(TFT_MOSI); //MOSI
    SPI.setRX(TFT_MISO);
    SPI.setSCK(TFT_SCK);

    // Start LCD initialization.
    lcd.begin();
    lcd.setRotation(0);
    lcd.setBacklight(true);

    // Apply LCD Pixel offset from manufacturer. It is stored in Gigasitron LCD that can be read out
    lcd.setOffset(lcd.offset(), 0); 

    Serial.println(lcd.controllerId());
    gui.attachDriver(&lcd);
    gui.set16BitPerPixel(false);
    if (!gui.begin(guiBuffer, sizeof(guiBuffer), TFT_WIDTH, TFT_HEIGHT))
    {
        Serial.println("GUI begin failed");
        while (1) {}
    }

    gui.setFont( &g_FontSevenSegment32 );
    gui.setFontBackColor(XC_Red);
    gui.setFontBackColorEnable(0);

    add_repeating_timer_ms(10, xcGuiTimerCallback, nullptr, &guiTimer);

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

    }
}

void setup1()
{
     while (!timeReady.load(std::memory_order_acquire))
    {
        delay(1);
    }
}

void loop1()
{
    char buffer[32];
    int prev_sc = 0;
    
    while(true)
    {
        time_t now = time(nullptr);
        if (localtime_r(&now, &localTime) == nullptr)
        {
            delay(10);
            return;
        }

        if(prev_sc != localTime.tm_sec)
        {
            prev_sc = localTime.tm_sec;
            curr_secondAngle = (float)(localTime.tm_sec*6uL);

            curr_minuteAngle = (float)(localTime.tm_min * 6) + (float)(localTime.tm_sec * 1)/10.0f;

            curr_hourAngle   = (float)((localTime.tm_hour % 12) * 30) + (float)(localTime.tm_min * 5)/10.0f + (float)localTime.tm_sec * (0.5f / 60.0f);
            #if 0
            strftime(
                buffer,
                sizeof(buffer),
                "%Y-%m-%d %H:%M:%S",
                &localTime );
                Serial.println(buffer);
            #endif
        }

        delay(900);
    }
   
}


