/*******************************************************************************
GestureResult

PURPOSE: Stores key information on a LineGesture object.

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
#include "gestures/GestureResult.h"

using gestures::GestureResult;

GestureResult::GestureResult() :
  isPlusSign_( false ),
  isHorizontalLine_( false ),
  isVerticalLine_( false ),
  widthHeightAspect_( 1.0 ),
  heightWidthAspect_( 1.0 ),
  numPoints_( 0 )
{
}

GestureResult::GestureResult( bool plusSign, 
                              bool horizontalLine, 
                              bool verticalLine, 
                              double widthHeightAspect,
                              double heightWidthAspect,
                              int numPoints ) :
  isPlusSign_( plusSign ),
  isHorizontalLine_( horizontalLine ),
  isVerticalLine_( verticalLine ),
  widthHeightAspect_( widthHeightAspect ),
  heightWidthAspect_( heightWidthAspect ),
  numPoints_( numPoints )
{
}

GestureResult::~GestureResult()
{
}

bool GestureResult::isPlusSign()
{
    return isPlusSign_;
}

bool GestureResult::isHorizontalLine()
{
    return isHorizontalLine_;
}

bool GestureResult::isVerticalLine()
{
    return isVerticalLine_;
}

bool GestureResult::isHorizontalOrVerticalLine()
{
    return isHorizontalLine_ || isVerticalLine_;
}

double GestureResult::widthHeightRatio()
{
    return widthHeightAspect_;
}

double GestureResult::heightWidthRatio()
{
    return heightWidthAspect_;
}

int GestureResult::numberOfPoints()
{
    return numPoints_;
}
