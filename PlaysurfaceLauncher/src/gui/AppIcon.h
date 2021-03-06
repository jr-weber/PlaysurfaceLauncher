/*******************************************************************************
AppIcon

PURPOSE: Holds information on the desktop location of an app icon.  This info
         is needed by the TouchDetector to determine if a touch event was on
         a particular desktop icon.

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
#ifndef GUI_APPICON_H
#define GUI_APPICON_H

#include <QImage>

namespace gui
{
    class AppIcon
    {
    public:
        static const int DEFAULT_WIDTH,
                         DEFAULT_HEIGHT;

        AppIcon( const QImage & iconImage );
        virtual ~AppIcon();

        void setImage( const QImage & image );
        QImage iconImage();

        void setRect( int x, int y, int width, int height );
        QRect rect();
        bool containsPoint( double x, double y );

    private:
        QImage iconImage_;
        QRect rect_;
        int xMin_,
            xMax_,
            yMin_,
            yMax_;
    };
}

#endif
