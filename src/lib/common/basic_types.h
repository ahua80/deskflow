/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2002 Chris Schoeneman
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "common.h"

// VC++ has built-in sized types
// moved from common.h (why was it there?)
#if defined(_MSC_VER)
#include <wchar.h>
#define TYPE_OF_SIZE_1 __int8
#define TYPE_OF_SIZE_2 __int16
#define TYPE_OF_SIZE_4 __int32
#endif

//
// pick types of particular sizes
//

#if !defined(TYPE_OF_SIZE_1)
#if SIZEOF_CHAR == 1
#define TYPE_OF_SIZE_1 char
#endif
#endif

#if !defined(TYPE_OF_SIZE_2)
#if SIZEOF_INT == 2
#define TYPE_OF_SIZE_2 int
#else
#define TYPE_OF_SIZE_2 short
#endif
#endif

#if !defined(TYPE_OF_SIZE_4)
// Carbon defines SInt32 and UInt32 in terms of long
#if SIZEOF_INT == 4 && !defined(__APPLE__)
#define TYPE_OF_SIZE_4 int
#else
#define TYPE_OF_SIZE_4 long
#endif
#endif

//
// verify existence of required types
//

#if !defined(TYPE_OF_SIZE_1)
#error No 1 byte integer type
#endif
#if !defined(TYPE_OF_SIZE_2)
#error No 2 byte integer type
#endif
#if !defined(TYPE_OF_SIZE_4)
#error No 4 byte integer type
#endif

//
// make typedefs
//
// except for SInt8 and UInt8 these types are only guaranteed to be
// at least as big as indicated (in bits).  that is, they may be
// larger than indicated.
//

// Added this because it doesn't compile on OS X 10.6 because they are already
// defined in Carbon
#if !defined(__MACTYPES__)
#if defined(__APPLE__)
#include <CoreServices/CoreServices.h>
#else
using SInt8 = signed TYPE_OF_SIZE_1;
using SInt16 = signed TYPE_OF_SIZE_2;
using SInt32 = signed TYPE_OF_SIZE_4;
using UInt8 = unsigned TYPE_OF_SIZE_1;
using UInt16 = unsigned TYPE_OF_SIZE_2;
using UInt32 = unsigned TYPE_OF_SIZE_4;
#endif
#endif
//
// clean up
//

#undef TYPE_OF_SIZE_1
#undef TYPE_OF_SIZE_2
#undef TYPE_OF_SIZE_4
