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
 */

#ifndef XEEN_UTILITY_H
#define XEEN_UTILITY_H

#include <stdarg.h>
#include "common/scummsys.h"

namespace XEEN
{
    struct CCFileId
    {
        public:        
            CCFileId(uint16 id) : _id(id)
            {
            }
                        
            CCFileId(const char* format, ...) : _id(0)
            {
                char buf[256];
                
                va_list args;
                va_start(args, format);
                vsnprintf(buf, 256, format, args);
                va_end(args);
                
                buf[255] = 0;
                
                loadFromString(buf);
            }
            
            operator uint16() { return _id; }
        
        private:
            void loadFromString(const char* name)
            {
                for(; *name; name ++)
                {
                    _id = (_id & 0x7F) << 9 | (_id & 0xFF80) >> 7;            
                    _id += *name;
                }            
            }
        
            uint16 _id;
    };
}

#endif // XEEN_CCFILE_H
