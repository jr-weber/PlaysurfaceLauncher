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
#include "gui/AppIcon.h"

using gui::AppIcon;

const int AppIcon::DEFAULT_WIDTH = 200,
          AppIcon::DEFAULT_HEIGHT = 200;

AppIcon::AppIcon( const QImage & iconImage ) :
  iconImage_( iconImage ),
  rect_( 0, 0, 200, 200 ),
  xMin_( 0 ),
  xMax_( 200 ),
  yMin_( 0 ),
  yMax_( 200 )
{

}

AppIcon::~AppIcon()
{
}

void AppIcon::setImage( const QImage & image )
{
    iconImage_ = image;
}

QImage AppIcon::iconImage()
{
    return iconImage_;
}

void AppIcon::setRect( int x, int y, int width, int height )
{
    rect_.setRect( x, y, width, height );
    xMin_ = x;
    xMax_ = x + width;
    yMin_ = y;
    yMax_ = y + height;
    
}

QRect AppIcon::rect()
{
    return rect_;
}

bool AppIcon::containsPoint( double x, double y )
{
    return (x >= xMin_ && x <= xMax_) 
        && (y >= yMin_ && y <= yMax_);
}
