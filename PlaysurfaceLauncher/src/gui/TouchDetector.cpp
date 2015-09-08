/*******************************************************************************
TouchDetector

PURPOSE: Provides functions to determine if a touch event was on a desktop icon
         for a multiouch app.  This class will be operated by the CentralWidget,
         which has the event() function that listens for QTouchEvent objects.

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
#include "gui/TouchDetector.h"
#include "gui/ConfirmExitBox.h"
#include "gui/ConfirmShutdownBox.h"
#include "gui/AppIcon.h"
#include "gestures/GestureDetector.h"
#include "gestures/SidewaysSwipe.h"
#include "gestures/GestureResult.h"
#include "gestures/LineGesture.h"
#include <QEvent>
#include <QPointF>
#include <QTouchEvent>
#include <QList>
#include <iostream>
#include <iomanip>

using gui::TouchDetector;

const int TouchDetector::DOUBLE_CLICK_MILLISECONDS = 1500;
const double TouchDetector::DOT_PIXELS = 20.0,
             TouchDetector::DEFAULT_SCROLL_FACTOR = 3.0;

TouchDetector::TouchDetector( gui::ConfirmExitBox * confirmExitBox,
                              gui::ConfirmShutdownBox * confirmShutdownBox,
                              std::vector<gui::AppIcon *> & appIcons ) :
  confirmExitBox_( confirmExitBox ),
  confirmShutdownBox_( confirmShutdownBox ),
  tuioGeneratorIcon_( NULL ),
  reloadIcon_( NULL ),
  shutdownIcon_( NULL ),
  appIcons_( appIcons ),
  gestureDetector_( new gestures::GestureDetector() ),
  penColors_(),
  dirty_( false ),
  sidewaysSwipe_( new gestures::SidewaysSwipe() ),
  scrollOffsetX_( 0 ),
  scrollFactor_( 1.0 )
{
    setPenColors();
}

TouchDetector::~TouchDetector()
{
    delete gestureDetector_;
    delete sidewaysSwipe_;
}

void TouchDetector::setPenColors()
{
    penColors_ << QColor( "red" )
               << QColor( "green" )
               << QColor( "cyan" )
               << QColor( "yellow" )
               << QColor( "magenta" )
               << QColor( "orange" );
}

void TouchDetector::setScrollOffsetX( int x )
{
    scrollOffsetX_ = x;
}

void TouchDetector::setTuioGeneratorIcon( gui::AppIcon * tuioGeneratorIcon )
{
    tuioGeneratorIcon_ = tuioGeneratorIcon;
}

void TouchDetector::setReloadIcon( gui::AppIcon * reloadIcon )
{
    reloadIcon_ = reloadIcon;
}

void TouchDetector::setShutdownIcon( gui::AppIcon * shutdownIcon )
{
    shutdownIcon_ = shutdownIcon;
}

bool TouchDetector::touchOnYesButton( const QTouchEvent::TouchPoint & touchPoint )
{
    double x = touchPoint.pos().x(),
           y = touchPoint.pos().y();
    return confirmExitBox_->isPointOnYesButton( x, y );
}

bool TouchDetector::touchOnNoButton( const QTouchEvent::TouchPoint & touchPoint )
{
    double x = touchPoint.pos().x(),
           y = touchPoint.pos().y();
    return confirmExitBox_->isPointOnNoButton( x, y );
}

bool TouchDetector::touchOnShutdownComputerButton( const QTouchEvent::TouchPoint & touchPoint )
{
    double x = touchPoint.pos().x(),
           y = touchPoint.pos().y();
    return confirmShutdownBox_->isPointOnShutdownComputerButton( x, y );
}

bool TouchDetector::touchOnShutdownPlaysurfaceButton( const QTouchEvent::TouchPoint & touchPoint )
{
    double x = touchPoint.pos().x(),
           y = touchPoint.pos().y();
    return confirmShutdownBox_->isPointOnShutdownPlaysurfaceButton( x, y );
}

bool TouchDetector::touchOnCancelShutdownButton( const QTouchEvent::TouchPoint & touchPoint )
{
    double x = touchPoint.pos().x(),
           y = touchPoint.pos().y();
    return confirmShutdownBox_->isPointOnCancelButton( x, y );
}

bool TouchDetector::isTouchOnTuioGeneratorIcon( const QTouchEvent::TouchPoint & touchPoint )
{
    if( tuioGeneratorIcon_ != NULL ) {
        double x = touchPoint.pos().x() - scrollOffsetX_,
               y = touchPoint.pos().y();
        //debugPrint( x, y );

        return tuioGeneratorIcon_->containsPoint( x, y );
    }
    return false;
}

bool TouchDetector::isClickOnTuioGeneratorIcon( double x, double y )
{
    x -= scrollOffsetX_;
    return (tuioGeneratorIcon_ != NULL) 
        && tuioGeneratorIcon_->containsPoint( x, y );
}

bool TouchDetector::isTouchOnReloadIcon( const QTouchEvent::TouchPoint & touchPoint )
{
    if( reloadIcon_ != NULL ) {
        double x = touchPoint.pos().x() - scrollOffsetX_,
               y = touchPoint.pos().y();
        //debugPrint( x, y );

        return reloadIcon_->containsPoint( x, y );
    }
    return false;
}

bool TouchDetector::isClickOnReloadIcon( double x, double y )
{
    x -= scrollOffsetX_;
    return (reloadIcon_ != NULL) && reloadIcon_->containsPoint( x, y );
}

bool TouchDetector::isTouchOnShutdownIcon( const QTouchEvent::TouchPoint & touchPoint )
{
    if( shutdownIcon_ != NULL ) {
        double x = touchPoint.pos().x() - scrollOffsetX_,
               y = touchPoint.pos().y();
        //debugPrint( x, y );

        return shutdownIcon_->containsPoint( x, y );
    }
    return false;
}

bool TouchDetector::isClickOnShutdownIcon( double x, double y )
{
    x -= scrollOffsetX_;
    return (shutdownIcon_ != NULL) && shutdownIcon_->containsPoint( x, y );
}

int TouchDetector::indexOfAppIcon( const QTouchEvent::TouchPoint & touchPoint )
{
    int index = -1;

    double x = touchPoint.pos().x() - scrollOffsetX_,
           y = touchPoint.pos().y();
    //debugPrint( x, y );

    for( int i = 0; i < appIcons_.size(); ++i ) {
        if( appIcons_.at( i )->containsPoint( x, y ) ) {
            index = i;
            //debugPrint( x, y, index );
            break;
        }
    }
    return index;
}

int TouchDetector::indexOfAppIcon( double x, double y )
{
    int index = -1;
    x -= scrollOffsetX_;

    for( int i = 0; i < appIcons_.size(); ++i ) {
        if( appIcons_.at( i )->containsPoint( x, y ) ) {
            index = i;
            //debugPrint( x, y, index );
            break;
        }
    }
    return index;
}

/***************************************************************************//**
If the touchPoint id is not already in the list of gestures, then a new 
gesture will be started.
*******************************************************************************/
void TouchDetector::startGesture( const QTouchEvent::TouchPoint & touchPoint,
                                  QImage * image /*= NULL*/ )
{
    //std::cout << "\n\nTouchDetector::startGesture() called...\n";
    int id = touchPoint.id();
    double x = touchPoint.pos().x() - scrollOffsetX_,
           y = touchPoint.pos().y();

    //debugPrintIdAndXY( id, x, y );
    bool ok = gestureDetector_->startGesture( id, x, y );

    if( ok ) {
        paintDot( image, id, x, y, touchPoint.pressure(), touchPoint.rect() );
    }
}

/*******************************************************************************
Checks for NULL before painting the dot.
*******************************************************************************/
void TouchDetector::paintDot( QImage * image, int id, double x, double y, 
                              double pressure, const QRectF & boundingBox )
{
    if( image != NULL ) {
        double dotDiameter = DOT_PIXELS,
               w = boundingBox.width(),
               h = boundingBox.height();
        //std::cout << std::fixed << std::setprecision( 1 )
        //          << "bounding box (w x h) = (" << w << " x " << h << ")\n"; 
        if( w > 0 && h > 0 ) {
            dotDiameter = (w > h) ? (w / 2.0) : (h / 2.0);
        }
        double halfDotDiameter = dotDiameter / 2.0;

        QPainter painter( image );
        painter.setPen( Qt::NoPen );
        painter.setBrush( penColors_.at( id % penColors_.count() ) );
        painter.drawEllipse( QPointF( x, y ), halfDotDiameter, halfDotDiameter );
        painter.end();
        dirty_ = true;
    }
}

/***************************************************************************//**

*******************************************************************************/
void TouchDetector::updateGesture( const QTouchEvent::TouchPoint & touchPoint, 
                                   QImage * image /*= NULL*/ )
{
    //std::cout << "TouchDetector::updateGesture() called.\n";
    int id = touchPoint.id();
    double x = touchPoint.pos().x() - scrollOffsetX_,
           y = touchPoint.pos().y();

    if( touchPoint.state() != Qt::TouchPointStationary ) {
        gestureDetector_->updateGesture( id, x, y );
        paintDot( image, id, x, y, touchPoint.pressure(), touchPoint.rect() );
    }
}

/***************************************************************************//**
Returns true if the gesture completes a plus sign.
*******************************************************************************/
bool TouchDetector::finishGestureAndTestForPlusSign( const QTouchEvent::TouchPoint & touchPoint, 
                                                     QImage * image /*= NULL*/ )
{
    //std::cout << "TouchDetector::finishGesture() called...\n";
    bool plusSign = false;
    int id = touchPoint.id();
    double x = touchPoint.pos().x() - scrollOffsetX_,
           y = touchPoint.pos().y(),
           pressure = touchPoint.pressure();
    //debugPrintIdAndXY( id, x, y );

    if( image == NULL ) {
        plusSign = gestureDetector_->finishGestureAndTestForPlusSign( id, x, y );
    }
    else { // The image is not null, so need info for painting.
        gestures::GestureResult gestureResult = gestureDetector_->finishGestureAndCalculateResult( id, x, y );
        plusSign = gestureResult.isPlusSign();
        paintDot( image, id, x, y, touchPoint.pressure(), touchPoint.rect() );
        paintResultLabel( image, id, x, y, gestureResult );
    }
    return plusSign;
}

void TouchDetector::paintResultLabel( QImage * image, int id, double x, double y,
                                      gestures::GestureResult & gestureResult )
{
    QPainter painter( image );
    painter.setPen( Qt::black );
    QFont font;
    font.setPointSize( 12 );
    painter.setFont( font );
    double textX = x + 10.0,
           textY1 = y + 6.0,
           textY2 = textY1 + 18,
           textY3 = textY2 + 18,
           textY4 = textY3 + 18;

    if( gestureResult.isHorizontalLine() ) {
        QString aspectRatio = widthHeightRatioAsString( gestureResult );
        painter.drawText( textX, textY1, "Horizontal Line" );
        painter.drawText( textX, textY2, aspectRatio );

        //QString numPoints = numberOfPointsAsString( gestureResult );
        //painter.drawText( textX, textY4, numPoints );
    }
    else if( gestureResult.isVerticalLine() ) {
        QString aspectRatio = heightWidthRatioAsString( gestureResult );
        painter.drawText( textX, textY1, "Vertical Line" );
        painter.drawText( textX, textY2, aspectRatio );

        //QString numPoints = numberOfPointsAsString( gestureResult );
        //painter.drawText( textX, textY4, numPoints );
    }
    else if( gestureResult.numberOfPoints() >= gestures::LineGesture::MIN_NUM_POINTS ) {
        QString aspectRatio = greaterRatioAsString( gestureResult );
        painter.drawText( textX, textY1, "Neither" );
        painter.drawText( textX, textY2, aspectRatio );

        //QString numPoints = numberOfPointsAsString( gestureResult );
        //painter.drawText( textX, textY4, numPoints );
    }
    if( gestureResult.isPlusSign() ) {
        painter.drawText( textX, textY3, "PLUS SIGN DETECTED" );
    }
    painter.setBrush( Qt::black );
    painter.drawEllipse( QPointF( x, y ), 4.0, 4.0 );
    dirty_ = true;
}

QString TouchDetector::widthHeightRatioAsString( gestures::GestureResult & result )
{
    return "Width/Height = " + QString::number( result.widthHeightRatio(), 'f', 1 );
}

QString TouchDetector::heightWidthRatioAsString( gestures::GestureResult & result )
{
    return "Height/Width = " + QString::number( result.heightWidthRatio(), 'f', 1 );
}

QString TouchDetector::greaterRatioAsString( gestures::GestureResult & result )
{
    double widthHeightRatio = result.widthHeightRatio(),
           heightWidthRatio = result.heightWidthRatio();

    if( widthHeightRatio > heightWidthRatio ) {
        return "Width/Height = " + QString::number( widthHeightRatio, 'f', 1 );
    }
    return "Height/Width = " + QString::number( heightWidthRatio, 'f', 1 );
}

QString TouchDetector::numberOfPointsAsString( gestures::GestureResult & result )
{
    return "Points = " + QString::number( result.numberOfPoints() );
}

bool TouchDetector::isPaintingUpdated()
{
    bool update = dirty_;
    dirty_= false;
    return update;
}

void TouchDetector::clearGestures()
{
    gestureDetector_->clear();
}

void TouchDetector::setScrollBarParams( double min, double max, double viewWidth )
{
     int scrollRange = max - min;
     
     if( viewWidth > 0 && scrollRange > 0 ) {
         scrollFactor_ = (scrollRange / viewWidth) * DEFAULT_SCROLL_FACTOR;
     }
     else {
         scrollFactor_ = DEFAULT_SCROLL_FACTOR;
     }
}

void TouchDetector::startSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint )
{
    if( sidewaysSwipe_->isReadyToStart() ) {
        int id = touchPoint.id(),
            x = (int)touchPoint.pos().x();

        sidewaysSwipe_->startSwipe( id, x );
    }
}

int TouchDetector::updateSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint )
{
    int deltaX = 0;

    if( touchPoint.id() == sidewaysSwipe_->blobID() ) {
        int x = (int)touchPoint.pos().x();
        deltaX = (sidewaysSwipe_->lengthOfSwipe( x ) * scrollFactor_);
    }
    return deltaX;
}

void TouchDetector::finishSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint )
{
    if( touchPoint.id() == sidewaysSwipe_->blobID() ) {
        sidewaysSwipe_->clear();
    }
}

void TouchDetector::debugPrint( double x, double y )
{
    std::cout << "(x, y) = (" << x << ", " << y << ")\n";
}

void TouchDetector::debugPrint( double x, double y, int index )
{
    std::cout << "\n(" << x << ", " << y << ") is inside icon " << index << ".\n\n";
}

void TouchDetector::debugPrintIdAndXY( int id, double x, double y )
{
    std::cout << std::fixed << std::setprecision( 0 ) 
              << "    (id, x, y) = (" 
              << id << ", " << x << ", " << y << ")\n";
}
