/*******************************************************************************
TuioSourceEnum

PURPOSE: Enumeration for the 3 touch detection modes that can be used by the
         PlaysurfaceLauncher: 
         
             BigBlobby (camera to TUIO)
             TouchHooks2Tuio (Windows 8 touch events converted to TUIO)
             Windows 8 Touch

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
*******************************************************************************/
/*
 PlaysurfaceLauncher - Provides a game console-like environment for launching
                       TUIO-based multitouch apps.  
 
 Copyright (c) 2014, 2015 J.R.Weber <joe.weber77@gmail.com>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with 
 this program.  If not, go to http://www.gnu.org/licenses/gpl-3.0.en.html or
 write to 
 
 Free Software  Foundation, Inc.
 59 Temple Place, Suite 330
 Boston, MA  02111-1307  USA
*/
#ifndef ENUMS_TUIOSOURCEENUM_H
#define ENUMS_TUIOSOURCEENUM_H

#include <QString>

namespace enums 
{
    class TuioSourceEnum
    {
    public:
        enum Enum {
            BIG_BLOBBY,
            TOUCH_HOOKS_2_TUIO,
            WINDOWS_8_TOUCH
        };

        static QString convertToString( enums::TuioSourceEnum::Enum type );
    };
}

#endif
