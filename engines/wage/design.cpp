/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
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
 * MIT License:
 *
 * Copyright (c) 2009 Alexei Svitkine, Eugene Sandulenko
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "common/system.h"
#include "wage/wage.h"
#include "wage/design.h"

namespace Wage {

struct plotData {
	Graphics::Surface *surface;
	Patterns *patterns;
	int fillType;
	int x0;
	int y0;

	plotData(Graphics::Surface *s, Patterns *p, int f, int x, int y) :
		surface(s), patterns(p), fillType(f), x0(x), y0(y) {}
};

Design::Design(Common::SeekableReadStream *data) {
	_len = data->readUint16BE() - 2;
	_data = (byte *)malloc(_len);
	data->read(_data, _len);
}

Design::~Design() {
	free(_data);
}

void Design::paint(Graphics::Surface *canvas, Patterns &patterns, bool mask) {
	Common::MemoryReadStream in(_data, _len);

	if (mask || 1) {
		//canvas.setColor(Color.WHITE);
		canvas->fillRect(Common::Rect(0, 0, _bounds->width(), _bounds->height()), kColorWhite);
		//canvas.setColor(Color.BLACK);
	}

	while (!in.eos()) {
		byte fillType = in.readByte();
		byte borderThickness = in.readByte();
		byte borderFillType = in.readByte();
		warning("fill: %d border: %d borderFill: %d", fillType, borderThickness, borderFillType);
		int type = in.readByte();
		switch (type) {
		case 4:
			drawRect(canvas, in, mask, patterns, fillType, borderThickness, borderFillType);
			break;
			/*
		case 8:
			drawRoundRect(canvas, in, mask, patterns, fillType, borderThickness, borderFillType);
			break;
			*/
		case 12:
			drawOval(canvas, in, mask, patterns, fillType, borderThickness, borderFillType);
			break;
		case 16:
		case 20:
			drawPolygon(canvas, in, mask, patterns, fillType, borderThickness, borderFillType);
			break;
		case 24:
			drawBitmap(canvas, in, mask);
			break;
		default:
			warning("Unknown type => %d", type);
			while (true) {
				((WageEngine *)g_engine)->processEvents();
				g_system->updateScreen();
			}
			return;
		}

		g_system->copyRectToScreen(canvas->getPixels(), canvas->pitch, 0, 0, canvas->w, canvas->h);
		g_system->updateScreen();

	}
}

void drawPixel(int x, int y, int color, void *data) {
	plotData *p = (plotData *)data;

	if (p->fillType > (*p->patterns).size())
		return;

	if (x >= 0 && x < p->surface->w && y >= 0 && y < p->surface->h)
		*((byte *)p->surface->getBasePtr(x, y)) =
			((*p->patterns)[p->fillType - 1][(y - p->y0) % 8] & (1 << (7 - (x - p->x0) % 8))) ?
				color : kColorWhite;
}

void drawPixelPlain(int x, int y, int color, void *data) {
	plotData *p = (plotData *)data;

	if (x >= 0 && x < p->surface->w && y >= 0 && y < p->surface->h)
		*((byte *)p->surface->getBasePtr(x, y)) = (byte)color;
}

void Design::drawRect(Graphics::Surface *surface, Common::ReadStream &in, bool mask,
				Patterns &patterns, byte fillType, byte borderThickness, byte borderFillType) {
	int16 y1 = in.readSint16BE();
	int16 x1 = in.readSint16BE();
	int16 y2 = in.readSint16BE();
	int16 x2 = in.readSint16BE();
	Common::Rect outer(x1, y1, x2, y2);

	plotData pd(surface, &patterns, borderFillType, x1, y1);

	if (mask) {
		drawFilledRect(outer, kColorBlack, drawPixelPlain, &pd);
		return;
	}
	Common::Rect inner(x1 + borderThickness, y1 + borderThickness, x2 - borderThickness, y2 - borderThickness);

	drawFilledRect(outer, kColorBlack, drawPixel, &pd);

	pd.fillType = fillType;

	drawFilledRect(inner, kColorBlack, drawPixel, &pd);
}

void Design::drawPolygon(Graphics::Surface *surface, Common::ReadStream &in, bool mask,
	Patterns &patterns, byte fillType, byte borderThickness, byte borderFillType) {

	warning("ignored => %d", in.readSint16BE());
	int numBytes = in.readSint16BE(); // #bytes used by polygon data, including the numBytes
	int16 by1 = in.readSint16BE();
	int16 bx1 = in.readSint16BE();
	int16 by2 = in.readSint16BE();
	int16 bx2 = in.readSint16BE();
	Common::Rect bbox(bx1, by1, bx2, by2);
	warning("Bbox: %d, %d, %d, %d", bx1, by1, bx2, by2);

	numBytes -= 8;

	int y1 = in.readSint16BE();
	int x1 = in.readSint16BE();

	Common::Array<int> xcoords;
	Common::Array<int> ycoords;

	warning("Start point is (%d,%d)", x1, y1);
	numBytes -= 6;

	while (numBytes > 0) {
		int y2 = y1;
		int x2 = x1;
		int b = in.readSByte();
		//warning("YB = %x", b);
		if (b == (byte)0x80) {
			y2 = in.readSint16BE();
			numBytes -= 3;
		} else {
			//warning("Y");
			y2 += b;
			numBytes -= 1;
		}
		b = in.readSByte();
		//warning("XB = %x", b);
		if (b == (byte) 0x80) {
			x2 = in.readSint16BE();
			numBytes -= 3;
		} else {
			//warning("X");
			x2 += b;
			numBytes -= 1;
		}
		//surface->setColor(colors[c++]);
		//surface->setColor(Color.black);
		xcoords.push_back(x1);
		ycoords.push_back(y1);
		debug(8, "%d %d %d %d", x1, y1, x2, y2);
		//surface->drawLine(x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	}
	xcoords.push_back(x1);
	ycoords.push_back(y1);

	int npoints = xcoords.size();
	int *xpoints = (int *)calloc(npoints, sizeof(int));
	int *ypoints = (int *)calloc(npoints, sizeof(int));
	for (int i = 0; i < npoints; i++) {
		xpoints[i] = xcoords[i];
		ypoints[i] = ycoords[i];
	}
	//	warning(fillType);
/*
	if (mask) {
		surface->fillPolygon(xpoints, ypoints, npoints);
		if (borderThickness > 0) {
			Stroke oldStroke = surface->getStroke();
			surface->setStroke(new BasicStroke(borderThickness - 0.5f, BasicStroke.CAP_SQUARE, BasicStroke.JOIN_BEVEL));
			for (int i = 1; i < npoints; i++)
				surface->drawLine(xpoints[i-1], ypoints[i-1], xpoints[i], ypoints[i]);
			surface->setStroke(oldStroke);
		}
		return;
	}
	if (setPattern(g2d, patterns, fillType - 1)) {
		surface->fillPolygon(xpoints, ypoints, npoints);
	}
	//	warning(borderFillType);
	//surface->setColor(Color.black);
	//if (1==0)
	if (borderThickness > 0 && setPattern(g2d, patterns, borderFillType - 1)) {
		Stroke oldStroke = surface->getStroke();
		//if (borderThickness != 1)
		surface->setStroke(new BasicStroke(borderThickness - 0.5f, BasicStroke.CAP_SQUARE, BasicStroke.JOIN_BEVEL));
*/
	plotData pd(surface, &patterns, fillType, bx1, by1);

	drawPolygonScan(xpoints, ypoints, npoints, bbox, kColorBlack, drawPixel, &pd);
	//	surface->setStroke(oldStroke);
//	}

	pd.fillType = borderFillType;
	if (borderThickness > 0) {
		for (int i = 1; i < npoints; i++)
			drawThickLine(xpoints[i-1], ypoints[i-1], xpoints[i], ypoints[i], borderThickness, kColorBlack, drawPixel, &pd);
	}

	free(xpoints);
	free(ypoints);
}

void Design::drawBitmap(Graphics::Surface *surface, Common::ReadStream &in, bool mask) {
	int numBytes = in.readSint16BE();
	int y1 = in.readSint16BE();
	int x1 = in.readSint16BE();
	int y2 = in.readSint16BE();
	int x2 = in.readSint16BE();
	int w = x2 - x1;
	int h = y2 - y1;

	numBytes -= 10;

	int x = 0, y = 0;
	while (numBytes) {
		int n = in.readSByte();
		int count;
		int b;
		int state;

		numBytes--;

		if ((n >= 0) && (n <= 127)) { // If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
			count = n + 1;
			state = 1;
		} else if ((n >= -127) && (n <= -1)) { // Else if n is between -127 and -1 inclusive, copy the next byte -n+1 times.
			b = in.readByte();
			numBytes--;
			count = -n + 1;
			state = 2;
		} else { // Else if n is -128, noop.
			count = 0;
		}

		for (int i = 0; i < count; i++) {
			byte color;
			if (state == 1) {
				color = in.readByte();
				numBytes--;
			} else if (state == 2)
				color = b;

			for (int c = 0; c < 8; c++) {
				if (x1 + x >= 0 && x1 + x < surface->w && y1 + y >= 0 && y1 + y < surface->h)
					*((byte *)surface->getBasePtr(x1 + x, y1 + y)) =
						(color & (1 << (7 - c % 8))) ?
							kColorBlack : kColorWhite;
				x++;
				if (x == w) {
					y++;

					if (y == h)
						return;

					x = 0;
					break;
				}

			}
		}
	}
}

void Design::drawFilledRect(Common::Rect &rect, int color, void (*plotProc)(int, int, int, void *), void *data) {
	for (int y = rect.top; y < rect.bottom; y++)
		drawHLine(rect.left, rect.right, y, color, plotProc, data);
}


// Based on public-domain code by Darel Rex Finley, 2007
// http://alienryderflex.com/polygon_fill/
void Design::drawPolygonScan(int *polyX, int *polyY, int npoints, Common::Rect &bbox, int color,
								void (*plotProc)(int, int, int, void *), void *data) {
	int *nodeX = (int *)calloc(npoints, sizeof(int));
	int i, j;

	//  Loop through the rows of the image.
	for (int pixelY = bbox.top; pixelY < bbox.bottom; pixelY++) {
		//  Build a list of nodes.
		int nodes = 0;
		j = npoints - 1;

		for (i = 0; i < npoints; i++) {
			if ((polyY[i] < pixelY && polyY[j] >= pixelY) || (polyY[j] < pixelY && polyY[i] >= pixelY)) {
				nodeX[nodes++] = (int)(polyX[i] + (double)(pixelY - polyY[i]) / (double)(polyY[j]-polyY[i]) *
														(double)(polyX[j] - polyX[i]) + 0.5);
			}
			j = i;
		}

		//  Sort the nodes, via a simple “Bubble” sort.
		i = 0;
		while (i < nodes - 1) {
			if (nodeX[i] > nodeX[i + 1]) {
				SWAP(nodeX[i], nodeX[i + 1]);
				if (i)
					i--;
			} else {
				i++;
			}
		}

		//  Fill the pixels between node pairs.
		for (i = 0; i < nodes; i += 2) {
			if (nodeX[i  ] >= bbox.right)
				break;
			if (nodeX[i + 1] > bbox.left) {
				nodeX[i] = MAX<int16>(nodeX[i], bbox.left);
				nodeX[i + 1] = MIN<int16>(nodeX[i + 1], bbox.right);

				drawHLine(nodeX[i], nodeX[i + 1], pixelY, color, plotProc, data);
			}
		}
	}

	free(nodeX);
}

void Design::drawOval(Graphics::Surface *surface, Common::ReadStream &in, bool mask,
	Patterns &patterns, byte fillType, byte borderThickness, byte borderFillType) {
	int16 y1 = in.readSint16BE();
	int16 x1 = in.readSint16BE();
	int16 y2 = in.readSint16BE();
	int16 x2 = in.readSint16BE();

	plotData pd(surface, &patterns, borderFillType, x1, y1);

	if (mask) {
		drawFilledEllipse(x1, y1, x2, y2, drawPixelPlain, &pd);
		return;
	}

	drawFilledEllipse(x1, y1, x2, y2, drawPixel, &pd);

	pd.fillType = fillType;
	drawFilledEllipse(x1+borderThickness, y1+borderThickness, x2-2*borderThickness, y2-2*borderThickness, drawPixel, &pd);
}


void Design::drawFilledEllipse(int x0, int y0, int x1, int y1, void (*plotProc)(int, int, int, void *), void *data) {
	int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
	long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
	long err = dx+dy+b1*a*a, e2; /* error of 1.step */

	if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
	a *= 8*a; b1 = 8*b*b;

	do {
		drawHLine(x0, x1 + 1, y0, kColorBlack, plotProc, data);
		drawHLine(x0, x1 + 1, y1, kColorBlack, plotProc, data);
		e2 = 2*err;
		if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
		if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
	} while (x0 <= x1);

	while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
		drawHLine(x0-1, x0-1, y0, kColorBlack, plotProc, data); /* -> finish tip of ellipse */
		drawHLine(x1+1, x1+1, y0, kColorBlack, plotProc, data);
		y0++;
		drawHLine(x0-1, x0-1, y1, kColorBlack, plotProc, data);
		drawHLine(x1+1, x1+1, y1, kColorBlack, plotProc, data);
		y1--;
	}
}

void Design::drawHLine(int x1, int x2, int y, int color, void (*plotProc)(int, int, int, void *), void *data) {
	if (x1 > x2)
		SWAP(x1, x2);

	for (int x = x1; x < x2; x++)
		(*plotProc)(x, y, color, data);
}

void Design::drawVLine(int x, int y1, int y2, int color, void (*plotProc)(int, int, int, void *), void *data) {
	if (y1 > y2)
		SWAP(y1, y2);

	for (int y = y1; y < y2; x++)
		(*plotProc)(x, y, color, data);
}

/* Bresenham as presented in Foley & Van Dam */
/* Code is based on GD lib http://libgd.github.io/ */
void Design::drawThickLine (int x1, int y1, int x2, int y2, int thick, int color,
								void (*plotProc)(int, int, int, void *), void *data) {
	int incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int wid;
	int w, wstart;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if (dx == 0) {
		drawVLine(x1, y1, y2, color, plotProc, data);
		return;
	} else if (dy == 0) {
		drawHLine(x1, x2, y1, color, plotProc, data);
		return;
	}

	if (dy <= dx) {
		/* More-or-less horizontal. use wid for vertical stroke */
		/* Doug Claar: watch out for NaN in atan2 (2.0.5) */

		/* 2.0.12: Michael Schwartz: divide rather than multiply;
			  TBB: but watch out for /0! */
		double ac = cos(atan2 (dy, dx));
		if (ac != 0) {
			wid = thick / ac;
		} else {
			wid = 1;
		}
		if (wid == 0) {
			wid = 1;
		}
		d = 2 * dy - dx;
		incr1 = 2 * dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}

		/* Set up line thickness */
		wstart = y - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			(*plotProc)(x, y, color, data);

		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y++;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					(*plotProc)(x, w, color, data);
			}
		} else {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y--;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					(*plotProc)(x, w, color, data);
			}
		}
	} else {
		/* More-or-less vertical. use wid for horizontal stroke */
		/* 2.0.12: Michael Schwartz: divide rather than multiply;
		   TBB: but watch out for /0! */
		double as = sin(atan2(dy, dx));
		if (as != 0) {
			wid = thick / as;
		} else {
			wid = 1;
		}
		if (wid == 0)
			wid = 1;

		d = 2 * dx - dy;
		incr1 = 2 * dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}

		/* Set up line thickness */
		wstart = x - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			(*plotProc)(w, y, color, data);

		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x++;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					(*plotProc)(w, y, color, data);
			}
		} else {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x--;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					(*plotProc)(w, y, color, data);
			}
		}
	}
}


} // End of namespace Wage
