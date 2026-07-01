#pragma once

#include <stdint.h>

/*
 * Public XC_GUI types.
 *
 * This file intentionally does NOT include GUI/XGUI.h.
 * The old C core remains hidden inside XC_GUI.cpp.
 */

typedef uint16_t XC_DATA;

/*
 * Pixel format / color depth.
 * Values are equal to bits-per-pixel.
 */
typedef enum
{
    XC_PIXEL_RGB565 = 16,
    XC_PIXEL_MONO1  = 1
} XC_PIXEL_FORMAT;

/*
 * XC_COLOR values must stay in the same order as XGUI_COLOR in GUI/XGUI.h.
 * XC_GUI.cpp casts XC_COLOR to XGUI_COLOR internally.
 */
typedef enum
{
    XC_AliceBlue = 0,
    XC_AntiqueWhite,
    XC_Aqua,
    XC_Aquamarine,
    XC_Azure,
    XC_Beige,
    XC_Bisque,
    XC_Black,
    XC_BlanchedAlmond,
    XC_Blue,
    XC_BlueViolet,
    XC_Brown,
    XC_BurlyWood,
    XC_CadetBlue,
    XC_Chartreuse,
    XC_Chocolate,
    XC_Coral,
    XC_CornflowerBlue,
    XC_Cornsilk,
    XC_Crimson,
    XC_Cyan,
    XC_DarkBlue,
    XC_DarkCyan,
    XC_DarkGoldenrod,
    XC_DarkGray,
    XC_DarkGreen,
    XC_DarkKhaki,
    XC_DarkMagenta,
    XC_DarkOliveGreen,
    XC_DarkOrange,
    XC_DarkOrchid,
    XC_DarkRed,
    XC_DarkSalmon,
    XC_DarkSeaGreen,
    XC_DarkSlateBlue,
    XC_DarkSlateGray,
    XC_DarkTurquoise,
    XC_DarkViolet,
    XC_DeepPink,
    XC_DeepSkyBlue,
    XC_DimGray,
    XC_DodgerBlue,
    XC_FireBrick,
    XC_FloralWhite,
    XC_ForestGreen,
    XC_Fuchsia,
    XC_Gainsboro,
    XC_GhostWhite,
    XC_Gold,
    XC_Goldenrod,
    XC_Gray,
    XC_Green,
    XC_GreenYellow,
    XC_Honeydew,
    XC_HotPink,
    XC_IndianRed,
    XC_Indigo,
    XC_Ivory,
    XC_Khaki,
    XC_Lavender,
    XC_LavenderBlush,
    XC_LawnGreen,
    XC_LemonChiffon,
    XC_LightBlue,
    XC_LightCoral,
    XC_LightCyan,
    XC_LightGoldenrodYellow,
    XC_LightGreen,
    XC_LightGrey,
    XC_LightPink,
    XC_LightSalmon,
    XC_LightSeaGreen,
    XC_LightSkyBlue,
    XC_LightSlateGray,
    XC_LightSteelBlue,
    XC_LightYellow,
    XC_Lime,
    XC_LimeGreen,
    XC_Linen,
    XC_Magenta,
    XC_Maroon,
    XC_MediumAquamarine,
    XC_MediumBlue,
    XC_MediumOrchid,
    XC_MediumPurple,
    XC_MediumSeaGreen,
    XC_MediumSlateBlue,
    XC_MediumSpringGreen,
    XC_MediumTurquoise,
    XC_MediumVioletRed,
    XC_MidnightBlue,
    XC_MintCream,
    XC_MistyRose,
    XC_Moccasin,
    XC_NavajoWhite,
    XC_Navy,
    XC_OldLace,
    XC_Olive,
    XC_OliveDrab,
    XC_Orange,
    XC_OrangeRed,
    XC_Orchid,
    XC_PaleGoldenrod,
    XC_PaleGreen,
    XC_PaleTurquoise,
    XC_PaleVioletRed,
    XC_PapayaWhip,
    XC_PeachPuff,
    XC_Peru,
    XC_Pink,
    XC_Plum,
    XC_PowderBlue,
    XC_Purple,
    XC_Red,
    XC_RosyBrown,
    XC_RoyalBlue,
    XC_SaddleBrown,
    XC_Salmon,
    XC_SandyBrown,
    XC_SeaGreen,
    XC_Seashell,
    XC_Sienna,
    XC_Silver,
    XC_SkyBlue,
    XC_SlateBlue,
    XC_SlateGray,
    XC_Snow,
    XC_SpringGreen,
    XC_SteelBlue,
    XC_Tan,
    XC_Teal,
    XC_Thistle,
    XC_Tomato,
    XC_Turquoise,
    XC_Violet,
    XC_Wheat,
    XC_White,
    XC_WhiteSmoke,
    XC_Yellow,
    XC_YellowGreen
} XC_COLOR;


typedef int XC_GUI_STATUS;

#define  XC_GUI_OK              0
#define  XC_GUI_ERR_MEM         -1
#define  XC_GUI_ERR_LCD_SIZE    -2
#define XC_GUI_ERR_LCD_INIT     -3
/*
 * Public font layout.
 * This mirrors GUI_FONT from the C core but avoids including XGUI.h.
 */
typedef struct
{
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned short array[96];
    const unsigned char *f;
} XC_FONT;

/*
 * Optional C callback type exposed under XC naming.
 */
typedef void XC_FRAME_END_CALLBACK(void);
typedef void XC_BLOCK_END_CALLBACK(void);
