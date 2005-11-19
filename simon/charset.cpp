/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001-2005 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $Header$
 *
 */

#include "common/stdafx.h"
#include "simon/simon.h"
#include "simon/intern.h"

namespace Simon {

void SimonEngine::print_char_helper_1(const byte *src, uint len) {
	uint ind;

	if (_textWindow == NULL)
		return;

	while (len-- != 0) {
		if (*src != 12 && _textWindow->fcs_data != NULL &&
				_fcsData1[ind = get_fcs_ptr_3_index(_textWindow)] != 2) {

			_fcsData1[ind] = 2;
			_fcsData2[ind] = 1;
		}

		fcs_putchar(*src++);
	}
}

void SimonEngine::print_char_helper_5(FillOrCopyStruct *fcs) {
	uint index = get_fcs_ptr_3_index(fcs);
	print_char_helper_6(index);
	_fcsData1[index] = 0;
}

void SimonEngine::print_char_helper_6(uint i) {
	FillOrCopyStruct *fcs;

	if (_fcsData2[i]) {
		lock();
		fcs = _windowArray[i];
		drawIconArray(i, fcs->fcs_data->item_ptr, fcs->fcs_data->unk1, fcs->fcs_data->unk2);
		_fcsData2[i] = 0;
		unlock();
	}
}

void SimonEngine::render_string_amiga(uint vga_sprite_id, uint color, uint width, uint height, const char *txt) {
	VgaPointersEntry *vpe = &_vgaBufferPointers[2];
	byte *src, *dst, *dst_org, chr;
	uint count;

	if (vga_sprite_id >= 100) {
		vga_sprite_id -= 100;
		vpe++;
	}

	src = dst = vpe->vgaFile2;

	count = 499;
	if (vga_sprite_id == 1)
		count *= 2;

	src += vga_sprite_id * 8;
	dst += READ_BE_UINT32(src);
	WRITE_BE_UINT16(dst + 4, height);
	WRITE_BE_UINT16(dst + 6, width);

	uint charsize = width/8 * height;
	memset(dst, 0, count);
	dst_org = dst;
	int delta = 0;
	while ((chr = *txt++) != 0) {
		int tmp = chr;
		if (chr == 10) {
			dst_org += width * 10;
			dst = dst_org;
			delta = 0;
		} else if ((tmp -= '!') < 0) {
			delta += 6;
			if (delta > 8)
			{
				delta -= 8;
				dst_org++;
			}
		} else {
			byte *img = src + chr * 41;
			int CTR = img[40];
			int D3 = 8 - delta;
			for (int D2 = 9; D2 != 0; D2--)
			{
				byte *cur_dst = dst_org;
				for (int D7 = 2; D7 != 0; D7--)
				{
					chr = *img >> delta;
					if (chr)
					{
						if (color & 1) *(cur_dst + charsize * 0) |= chr;
						if (color & 2) *(cur_dst + charsize * 1) |= chr;
						if (color & 4) *(cur_dst + charsize * 2) |= chr;
						if (color & 8) *(cur_dst + charsize * 3) |= chr;
					}
					if ((D3 >= CTR) && (chr = *img++ << (D3)))
					{
						if (color & 1) *(cur_dst + charsize * 0) |= chr;
						if (color & 2) *(cur_dst + charsize * 1) |= chr;
						if (color & 4) *(cur_dst + charsize * 2) |= chr;
						if (color & 8) *(cur_dst + charsize * 3) |= chr;
					}
					color++;
				}
				chr = *img >> delta;
				if (chr)
				{
					*(cur_dst + charsize * 0) |= chr;
					*(cur_dst + charsize * 1) |= chr;
					*(cur_dst + charsize * 2) |= chr;
					*(cur_dst + charsize * 3) |= chr;
				}
				if ((D3 >= CTR) && (chr = *img++ << (D3)))
				{
					*(cur_dst + charsize * 0) |= chr;
					*(cur_dst + charsize * 1) |= chr;
					*(cur_dst + charsize * 2) |= chr;
					*(cur_dst + charsize * 3) |= chr;
				}
				cur_dst += width/8;
			}
			delta += CTR;
			if (delta > 8)
			{
				delta -= 8;
				dst_org++;
			}
		}
	}
}

void SimonEngine::render_string(uint vga_sprite_id, uint color, uint width, uint height, const char *txt) {
	VgaPointersEntry *vpe = &_vgaBufferPointers[2];
	byte *src, *dst, *p, *dst_org, chr;
	uint count;

	if (vga_sprite_id >= 100) {
		vga_sprite_id -= 100;
		vpe++;
	}

	src = dst = vpe->vgaFile2;

	count = 4000;
	if (vga_sprite_id == 1)
		count *= 2;

	p = dst + vga_sprite_id * 8;

	WRITE_BE_UINT16(p + 4, height);
	WRITE_BE_UINT16(p + 6, width);

	dst += READ_BE_UINT32(p);

	memset(dst, 0, count);
	if (_language == Common::HB_ISR)
		dst += width - 1; // For Hebrew, start at the right edge, not the left.

	dst_org = dst;
	while ((chr = *txt++) != 0) {
		if (chr == 10) {
			dst_org += width * 10;
			dst = dst_org;
		} else if ((chr -= ' ') == 0) {
			dst += (_language == Common::HB_ISR ? -6 : 6); // Hebrew moves to the left, all others to the right
		} else {
			byte *img_hdr = src + 48 + chr * 4;
			uint img_height = img_hdr[2];
			uint img_width = img_hdr[3], i;
			byte *img = src + READ_LE_UINT16(img_hdr);
			if (_language == Common::HB_ISR)
				dst -= img_width - 1; // For Hebrew, move from right edge to left edge of image.
			byte *cur_dst = dst;

			assert(img_width > 0 && img_width < 50 && img_height > 0 && img_height < 50);

			do {
				for (i = 0; i != img_width; i++) {
					chr = *img++;
					if (chr) {
						if (chr == 0xF)
							chr = 207;
						else
							chr += color;
						cur_dst[i] = chr;
					}
				}
				cur_dst += width;
			} while (--img_height);

			if (_language != Common::HB_ISR) // Hebrew character movement is done higher up
				dst += img_width - 1;
		}
	}
}

void SimonEngine::showMessageFormat(const char *s, ...) {
	char buf[STRINGBUFLEN];
	char *str;
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	if (!_fcsData1[_curWindow]) {
		showmessage_helper_2();
		if (!_showMessageFlag) {
			_windowArray[0] = _textWindow;
			showmessage_helper_3(_textWindow->textLength,
                                 _textWindow->textMaxLength);
		}
		_showMessageFlag = true;
		_fcsData1[_curWindow] = 1;
	}

	for (str = buf; *str; str++)
		showmessage_print_char(*str);
}

void SimonEngine::showmessage_print_char(byte chr) {
	if (chr == 12) {
		_numLettersToPrint = 0;
		_printCharCurPos = 0;
		print_char_helper_1(&chr, 1);
		print_char_helper_5(_textWindow);
	} else if (chr == 0 || chr == ' ' || chr == 10) {
		if (_printCharMaxPos - _printCharCurPos >= _numLettersToPrint) {
			_printCharCurPos += _numLettersToPrint;
			print_char_helper_1(_lettersToPrintBuf, _numLettersToPrint);

			if (_printCharCurPos == _printCharMaxPos) {
				_printCharCurPos = 0;
			} else {
				if (chr)
					print_char_helper_1(&chr, 1);
				if (chr == 10)
					_printCharCurPos = 0;
				else if (chr != 0)
					_printCharCurPos++;
			}
		} else {
			const byte newline_character = 10;
			_printCharCurPos = _numLettersToPrint;
			print_char_helper_1(&newline_character, 1);
			print_char_helper_1(_lettersToPrintBuf, _numLettersToPrint);
			if (chr == ' ') {
				print_char_helper_1(&chr, 1);
				_printCharCurPos++;
			} else {
				print_char_helper_1(&chr, 1);
				_printCharCurPos = 0;
			}
		}
		_numLettersToPrint = 0;
	} else {
		_lettersToPrintBuf[_numLettersToPrint++] = chr;
	}
}

void SimonEngine::showmessage_helper_2() {
	if (_textWindow)
		return;

	_textWindow = openWindow(8, 0x90, 0x18, 6, 1, 0, 0xF);
}

void SimonEngine::showmessage_helper_3(uint a, uint b) {
	_printCharCurPos = a;
	_printCharMaxPos = b;
	_numLettersToPrint = 0;
}

void SimonEngine::video_putchar(FillOrCopyStruct *fcs, byte c, byte b) {
	byte width = 6;

	if (c == 0xC) {
		video_fill_or_copy_from_3_to_2(fcs);
	} else if (c == 0xD || c == 0xA) {
		video_putchar_newline(fcs);
	} else if ((c == 1 && _language != Common::HB_ISR) || (c == 8)) {
		if (_language == Common::HB_ISR) { //Hebrew
			if (b >= 64 && b < 91)
				width = _hebrew_char_widths [b - 64];

			if (fcs->textLength != 0) {
				fcs->textLength--;
				fcs->textColumnOffset += width;
				if (fcs->textColumnOffset >= 8) {
					fcs->textColumnOffset -= 8;
					fcs->textColumn--;
				}
			}
		} else {
			int8 val = (c == 8) ? 6 : 4;

			if (fcs->textLength != 0) {
				fcs->textLength--;
				fcs->textColumnOffset -= val;
				if ((int8)fcs->textColumnOffset < val) {
					fcs->textColumnOffset += 8;
					fcs->textColumn--;
				}
			}
		}
	} else if (c >= 0x20) {
		if (fcs->textLength == fcs->textMaxLength) {
			video_putchar_newline(fcs);
		} else if (fcs->textRow == fcs->height) {
			video_putchar_newline(fcs);
			fcs->textRow--;
		}

		if (_language == Common::HB_ISR) { //Hebrew
			if (c >= 64 && c < 91)
				width = _hebrew_char_widths [c - 64];
			fcs->textColumnOffset  -= width;
			if (fcs->textColumnOffset >= width) {
				fcs->textColumn++;
				fcs->textColumnOffset += 8;
			}
			video_putchar_drawchar(fcs, fcs->width + fcs->x - fcs->textColumn, fcs->textRow * 8 + fcs->y, c);
			fcs->textLength++;
		} else {
			video_putchar_drawchar(fcs, fcs->textColumn + fcs->x, fcs->textRow * 8 + fcs->y, c);

			fcs->textLength++;
			fcs->textColumnOffset += 6;
			if (c == 'i' || c == 'l')
				fcs->textColumnOffset -= 2;

			if (fcs->textColumnOffset >= 8) {
				fcs->textColumnOffset -= 8;
				fcs->textColumn++;
			}
		}
	}
}

void SimonEngine::video_putchar_newline(FillOrCopyStruct *fcs) {
	fcs->textColumnOffset = 0;
	fcs->textLength = 0;
	fcs->textColumn = 0;

	if (fcs->textRow != fcs->height)
		fcs->textRow++;
}

#ifdef PALMOS_68K
static const byte *russian_video_font;
static const byte *polish_video_font;
static const byte *french_video_font;
static const byte *german_video_font;
static const byte *hebrew_video_font;
static const byte *italian_video_font;
static const byte *spanish_video_font;
static const byte *video_font;
#else
static const byte russian_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 0, 100, 40, 48, 40, 100, 0,
	0, 0, 96, 48, 40, 40, 112, 0,
	60, 68, 68, 60, 36, 68, 68, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	72, 84, 84, 116, 84, 84, 72, 0,
	0, 0, 60, 68, 60, 36, 100, 0,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	16, 32, 0, 120, 112, 64, 56, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	32, 16, 0, 112, 8, 248, 120, 0,
	0, 0, 96, 48, 40, 40, 112, 0,
	0, 0, 112, 88, 20, 20, 56, 0,
	0, 0, 120, 4, 28, 4, 120, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	0, 0, 84, 84, 84, 84, 56, 4,
	56, 68, 68, 124, 68, 68, 68, 0,
	124, 36, 32, 56, 36, 36, 120, 0,
	120, 36, 36, 56, 36, 36, 120, 0,
	124, 36, 32, 32, 32, 32, 112, 0,
	56, 40, 40, 40, 40, 40, 124, 68,
	124, 36, 32, 56, 32, 36, 124, 0,
	84, 84, 84, 56, 84, 84, 84, 0,
	56, 68, 4, 24, 4, 68, 56, 0,
	68, 68, 76, 84, 100, 68, 68, 0,
	100, 40, 40, 48, 40, 36, 100, 0,
	28, 36, 36, 36, 36, 36, 100, 0,
	68, 108, 84, 68, 68, 68, 68, 0,
	68, 68, 68, 124, 68, 68, 68, 0,
	56, 68, 68, 68, 68, 68, 56, 0,
	124, 68, 68, 68, 68, 68, 68, 0,
	120, 36, 36, 56, 32, 32, 112, 0,
	56, 68, 64, 64, 64, 68, 56, 0,
	124, 84, 16, 16, 16, 16, 56, 0,
	100, 36, 36, 28, 4, 4, 56, 0,
	56, 84, 84, 84, 56, 16, 56, 0,
	108, 40, 16, 16, 40, 40, 108, 0,
	72, 72, 72, 72, 72, 72, 60, 4,
	76, 72, 72, 56, 8, 8, 28, 0,
	84, 84, 84, 84, 84, 84, 60, 0,
	84, 84, 84, 84, 84, 84, 56, 4,
	56, 68, 4, 28, 4, 68, 56, 0,
	0, 0, 68, 100, 84, 84, 100, 0,
	0, 0, 72, 84, 116, 84, 72, 0,
	0, 0, 60, 68, 60, 36, 100, 0,
	0, 0, 120, 4, 24, 4, 120, 0,
	0, 0, 100, 40, 48, 40, 100, 0,
	60, 68, 68, 60, 36, 68, 68, 0,
	0, 0, 56, 4, 60, 68, 60, 0,
	60, 64, 32, 56, 68, 68, 56, 0,
	48, 72, 80, 120, 68, 68, 56, 0,
	0, 0, 120, 4, 56, 64, 60, 0,
	56, 4, 4, 60, 68, 68, 56, 0,
	0, 0, 56, 68, 120, 64, 56, 0,
	40, 0, 56, 68, 120, 64, 56, 0,
	0, 0, 84, 84, 56, 84, 84, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	0, 0, 68, 68, 68, 68, 60, 0,
	56, 0, 68, 68, 68, 68, 60, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 28, 36, 36, 36, 100, 0,
	0, 0, 68, 108, 84, 68, 68, 0,
	0, 0, 56, 68, 68, 68, 56, 0,
	0, 0, 68, 68, 124, 68, 68, 0,
	0, 0, 124, 68, 68, 68, 68, 0,
	0, 0, 120, 36, 36, 56, 32, 112,
	0, 0, 60, 64, 64, 64, 60, 0,
	0, 0, 124, 84, 16, 16, 56, 0,
	0, 0, 68, 68, 60, 4, 56, 0,
	48, 16, 56, 84, 84, 56, 16, 56,
	0, 0, 68, 40, 16, 40, 68, 0,
	0, 0, 72, 72, 72, 72, 60, 4,
	0, 0, 76, 72, 72, 56, 8, 28,
	0, 0, 84, 84, 84, 84, 60, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte polish_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 0, 112, 136, 248, 128, 112, 8,
	0, 16, 120, 128, 112, 8, 240, 0,
	192, 64, 64, 96, 192, 64, 224, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	0, 32, 112, 136, 136, 136, 112, 0,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	0, 32, 112, 136, 128, 136, 112, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	0, 32, 240, 136, 136, 136, 136, 0,
	80, 0, 136, 136, 136, 136, 112, 0,
	0, 32, 248, 144, 32, 72, 248, 0,
	8, 32, 248, 144, 32, 72, 248, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	0, 0, 112, 8, 120, 136, 120, 4,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	224, 64, 64, 96, 192, 72, 248, 0,
	16, 120, 128, 112, 8, 136, 112, 0,
	248, 72, 64, 112, 64, 72, 248, 16,
	32, 248, 16, 32, 64, 136, 248, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte french_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	32, 80, 0, 112, 136, 136, 112, 0,
	32, 80, 0, 112, 8, 248, 120, 0,
	112, 136, 128, 128, 136, 112, 32, 96,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	112, 136, 128, 128, 136, 112, 32, 96,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	32, 64, 0, 112, 248, 128, 112, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	32, 16, 0, 112, 8, 248, 120, 0,
	32, 80, 0, 144, 144, 144, 104, 0,
	32, 16, 0, 112, 248, 128, 112, 0,
	32, 80, 0, 112, 248, 128, 112, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	32, 80, 0, 192, 64, 64, 224, 0,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	0, 0, 112, 136, 128, 112, 32, 96,
	160, 0, 192, 64, 64, 64, 224, 0,
	32, 16, 0, 144, 144, 144, 104, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte german_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	80, 0, 112, 8, 120, 136, 120, 0,
	80, 0, 112, 136, 136, 136, 112, 0,
	80, 0, 144, 144, 144, 144, 104, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	96, 144, 144, 160, 144, 144, 160, 128,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	80, 0, 112, 136, 248, 136, 136, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	136, 112, 136, 136, 136, 136, 112, 0,
	80, 0, 136, 136, 136, 136, 112, 0,
	80, 0, 144, 144, 144, 144, 104, 0,
	32, 64, 0, 112, 248, 128, 112, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	32, 80, 0, 192, 64, 64, 224, 0,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	0, 48, 72, 64, 72, 48, 16, 48,
	0, 80, 0, 96, 32, 40, 48, 0,
	32, 16, 0, 152, 144, 144, 232, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte hebrew_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 144, 0, 96, 144, 144, 104, 0,
	0, 144, 0, 96, 144, 144, 96, 0,
	0, 144, 0, 144, 144, 144, 96, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	0, 112, 136, 240, 136, 136, 240, 0,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	16, 32, 0, 120, 112, 64, 56, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	5, 5, 4, 6, 5, 3, 4, 5,
	6, 3, 5, 5, 4, 6, 5, 3,
	4, 6, 5, 6, 6, 6, 5, 5,
	5, 6, 5, 6, 6, 6, 6, 6,
	112, 136, 8, 16, 32, 0, 32, 0,
	0, 0, 144, 80, 160, 144, 144, 0,
	0, 0, 224, 32, 32, 32, 240, 0,
	0, 0, 224, 32, 96, 160, 160, 0,
	0, 0, 248, 16, 16, 16, 16, 0,
	0, 0, 240, 16, 16, 144, 144, 0,
	0, 0, 192, 64, 64, 64, 64, 0,
	0, 0, 224, 64, 32, 64, 64, 0,
	0, 0, 240, 144, 144, 144, 144, 0,
	0, 0, 184, 168, 136, 136, 112, 0,
	0, 0, 192, 64, 0, 0, 0, 0,
	0, 0, 240, 16, 16, 16, 16, 16,
	0, 0, 224, 16, 16, 16, 224, 0,
	128, 128, 224, 32, 32, 32, 192, 0,
	0, 0, 248, 72, 72, 72, 120, 0,
	0, 0, 176, 208, 144, 144, 176, 0,
	0, 0, 192, 64, 64, 64, 64, 64,
	0, 0, 96, 32, 32, 32, 224, 0,
	0, 0, 248, 72, 72, 72, 48, 0,
	0, 0, 80, 80, 80, 80, 224, 0,
	0, 0, 248, 72, 104, 8, 8, 8,
	0, 0, 248, 72, 104, 8, 248, 0,
	0, 0, 216, 72, 48, 16, 16, 16,
	0, 0, 144, 80, 32, 16, 240, 0,
	0, 0, 240, 16, 144, 160, 128, 128,
	0, 0, 240, 16, 16, 16, 16, 0,
	0, 0, 168, 168, 200, 136, 112, 0,
	0, 0, 240, 80, 80, 80, 208, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	0, 48, 72, 64, 72, 48, 16, 48,
	0, 80, 0, 96, 32, 40, 48, 0,
	32, 16, 0, 152, 144, 144, 232, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte italian_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	80, 0, 112, 8, 120, 136, 120, 0,
	80, 0, 112, 136, 136, 136, 112, 0,
	32, 16, 0, 112, 136, 136, 112, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	32, 16, 0, 192, 64, 64, 224, 0,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	32, 64, 0, 112, 248, 128, 112, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	32, 16, 0, 112, 8, 248, 120, 0,
	32, 16, 0, 112, 136, 136, 112, 0,
	32, 16, 0, 112, 248, 128, 112, 0,
	32, 80, 0, 112, 248, 128, 112, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	32, 80, 0, 96, 32, 40, 48, 0,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	0, 0, 112, 136, 128, 112, 32, 96,
	160, 0, 192, 64, 64, 64, 224, 0,
	32, 16, 0, 144, 144, 144, 104, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte spanish_video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	80, 0, 112, 8, 120, 136, 120, 0,
	80, 0, 112, 136, 136, 136, 112, 0,
	80, 0, 144, 144, 144, 144, 104, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	96, 144, 144, 160, 144, 144, 160, 128,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	32, 64, 0, 112, 248, 128, 112, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	32, 64, 0, 112, 8, 248, 120, 0,
	32, 64, 0, 192, 64, 64, 224, 0,
	32, 64, 0, 112, 136, 136, 112, 0,
	32, 64, 0, 144, 144, 144, 104, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	80, 160, 0, 240, 136, 136, 136, 0,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	32, 0, 32, 64, 128, 136, 112, 0,
	32, 0, 32, 32, 112, 112, 32, 0,
	80, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};

static const byte video_font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	32, 112, 112, 32, 32, 0, 32, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 144, 0, 96, 144, 144, 104, 0,
	0, 144, 0, 96, 144, 144, 96, 0,
	0, 144, 0, 144, 144, 144, 96, 0,
	0, 16, 40, 16, 42, 68, 58, 0,
	48, 48, 96, 0, 0, 0, 0, 0,
	0, 4, 8, 8, 8, 8, 4, 0,
	0, 32, 16, 16, 16, 16, 32, 0,
	0, 0, 20, 8, 62, 8, 20, 0,
	0, 112, 136, 240, 136, 136, 240, 0,
	0, 0, 0, 0, 0, 48, 48, 96,
	0, 0, 0, 240, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 48, 48, 0,
	16, 32, 0, 120, 112, 64, 56, 0,
	112, 136, 152, 168, 200, 136, 112, 0,
	32, 96, 32, 32, 32, 32, 112, 0,
	112, 136, 8, 48, 64, 136, 248, 0,
	112, 136, 8, 48, 8, 136, 112, 0,
	16, 48, 80, 144, 248, 16, 56, 0,
	248, 128, 240, 8, 8, 136, 112, 0,
	48, 64, 128, 240, 136, 136, 112, 0,
	248, 136, 8, 16, 32, 32, 32, 0,
	112, 136, 136, 112, 136, 136, 112, 0,
	112, 136, 136, 120, 8, 16, 96, 0,
	0, 0, 48, 48, 0, 48, 48, 0,
	32, 16, 0, 112, 8, 248, 120, 0,
	32, 80, 0, 144, 144, 144, 104, 0,
	80, 0, 144, 144, 144, 144, 104, 0,
	32, 80, 0, 112, 248, 128, 112, 0,
	112, 136, 8, 16, 32, 0, 32, 0,
	32, 80, 0, 192, 64, 64, 224, 0,
	112, 136, 136, 248, 136, 136, 136, 0,
	240, 72, 72, 112, 72, 72, 240, 0,
	48, 72, 128, 128, 128, 72, 48, 0,
	224, 80, 72, 72, 72, 80, 224, 0,
	248, 72, 64, 112, 64, 72, 248, 0,
	248, 72, 64, 112, 64, 64, 224, 0,
	48, 72, 128, 152, 136, 72, 56, 0,
	136, 136, 136, 248, 136, 136, 136, 0,
	248, 32, 32, 32, 32, 32, 248, 0,
	24, 8, 8, 8, 136, 136, 112, 0,
	200, 72, 80, 96, 80, 72, 200, 0,
	224, 64, 64, 64, 64, 72, 248, 0,
	136, 216, 168, 168, 136, 136, 136, 0,
	136, 200, 168, 152, 136, 136, 136, 0,
	112, 136, 136, 136, 136, 136, 112, 0,
	240, 72, 72, 112, 64, 64, 224, 0,
	112, 136, 136, 136, 136, 168, 112, 8,
	240, 72, 72, 112, 72, 72, 200, 0,
	112, 136, 128, 112, 8, 136, 112, 0,
	248, 168, 32, 32, 32, 32, 112, 0,
	136, 136, 136, 136, 136, 136, 120, 0,
	136, 136, 136, 80, 80, 32, 32, 0,
	136, 136, 136, 136, 168, 216, 136, 0,
	136, 136, 80, 32, 80, 136, 136, 0,
	136, 136, 136, 112, 32, 32, 112, 0,
	248, 136, 16, 32, 64, 136, 248, 0,
	0, 14, 8, 8, 8, 8, 14, 0,
	0, 128, 64, 32, 16, 8, 4, 0,
	0, 112, 16, 16, 16, 16, 112, 0,
	0, 48, 72, 64, 72, 48, 16, 48,
	0, 80, 0, 96, 32, 40, 48, 0,
	32, 16, 0, 152, 144, 144, 232, 0,
	0, 0, 112, 8, 120, 136, 120, 0,
	192, 64, 80, 104, 72, 72, 112, 0,
	0, 0, 112, 136, 128, 136, 112, 0,
	24, 16, 80, 176, 144, 144, 112, 0,
	0, 0, 112, 136, 248, 128, 112, 0,
	48, 72, 64, 224, 64, 64, 224, 0,
	0, 0, 104, 144, 144, 112, 136, 112,
	192, 64, 80, 104, 72, 72, 200, 0,
	64, 0, 192, 64, 64, 64, 224, 0,
	8, 0, 8, 8, 8, 8, 136, 112,
	192, 64, 72, 80, 96, 80, 200, 0,
	192, 64, 64, 64, 64, 64, 224, 0,
	0, 0, 144, 216, 168, 136, 136, 0,
	0, 0, 240, 136, 136, 136, 136, 0,
	0, 0, 112, 136, 136, 136, 112, 0,
	0, 0, 176, 72, 72, 112, 64, 224,
	0, 0, 104, 144, 144, 112, 16, 56,
	0, 0, 176, 72, 72, 64, 224, 0,
	0, 0, 120, 128, 112, 8, 240, 0,
	64, 64, 240, 64, 64, 72, 48, 0,
	0, 0, 144, 144, 144, 144, 104, 0,
	0, 0, 136, 136, 136, 80, 32, 0,
	0, 0, 136, 136, 168, 216, 144, 0,
	0, 0, 136, 80, 32, 80, 136, 0,
	0, 0, 136, 136, 136, 112, 32, 192,
	0, 0, 248, 144, 32, 72, 248, 0,
	32, 80, 0, 96, 144, 144, 96, 0,
	0, 14, 8, 48, 8, 8, 14, 0,
	0, 8, 8, 8, 8, 8, 8, 0,
	0, 112, 16, 12, 16, 16, 112, 0,
	0, 0, 0, 0, 0, 0, 248, 0,
	252, 252, 252, 252, 252, 252, 252, 252,
	240, 240, 240, 240, 240, 240, 240, 240,
};
#endif

void SimonEngine::video_putchar_drawchar(FillOrCopyStruct *fcs, uint x, uint y, byte chr) {
	const byte *src;
	byte color, *dst;
	uint h, i;

	_lockWord |= 0x8000;

	dst = dx_lock_2();
	dst += y * _dxSurfacePitch + x * 8 + fcs->textColumnOffset;

	switch(_language) {
	case Common::RU_RUS:
		src = russian_video_font + (chr - 0x20) * 8;
		break;
	case Common::PL_POL:
		src = polish_video_font + (chr - 0x20) * 8;
		break;
	case Common::HB_ISR:
		src = hebrew_video_font + (chr - 0x20) * 8;
		break;
	case Common::ES_ESP:
		src = spanish_video_font + (chr - 0x20) * 8;
		break;
	case Common::IT_ITA:
		src = italian_video_font + (chr - 0x20) * 8;
		break;
	case Common::FR_FRA:
		src = french_video_font + (chr - 0x20) * 8;
		break;
	case Common::DE_DEU:
		src = german_video_font + (chr - 0x20) * 8;
		break;
	case Common::EN_USA:
		src = video_font + (chr - 0x20) * 8;
		break;
	default:
		error("video_putchar_drawchar: Unknown language %d\n", _language);
	}

	color = fcs->text_color;

	h = 8;
	do {
		int8 b = *src++;
		i = 0;
		do {
			if (b < 0)
				dst[i] = color;
			b <<= 1;
		} while (++i != 6);
		dst += _dxSurfacePitch;
	} while (--h);

	dx_unlock_2();

	_lockWord &= ~0x8000;
}

} // End of namespace Simon

#ifdef PALMOS_68K
#include "scumm_globals.h"

_GINIT(Simon_Charset)
_GSETPTR(Simon::russian_video_font, GBVARS_RUSSIANVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::polish_video_font, GBVARS_POLISHVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::french_video_font, GBVARS_FRENCHVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::german_video_font, GBVARS_GERMANVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::hebrew_video_font, GBVARS_HEBREWVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::italian_video_font, GBVARS_ITALIANVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::spanish_video_font, GBVARS_SPANISHVIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GSETPTR(Simon::video_font, GBVARS_VIDEOFONT_INDEX, byte, GBVARS_SIMON)
_GEND

_GRELEASE(Simon_Charset)
_GRELEASEPTR(GBVARS_RUSSIANVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_POLISHVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_FRENCHVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_GERMANVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_HEBREWVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_ITALIANVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_SPANISHVIDEOFONT_INDEX, GBVARS_SIMON)
_GRELEASEPTR(GBVARS_VIDEOFONT_INDEX, GBVARS_SIMON)
_GEND

#endif
