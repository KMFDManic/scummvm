#ifndef MSN_DEF_H
#define MSN_DEF_H

namespace Supernova {

const int kScreenWidth  = 320;
const int kScreenHeight = 200;
const int kTextSpeed[] = {19, 14, 10, 7, 4};

const int kMaxSection = 40;
const int kMaxSectionZus = 50;
const int kMaxClickfieldZus = 80;
const int kMaxObject = 25;
const int kMaxCarry = 30;

const int kRoomsChap0 =  1;
const int kRoomsChap1 = 16;
const int kRoomsChap2 =  9;
const int kRoomsChap3 = 21;
const int kRoomsNum = kRoomsChap0 + kRoomsChap1 + kRoomsChap2 + kRoomsChap3;

enum MessagePosition {
	kMessageNormal,
	kMessageLeft,
	kMessageRight,
	kMessageCenter,
	kMessageTop
};

// Colors
const int kColorBlack       =  0;
const int kColorWhite25     =  1;
const int kColorWhite35     =  2;
const int kColorWhite44     =  3;
const int kColorWhite99     =  4;
const int kColorDarkGreen   =  5;
const int kColorGreen       =  6;
const int kColorDarkRed     =  7;
const int kColorRed         =  8;
const int kColorDarkBlue    =  9;
const int kColorBlue        = 10;
const int kColorWhite63     = 11;
const int kColorLightBlue   = 12;
const int kColorLightGreen  = 13;
const int kColorLightYellow = 14;
const int kColorLightRed    = 15;

#define HGR_BEF      kColorWhite25
#define COL_BEF      kColorDarkGreen
#define HGR_BEF_HELL kColorWhite44
#define COL_BEF_HELL kColorGreen

#define HGR_INV      kColorWhite25
#define COL_INV      kColorDarkRed
#define HGR_INV_HELL kColorWhite35
#define COL_INV_HELL kColorRed

#define HGR_BEF_ANZ  kColorWhite25
#define COL_BEF_ANZ  kColorDarkGreen

#define HGR_AUSG     kColorWhite25
#define COL_AUSG     kColorDarkRed

#define HGR_MELD     kColorWhite35
#define COL_MELD     kColorWhite99

#define COL_LWIN     kColorDarkRed
#define HGR_LADEN    kColorBlue
#define COL_LADEN    kColorWhite99
#define HGR_LADEN2   kColorDarkBlue
#define COL_LADEN2   kColorWhite63
#define HGR_NAME     kColorDarkBlue
#define COL_NAME     kColorLightRed

#define HGR_EDIT     kcolorDarkBlue
#define COL_EDIT     kColorWhite99


const byte mouseNormal[64] = {
    0xff,0x3f,0xff,0x1f,0xff,0x0f,0xff,0x07,
    0xff,0x03,0xff,0x01,0xff,0x00,0x7f,0x00,
    0x3f,0x00,0x1f,0x00,0x0f,0x00,0x0f,0x00,
    0xff,0x00,0x7f,0x18,0x7f,0x38,0x7f,0xfc,

    0x00,0x00,0x00,0x40,0x00,0x60,0x00,0x70,
    0x00,0x78,0x00,0x7c,0x00,0x7e,0x00,0x7f,
    0x80,0x7f,0xc0,0x7f,0xe0,0x7f,0x00,0x7e,
    0x00,0x66,0x00,0x43,0x00,0x03,0x00,0x00
};

const byte mouseWait[64] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,
    0x01,0x80,0x01,0x80,0x11,0x88,0x31,0x8c,
    0x31,0x8c,0x11,0x88,0x01,0x80,0x01,0x80,
    0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,

    0x00,0x00,0xfe,0x7f,0xf4,0x2f,0xf4,0x2f,
    0x14,0x28,0x24,0x24,0x44,0x22,0x84,0x21,
    0x84,0x21,0xc4,0x23,0xe4,0x27,0x74,0x2e,
    0x34,0x2c,0x14,0x28,0xfe,0x7f,0x00,0x00
};

const byte font[][5] = {
    {0x00,0x00,0x00,0xff,0x00},
    {0x5f,0xff,0x00,0x00,0x00},
    {0x03,0x00,0x03,0xff,0x00},
    {0x14,0x7f,0x14,0x7f,0x14},
    {0x24,0x2a,0x7f,0x2a,0x12},
    {0x61,0x10,0x08,0x04,0x43},
    {0x38,0x4e,0x59,0x26,0x50},
    {0x03,0xff,0x00,0x00,0x00},
    {0x3e,0x41,0xff,0x00,0x00},
    {0x41,0x3e,0xff,0x00,0x00},
    {0x10,0x54,0x38,0x54,0x10},
    {0x10,0x10,0x7c,0x10,0x10},
    {0x80,0x40,0xff,0x00,0x00},
    {0x10,0x10,0x10,0x10,0x10},
    {0x40,0xff,0x00,0x00,0x00},
    {0x60,0x10,0x08,0x04,0x03},

    {0x3e,0x41,0x41,0x41,0x3e}, /* digits */
    {0x04,0x02,0x7f,0xff,0x00},
    {0x42,0x61,0x51,0x49,0x46},
    {0x22,0x41,0x49,0x49,0x36},
    {0x18,0x14,0x12,0x7f,0x10},
    {0x27,0x45,0x45,0x45,0x39},
    {0x3e,0x49,0x49,0x49,0x32},
    {0x01,0x61,0x19,0x07,0x01},
    {0x36,0x49,0x49,0x49,0x36},
    {0x26,0x49,0x49,0x49,0x3e},

    {0x44,0xff,0x00,0x00,0x00},
    {0x80,0x44,0xff,0x00,0x00},
    {0x10,0x28,0x44,0xff,0x00},
    {0x28,0x28,0x28,0x28,0x28},
    {0x44,0x28,0x10,0xff,0x00},
    {0x02,0x01,0x51,0x09,0x06},
    {0x3e,0x41,0x5d,0x5d,0x1e},

    {0x7c,0x12,0x11,0x12,0x7c}, /* uppercase letters*/
    {0x7f,0x49,0x49,0x49,0x36},
    {0x3e,0x41,0x41,0x41,0x22},
    {0x7f,0x41,0x41,0x22,0x1c},
    {0x7f,0x49,0x49,0x49,0xff},
    {0x7f,0x09,0x09,0x09,0xff},
    {0x3e,0x41,0x41,0x49,0x3a},
    {0x7f,0x08,0x08,0x08,0x7f},
    {0x41,0x7f,0x41,0xff,0x00},
    {0x20,0x40,0x40,0x3f,0xff},
    {0x7f,0x08,0x14,0x22,0x41},
    {0x7f,0x40,0x40,0x40,0xff},
    {0x7f,0x02,0x04,0x02,0x7f},
    {0x7f,0x02,0x0c,0x10,0x7f},
    {0x3e,0x41,0x41,0x41,0x3e},
    {0x7f,0x09,0x09,0x09,0x06},
    {0x3e,0x41,0x51,0x21,0x5e},
    {0x7f,0x09,0x19,0x29,0x46},
    {0x26,0x49,0x49,0x49,0x32},
    {0x01,0x01,0x7f,0x01,0x01},
    {0x3f,0x40,0x40,0x40,0x3f},
    {0x07,0x18,0x60,0x18,0x07},
    {0x1f,0x60,0x18,0x60,0x1f},
    {0x63,0x14,0x08,0x14,0x63},
    {0x03,0x04,0x78,0x04,0x03},
    {0x61,0x51,0x49,0x45,0x43},

    {0x7f,0x41,0x41,0xff,0x00},
    {0x03,0x04,0x08,0x10,0x60},
    {0x41,0x41,0x7f,0xff,0x00},
    {0x02,0x01,0x02,0xff,0x00},
    {0x80,0x80,0x80,0x80,0x80},
    {0x01,0x02,0xff,0x00,0x00},

    {0x38,0x44,0x44,0x44,0x7c}, /* lowercase letters */
    {0x7f,0x44,0x44,0x44,0x38},
    {0x38,0x44,0x44,0x44,0x44},
    {0x38,0x44,0x44,0x44,0x7f},
    {0x38,0x54,0x54,0x54,0x58},
    {0x04,0x7e,0x05,0x01,0xff},
    {0x98,0xa4,0xa4,0xa4,0x7c},
    {0x7f,0x04,0x04,0x04,0x78},
    {0x7d,0xff,0x00,0x00,0x00},
    {0x80,0x80,0x7d,0xff,0x00},
    {0x7f,0x10,0x28,0x44,0xff},
    {0x7f,0xff,0x00,0x00,0x00},
    {0x7c,0x04,0x7c,0x04,0x78},
    {0x7c,0x04,0x04,0x04,0x78},
    {0x38,0x44,0x44,0x44,0x38},
    {0xfc,0x24,0x24,0x24,0x18},
    {0x18,0x24,0x24,0x24,0xfc},
    {0x7c,0x08,0x04,0x04,0xff},
    {0x48,0x54,0x54,0x54,0x24},
    {0x04,0x3e,0x44,0x40,0xff},
    {0x7c,0x40,0x40,0x40,0x3c},
    {0x0c,0x30,0x40,0x30,0x0c},
    {0x3c,0x40,0x3c,0x40,0x3c},
    {0x44,0x28,0x10,0x28,0x44},
    {0x9c,0xa0,0xa0,0xa0,0x7c},
    {0x44,0x64,0x54,0x4c,0x44},

    {0x08,0x36,0x41,0xff,0x00},
    {0x77,0xff,0x00,0x00,0x00},
    {0x41,0x36,0x08,0xff,0x00},
    {0x02,0x01,0x02,0x01,0xff},
    {0xff,0x00,0x00,0x00,0x00},

    {0xfe,0x49,0x49,0x4e,0x30}, /* sharp S */

    {0x7c,0x41,0x40,0x41,0x3c}, /* umlauts */

    {0x04,0x06,0x7f,0x06,0x04}, /* arrows */
    {0x20,0x60,0xfe,0x60,0x20},

    {0x38,0x45,0x44,0x45,0x7c}, /* umlauts */
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0x79,0x14,0x12,0x14,0x79},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0x38,0x45,0x44,0x45,0x38},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0xff,0x00,0x00,0x00,0x00},
    {0x3d,0x42,0x42,0x42,0x3d},
    {0x3d,0x40,0x40,0x40,0x3d},
};

// Default palette
const byte initVGAPalette[768] = {
    0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x58, 0x58, 0x58, 0x70, 0x70, 0x70, 0xfc, 0xfc, 0xfc, 0x00, 0xd0, 0x00,
    0x00, 0xfc, 0x00, 0xd8, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0xb0, 0xa0, 0xa0, 0xa0,
    0x50, 0xc8, 0xfc, 0x28, 0xfc, 0x28, 0xf0, 0xf0, 0x00, 0xfc, 0x28, 0x28, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14,
    0x20, 0x20, 0x20, 0x2c, 0x2c, 0x2c, 0x38, 0x38, 0x38, 0x44, 0x44, 0x44, 0x50, 0x50, 0x50, 0x60, 0x60, 0x60,
    0x70, 0x70, 0x70, 0x80, 0x80, 0x80, 0x90, 0x90, 0x90, 0xa0, 0xa0, 0xa0, 0xb4, 0xb4, 0xb4, 0xc8, 0xc8, 0xc8,
    0xe0, 0xe0, 0xe0, 0xfc, 0xfc, 0xfc, 0x00, 0x00, 0xfc, 0x40, 0x00, 0xfc, 0x7c, 0x00, 0xfc, 0xbc, 0x00, 0xfc,
    0xfc, 0x00, 0xfc, 0xfc, 0x00, 0xbc, 0xfc, 0x00, 0x7c, 0xfc, 0x00, 0x40, 0xfc, 0x00, 0x00, 0xfc, 0x40, 0x00,
    0xfc, 0x7c, 0x00, 0xfc, 0xbc, 0x00, 0xfc, 0xfc, 0x00, 0xbc, 0xfc, 0x00, 0x7c, 0xfc, 0x00, 0x40, 0xfc, 0x00,
    0x00, 0xfc, 0x00, 0x00, 0xfc, 0x40, 0x00, 0xfc, 0x7c, 0x00, 0xfc, 0xbc, 0x00, 0xfc, 0xfc, 0x00, 0xbc, 0xfc,
    0x00, 0x7c, 0xfc, 0x00, 0x40, 0xfc, 0x7c, 0x7c, 0xfc, 0x9c, 0x7c, 0xfc, 0xbc, 0x7c, 0xfc, 0xdc, 0x7c, 0xfc,
    0xfc, 0x7c, 0xfc, 0xfc, 0x7c, 0xdc, 0xfc, 0x7c, 0xbc, 0xfc, 0x7c, 0x9c, 0xfc, 0x7c, 0x7c, 0xfc, 0x9c, 0x7c,
    0xfc, 0xbc, 0x7c, 0xfc, 0xdc, 0x7c, 0xfc, 0xfc, 0x7c, 0xdc, 0xfc, 0x7c, 0xbc, 0xfc, 0x7c, 0x9c, 0xfc, 0x7c,
    0x7c, 0xfc, 0x7c, 0x7c, 0xfc, 0x9c, 0x7c, 0xfc, 0xbc, 0x7c, 0xfc, 0xdc, 0x7c, 0xfc, 0xfc, 0x7c, 0xdc, 0xfc,
    0x7c, 0xbc, 0xfc, 0x7c, 0x9c, 0xfc, 0xb4, 0xb4, 0xfc, 0xc4, 0xb4, 0xfc, 0xd8, 0xb4, 0xfc, 0xe8, 0xb4, 0xfc,
    0xfc, 0xb4, 0xfc, 0xfc, 0xb4, 0xe8, 0xfc, 0xb4, 0xd8, 0xfc, 0xb4, 0xc4, 0xfc, 0xb4, 0xb4, 0xfc, 0xc4, 0xb4,
    0xfc, 0xd8, 0xb4, 0xfc, 0xe8, 0xb4, 0xfc, 0xfc, 0xb4, 0xe8, 0xfc, 0xb4, 0xd8, 0xfc, 0xb4, 0xc4, 0xfc, 0xb4,
    0xb4, 0xfc, 0xb4, 0xb4, 0xfc, 0xc4, 0xb4, 0xfc, 0xd8, 0xb4, 0xfc, 0xe8, 0xb4, 0xfc, 0xfc, 0xb4, 0xe8, 0xfc,
    0xb4, 0xd8, 0xfc, 0xb4, 0xc4, 0xfc, 0x00, 0x00, 0x70, 0x1c, 0x00, 0x70, 0x38, 0x00, 0x70, 0x54, 0x00, 0x70,
    0x70, 0x00, 0x70, 0x70, 0x00, 0x54, 0x70, 0x00, 0x38, 0x70, 0x00, 0x1c, 0x70, 0x00, 0x00, 0x70, 0x1c, 0x00,
    0x70, 0x38, 0x00, 0x70, 0x54, 0x00, 0x70, 0x70, 0x00, 0x54, 0x70, 0x00, 0x38, 0x70, 0x00, 0x1c, 0x70, 0x00,
    0x00, 0x70, 0x00, 0x00, 0x70, 0x1c, 0x00, 0x70, 0x38, 0x00, 0x70, 0x54, 0x00, 0x70, 0x70, 0x00, 0x54, 0x70,
    0x00, 0x38, 0x70, 0x00, 0x1c, 0x70, 0x38, 0x38, 0x70, 0x44, 0x38, 0x70, 0x54, 0x38, 0x70, 0x60, 0x38, 0x70,
    0x70, 0x38, 0x70, 0x70, 0x38, 0x60, 0x70, 0x38, 0x54, 0x70, 0x38, 0x44, 0x70, 0x38, 0x38, 0x70, 0x44, 0x38,
    0x70, 0x54, 0x38, 0x70, 0x60, 0x38, 0x70, 0x70, 0x38, 0x60, 0x70, 0x38, 0x54, 0x70, 0x38, 0x44, 0x70, 0x38,
    0x38, 0x70, 0x38, 0x38, 0x70, 0x44, 0x38, 0x70, 0x54, 0x38, 0x70, 0x60, 0x38, 0x70, 0x70, 0x38, 0x60, 0x70,
    0x38, 0x54, 0x70, 0x38, 0x44, 0x70, 0x50, 0x50, 0x70, 0x58, 0x50, 0x70, 0x60, 0x50, 0x70, 0x68, 0x50, 0x70,
    0x70, 0x50, 0x70, 0x70, 0x50, 0x68, 0x70, 0x50, 0x60, 0x70, 0x50, 0x58, 0x70, 0x50, 0x50, 0x70, 0x58, 0x50,
    0x70, 0x60, 0x50, 0x70, 0x68, 0x50, 0x70, 0x70, 0x50, 0x68, 0x70, 0x50, 0x60, 0x70, 0x50, 0x58, 0x70, 0x50,
    0x50, 0x70, 0x50, 0x50, 0x70, 0x58, 0x50, 0x70, 0x60, 0x50, 0x70, 0x68, 0x50, 0x70, 0x70, 0x50, 0x68, 0x70,
    0x50, 0x60, 0x70, 0x50, 0x58, 0x70, 0x00, 0x00, 0x40, 0x10, 0x00, 0x40, 0x20, 0x00, 0x40, 0x30, 0x00, 0x40,
    0x40, 0x00, 0x40, 0x40, 0x00, 0x30, 0x40, 0x00, 0x20, 0x40, 0x00, 0x10, 0x40, 0x00, 0x00, 0x40, 0x10, 0x00,
    0x40, 0x20, 0x00, 0x40, 0x30, 0x00, 0x40, 0x40, 0x00, 0x30, 0x40, 0x00, 0x20, 0x40, 0x00, 0x10, 0x40, 0x00,
    0x00, 0x40, 0x00, 0x00, 0x40, 0x10, 0x00, 0x40, 0x20, 0x00, 0x40, 0x30, 0x00, 0x40, 0x40, 0x00, 0x30, 0x40,
    0x00, 0x20, 0x40, 0x00, 0x10, 0x40, 0x20, 0x20, 0x40, 0x28, 0x20, 0x40, 0x30, 0x20, 0x40, 0x38, 0x20, 0x40,
    0x40, 0x20, 0x40, 0x40, 0x20, 0x38, 0x40, 0x20, 0x30, 0x40, 0x20, 0x28, 0x40, 0x20, 0x20, 0x40, 0x28, 0x20,
    0x40, 0x30, 0x20, 0x40, 0x38, 0x20, 0x40, 0x40, 0x20, 0x38, 0x40, 0x20, 0x30, 0x40, 0x20, 0x28, 0x40, 0x20,
    0x20, 0x40, 0x20, 0x20, 0x40, 0x28, 0x20, 0x40, 0x30, 0x20, 0x40, 0x38, 0x20, 0x40, 0x40, 0x20, 0x38, 0x40,
    0x20, 0x30, 0x40, 0x20, 0x28, 0x40, 0x2c, 0x2c, 0x40, 0x30, 0x2c, 0x40, 0x34, 0x2c, 0x40, 0x3c, 0x2c, 0x40,
    0x40, 0x2c, 0x40, 0x40, 0x2c, 0x3c, 0x40, 0x2c, 0x34, 0x40, 0x2c, 0x30, 0x40, 0x2c, 0x2c, 0x40, 0x30, 0x2c,
    0x40, 0x34, 0x2c, 0x40, 0x3c, 0x2c, 0x40, 0x40, 0x2c, 0x3c, 0x40, 0x2c, 0x34, 0x40, 0x2c, 0x30, 0x40, 0x2c,
    0x2c, 0x40, 0x2c, 0x2c, 0x40, 0x30, 0x2c, 0x40, 0x34, 0x2c, 0x40, 0x3c, 0x2c, 0x40, 0x40, 0x2c, 0x3c, 0x40,
    0x2c, 0x34, 0x40, 0x2c, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

enum ObjectType {
	NULLTYPE    =      0,
	TAKE        =      1,
	OPEN        =      2,
	OPENED      =      4,
	CLOSED      =      8,
	EXIT        =     16,
	PRESS       =     32,
	COMBINABLE  =     64,
	CARRIED     =    128,
	UNNECESSARY =    256,
	WORN        =    512,
	TALK        =   1024,
	OCCUPIED    =   2048,
	CAUGHT      =   4096
};

enum Action {
	ACTION_WALK,
	ACTION_LOOK,
	ACTION_TAKE,
	ACTION_OPEN,
	ACTION_CLOSE,
	ACTION_PRESS,
	ACTION_PULL,
	ACTION_USE,
	ACTION_TALK,
	ACTION_GIVE
};

enum RoomID {
	INTRO,CORRIDOR,HALL,SLEEP,COCKPIT,AIRLOCK,
	HOLD,LANDINGMODULE,GENERATOR,OUTSIDE,
	CABIN_R1,CABIN_R2,CABIN_R3,CABIN_L1,CABIN_L3,CABIN_L2,BATHROOM,

	ROCKS,CAVE,MEETUP,ENTRANCE,REST,ROGER,GLIDER,MEETUP2,MEETUP3,

	CELL,CORRIDOR1,CORRIDOR2,CORRIDOR3,CORRIDOR4,CORRIDOR5,CORRIDOR6,CORRIDOR7,CORRIDOR8,CORRIDOR9,
	BCORRIDOR,GUARD,GUARD3,OFFICE_L1,OFFICE_L2,OFFICE_R1,OFFICE_R2,OFFICE_L,
	ELEVATOR,STATION,SIGN,NULLROOM
};

enum ObjectID {
	NULLOBJECT,
	KEYCARD,KNIFE,WATCH,
	SOCKET,
	BUTTON,HATCH1,
	BUTTON1,BUTTON2,MANOMETER,SUIT,HELMET,LIFESUPPORT,
	SCRAP_LK,OUTERHATCH_TOP,GENERATOR_TOP,TERMINALSTRIP,LANDINGMOD_OUTERHATCH,
	HOLD_WIRE,
	LANDINGMOD_BUTTON,LANDINGMOD_SOCKET,LANDINGMOD_WIRE,LANDINGMOD_HATCH,LANDINGMOD_MONITOR,
	KEYBOARD,
	KEYCARD2,OUTERHATCH,GENERATOR_WIRE,TRAP,SHORT_WIRE,CLIP,
	VOLTMETER,LADDER,GENERATOR_ROPE,
	KITCHEN_HATCH,SLEEP_SLOT,
	MONITOR,INSTRUMENTS,
	COMPUTER,CABINS,CABIN,
	SLOT_K1,SLOT_K2,SLOT_K3,SLOT_K4,
	SHELF1,SHELF2,SHELF3,SHELF4,
	ROPE,BOOK,DISCMAN,CHESS,
	SLOT_KL1,SLOT_KL2,SLOT_KL3,SLOT_KL4,
	SHELF_L1,SHELF_L2,SHELF_L3,SHELF_L4,
	PISTOL,BOOK2,SPOOL,
	RECORD,TURNTABLE,TURNTABLE_BUTTON,WIRE,WIRE2,PLUG,
	PEN,
	BATHROOM_DOOR,BATHROOM_EXIT,SHOWER,TOILET,

	STONE,
	SPACESHIPS,SPACESHIP,STAR,DOOR,MEETUP_SIGN,
	PORTER,BATHROOM_BUTTON,BATHROOM_SIGN,KITCHEN_SIGN,CAR_SLOT,
	ARS_BATHROOM,COINS,SCHNUCK,EGG,PILL,PILL_HULL,STAIRCASE,
	MEETUP_EXIT,
	ROGER_W,WALLET,KEYCARD_R,CUP,
	GLIDER_BUTTON1,GLIDER_BUTTON2,GLIDER_BUTTON3,GLIDER_BUTTON4,GLIDER_SLOT,GLIDER_BUTTONS,
	GLIDER_DISPLAY,GLIDER_INSTRUMENTS,GLIDER_KEYCARD,
	UFO,

	CELL_BUTTON,CELL_TABLE,CELL_WIRE,TRAY,CELL_DOOR,MAGNET,
	NEWSPAPER,TABLE,
	PILLAR1,PILLAR2,DOOR1,DOOR2,DOOR3,DOOR4,
	GUARDIAN,LAMP,
	MASTERKEYCARD,PAINTING,MONEY,LOCKER,LETTER,
	JUNGLE,SLOT,STATION_SIGN,

	TICKETS
};

static const char *const guiCommands[] = {
    "Gehe",
    "Schau",
    "Nimm",
    "Öffne",
    "Schließe",
    "Drücke",
    "Ziehe",
    "Benutze",
    "Rede",
    "Gib"
};

ObjectType operator|(ObjectType a, ObjectType b);
ObjectType operator&(ObjectType a, ObjectType b);
ObjectType operator^(ObjectType a, ObjectType b);
ObjectType &operator|=(ObjectType &a, ObjectType b);
ObjectType &operator&=(ObjectType &a, ObjectType b);
ObjectType &operator^=(ObjectType &a, ObjectType b);

struct Object {
	static const char *const defaultDescription;

	Object()
	    : _name("")
	    , _description(Object::defaultDescription)
	    , _id(NULLOBJECT)
	    , _type(NULLTYPE)
	    , _click(0)
	    , _click2(0)
	    , _section(0)
	    , _exitRoom(NULLROOM)
	    , _direction(0)
	{}
	Object(const char *name, const char *description, ObjectID id, ObjectType type,
	       byte click, byte click2, byte section = 0, RoomID exitRoom = NULLROOM, byte direction = 0)
	    : _name(name)
	    , _description(description)
	    , _id(id)
	    , _type(type)
	    , _click(click)
	    , _click2(click2)
	    , _section(section)
	    , _exitRoom(exitRoom)
	    , _direction(direction)
	{}

	void setProperty(ObjectType type) {
		_type |= type;
	}

	void disableProperty(ObjectType type) {
		_type ^= type;
	}

	bool hasProperty(ObjectType type) const {
		return _type & type;
	}

	static bool combine(Object &obj1, Object &obj2, ObjectID id1, ObjectID id2) {
		if (obj1.hasProperty(COMBINABLE))
			return (((obj1._id == id1) && (obj2._id == id2)) ||
			        ((obj1._id == id2) && (obj2._id == id1)));
		else
			return false;
	}

	const char *_name;
	const char *_description;
	ObjectID _id;
	ObjectType _type;
	byte _click;
	byte _click2;
	byte _section;
	RoomID _exitRoom;
	byte _direction;
};

struct GameState {
	int32  time;
	int32  timeSleep;
	int32  timeStarting;
	int32  timeAlarm;
	int32  timeAlarmSystem;
	int32  eventTime;
	int32  shipEnergy;
	int32  landingModuleEnergy;
	uint16 greatF;
	int16  timeRobot;
	int16  money;
	char   coins;
	char   shoes;
	char   nameSeen;
	char   destination;
	char   benOverlay;
	bool   language;
	bool   corridorSearch;
	bool   alarmOn;
	bool   terminalStripConnected;
	bool   terminalStripWire;
	bool   cableConnected;
	bool   powerOff;
	bool   cockpitSeen;
	bool   airlockSeen;
	bool   holdSeen;
	bool   dream;
};

}

#endif // MSN_DEF_H
