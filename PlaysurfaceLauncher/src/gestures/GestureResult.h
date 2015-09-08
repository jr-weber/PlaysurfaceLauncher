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
#ifndef GESTURES_GESTURERESULT_H
#define GESTURES_GESTURERESULT_H

namespace gestures
{
    class GestureResult
    {
    public:
        GestureResult();
        GestureResult( bool plusSign, 
                       bool horizontalLine, 
                       bool verticalLine, 
                       double widthHeightAspect,
                       double heightWidthAspect,
                       int numPoints );
        virtual ~GestureResult();

        bool isPlusSign();
        bool isHorizontalLine();
        bool isVerticalLine();
        bool isHorizontalOrVerticalLine();
        double widthHeightRatio();
        double heightWidthRatio();
        int numberOfPoints();

    private:
        bool isPlusSign_,
             isHorizontalLine_,
             isVerticalLine_;
        double widthHeightAspect_,
               heightWidthAspect_;
        int numPoints_;
    };
}

#endif
