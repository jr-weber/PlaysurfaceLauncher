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
#include "gestures/LineGesture.h"
#include <QTimer>
#include <iostream>
#include <iomanip>
#include <cmath>

using gestures::LineGesture;

const int LineGesture::MIN_NUM_POINTS = 10,
          LineGesture::EXPIRATION_MILLISECONDS = 5000;

const double LineGesture::VALID_LINE_ASPECT_RATIO = 10.0,
             LineGesture::TOLERANCE_FACTOR = 0.25;

const double LineGesture::HORIZONTAL_LINE_ANGLE_IN_DEGREES = 0.0,
             LineGesture::VERTICAL_LINE_ANGLE_IN_DEGREES = 90.0;

LineGesture::LineGesture( int id, double x, double y ) :
  id_( id ),
  numPoints_( 1 ),
  minX_( x ),
  maxX_( x ),
  minY_( y ),
  maxY_( y ),
  boxWidth_( 1.0 ),
  boxHeight_( 1.0 ),
  widthHeightAspect_( 1.0 ),
  heightWidthAspect_( 1.0 ),
  centroidX_( 0.0 ),
  centroidY_( 0.0 ),
  xTolerance_( 0.0 ),
  yTolerance_( 0.0 ),
  angleInDegrees_( -1.0 ),
  isValidLine_( false ),
  isExpired_( false ),
  debug_( false )
{
}

LineGesture::~LineGesture()
{
}

int LineGesture::id()
{
    return id_;
}

void LineGesture::update( double x, double y )
{
    ++numPoints_;

    if( x < minX_ ) { minX_ = x; }
    else if( x > maxX_ ) { maxX_ = x; }

    if( y < minY_ ) { minY_ = y; }
    else if( y > maxY_ ) { maxY_ = y; }
}

/***************************************************************************//**
Uses bounding box dimensions to determine if the collection of points is a 
horizontal line, a vertical line, or some other gesture (not a valid line).
*******************************************************************************/
void LineGesture::finish( double x, double y )
{
    update( x, y );

    if( numPoints_ >= MIN_NUM_POINTS ) {
         calculateBoundingBox();

        if( isBoundingBoxForHorizontalLine() ) {
            isValidLine_ = true;
            angleInDegrees_ = HORIZONTAL_LINE_ANGLE_IN_DEGREES;
        }
        else if( isBoundingBoxForVerticalLine() ) {
            isValidLine_ = true;
            angleInDegrees_ = VERTICAL_LINE_ANGLE_IN_DEGREES;
        }
        else {
            isValidLine_ = false;
            angleInDegrees_ = -1.0;
        }

        if( isValidLine_ ) {
            calculateCentroid();
            calculateIntersectionTolerance();
            startExpirationTimer();
        }
    }
}

void LineGesture::calculateBoundingBox()
{
    boxWidth_ = maxX_ - minX_;
    boxHeight_ = maxY_ - minY_;

    // A perfect line would give a width or height of zero, which should
    // not be allowed because it would lead to a divide by zero error.
    if( boxWidth_ < 1.0 ) { boxWidth_ = 1.0; }
    if( boxHeight_ < 1.0 ) { boxHeight_ = 1.0; }

    widthHeightAspect_ = boxWidth_ / boxHeight_;
    heightWidthAspect_ = boxHeight_ / boxWidth_;
    if( debug_ ) { debugPrintBoundingBox(); }
}

bool LineGesture::isBoundingBoxForHorizontalLine()
{
    return (widthHeightAspect_ >= VALID_LINE_ASPECT_RATIO);
}

bool LineGesture::isBoundingBoxForVerticalLine()
{
    return (heightWidthAspect_ >= VALID_LINE_ASPECT_RATIO);
}

void LineGesture::calculateCentroid()
{
    centroidX_ = (minX_ + maxX_) / 2.0;
    centroidY_ = (minY_ + maxY_) / 2.0;
    //if( debug_ ) { debugPrintCentroid(); }
}

void LineGesture::calculateIntersectionTolerance()
{
    xTolerance_ = boxWidth_ * TOLERANCE_FACTOR;
    yTolerance_ = boxHeight_ * TOLERANCE_FACTOR;
    //if( debug_ ) { debugPrintIntersectionTolerance(); }
}

void LineGesture::startExpirationTimer()
{
    QTimer::singleShot( EXPIRATION_MILLISECONDS, this, SLOT( setExpired() ) );
}

void LineGesture::setExpired()
{
    isExpired_ = true;
}

bool LineGesture::isValidLine()
{
    return isValidLine_;
}

bool LineGesture::isExpired()
{
    return isExpired_;
}

gestures::GestureResult LineGesture::calculateResult( LineGesture * other )
{
    return gestures::GestureResult( isPlusSign( other ),
                                    isBoundingBoxForHorizontalLine(),
                                    isBoundingBoxForVerticalLine(),
                                    widthHeightAspect_,
                                    heightWidthAspect_,
                                    numPoints_ );
}

bool LineGesture::isPlusSign( LineGesture * other )
{
    bool plusSign = false;

    if( isPerpendicular( other ) ) {
        double deltaX = std::abs( centroidX_ - other->centroidX_ ),
               deltaY = std::abs( centroidY_ - other->centroidY_ );

        plusSign = isWithinTolerance( deltaX, deltaY )
                   && other->isWithinTolerance( deltaX, deltaY );
    }
    return plusSign;
}

bool LineGesture::isPerpendicular( LineGesture * other ) 
{
    bool perpendicular = false;

    if( other != NULL ) {
        double diffInDegrees = std::abs( angleInDegrees_ - other->angleInDegrees_);
        perpendicular = (diffInDegrees == 90.0);
    }
    return perpendicular;
}

bool LineGesture::isHorizontal()
{
    return (widthHeightAspect_ >= VALID_LINE_ASPECT_RATIO);
}

bool LineGesture::isVertical()
{
    return (heightWidthAspect_ >= VALID_LINE_ASPECT_RATIO);
}

bool LineGesture::isWithinTolerance( double deltaX, double deltaY )
{
    if( isHorizontal() ) {
        return (deltaX <= xTolerance_);
    }
    if( isVertical() ) {
        return (deltaY <= yTolerance_);
    }
    return false;
}

void LineGesture::debugPrintBoundingBox()
{
    std::cout << "\nid = " << id_ << "; numPoints = " << numPoints_ << "\n"
              << std::fixed << std::setprecision( 2 )
              << "(minX, maxX) = (" << minX_ << ", " << maxX_ 
              << "); width =  " << boxWidth_ << "\n"
              << "(minY, maxY) = (" << minY_ << ", " << maxY_ 
              << "); height = " << boxHeight_ << "\n"
              << "widthHeightAspect = " << widthHeightAspect_ << "\n"
              << "heightWidthAspect = " << heightWidthAspect_ << "\n";
}

void LineGesture::debugPrintCentroid()
{
    std::cout << "centroid(x, y) = (" << centroidX_ << ", " << centroidY_ << ")\n";
}

void LineGesture::debugPrintIntersectionTolerance()
{
    std::cout << "tolerance(x, y) = (" << xTolerance_ << ", " << yTolerance_ << ")\n";
}
