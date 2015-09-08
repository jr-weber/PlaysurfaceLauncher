/*******************************************************************************
LineGesture

PURPOSE: Uses a bounding box ratio to determine if a set of points is a line, 
         and, if so, is it a horizontal or vertical line.  LineGestures can be
         used to test if a plus-sign gesture for closing the current 
         multitouch app has been made.

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
#ifndef GESTURES_LINEGESTURE_H
#define GESTURES_LINEGESTURE_H

#include "gestures/GestureResult.h"
#include <QObject>

namespace gestures { class GestureResult; }

namespace gestures
{
    class LineGesture : public QObject
    {
        Q_OBJECT

    public:
        static const int MIN_NUM_POINTS,
                         EXPIRATION_MILLISECONDS;
        static const double VALID_LINE_ASPECT_RATIO,
                            TOLERANCE_FACTOR;
        static const double HORIZONTAL_LINE_ANGLE_IN_DEGREES,
                            VERTICAL_LINE_ANGLE_IN_DEGREES;

        LineGesture( int id, double x, double y );
        virtual ~LineGesture();

        int id();
        void update( double x, double y );
        void finish( double x, double y );
        bool isValidLine();
        bool isExpired();
        gestures::GestureResult calculateResult( LineGesture * other );
        bool isPlusSign( LineGesture * other );
        bool isHorizontal();
        bool isVertical();

    public slots:
        void setExpired();

    private:
        void calculateBoundingBox();
        bool isBoundingBoxForHorizontalLine();
        bool isBoundingBoxForVerticalLine();
        void calculateCentroid();
        void calculateIntersectionTolerance();
        void startExpirationTimer();
        bool isPerpendicular( LineGesture * other );
        bool isWithinTolerance( double deltaX, double deltaY );

        void debugPrintBoundingBox();
        void debugPrintCentroid();
        void debugPrintIntersectionTolerance();

        int id_,
            numPoints_;
        double minX_,
               maxX_,
               minY_,
               maxY_,
               boxWidth_,
               boxHeight_,
               widthHeightAspect_,
               heightWidthAspect_,
               centroidX_,
               centroidY_,
               xTolerance_,
               yTolerance_,
               angleInDegrees_;
        bool isValidLine_,
             isExpired_,
             debug_;
    };
}

#endif
