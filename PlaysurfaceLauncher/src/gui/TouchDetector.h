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
#ifndef GUI_TOUCHDETECTOR_H
#define GUI_TOUCHDETECTOR_H

#include <QList>
#include <QColor>
#include <QRectF>
#include <QPainter>
#include <QBrush>
#include <QTouchEvent>
#include <string>
#include <vector>

class QEvent;
class QImage;
namespace gui { class ConfirmExitBox; }
namespace gui { class ConfirmShutdownBox; }
namespace gui { class AppIcon; }
namespace gui { class CornerRect; }
namespace gestures { class GestureDetector; }
namespace gestures { class SidewaysSwipe; }
namespace gestures { class GestureResult; }

namespace gui
{
    class TouchDetector
    {
    public:
        static const int DOUBLE_CLICK_MILLISECONDS;
        static const double DOT_PIXELS,
                            DEFAULT_SCROLL_FACTOR;

        TouchDetector( gui::ConfirmExitBox * confirmExitBox,
                       gui::ConfirmShutdownBox * confirmShutdownBox,
                       std::vector<gui::AppIcon *> & appIcons );
        virtual ~TouchDetector();

        void setTuioGeneratorIcon( gui::AppIcon * tuioGeneratorIcon );
        void setReloadIcon( gui::AppIcon * reloadIcon );
        void setShutdownIcon( gui::AppIcon * shutdownIcon );
        void setScrollOffsetX( int x );

        bool touchOnYesButton( const QTouchEvent::TouchPoint & touchPoint );
        bool touchOnNoButton( const QTouchEvent::TouchPoint & touchPoint );
        bool touchOnShutdownComputerButton( const QTouchEvent::TouchPoint & touchPoint );
        bool touchOnShutdownPlaysurfaceButton( const QTouchEvent::TouchPoint & touchPoint );
        bool touchOnCancelShutdownButton( const QTouchEvent::TouchPoint & touchPoint );

        bool isTouchOnTuioGeneratorIcon( const QTouchEvent::TouchPoint & touchPoint );
        bool isClickOnTuioGeneratorIcon( double x, double y );
        bool isTouchOnReloadIcon( const QTouchEvent::TouchPoint & touchPoint );
        bool isClickOnReloadIcon( double x, double y );
        bool isTouchOnShutdownIcon( const QTouchEvent::TouchPoint & touchPoint );
        bool isClickOnShutdownIcon( double x, double y );
        int indexOfAppIcon( const QTouchEvent::TouchPoint & touchPoint );
        int indexOfAppIcon( double x, double y );

        void startGesture( const QTouchEvent::TouchPoint & touchPoint, QImage * image = NULL );
        void updateGesture( const QTouchEvent::TouchPoint & touchPoint, QImage * image = NULL );
        bool finishGestureAndTestForPlusSign( const QTouchEvent::TouchPoint & touchPoint, QImage * image = NULL );
        void clearGestures();
        bool isPaintingUpdated();

        void setScrollBarParams( double min, double max, double viewWidth );
        void startSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint );
        int updateSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint );
        void finishSidewaysSwipe( const QTouchEvent::TouchPoint & touchPoint );

    private:
        void setPenColors();
        void paintDot( QImage * image, int id, double x, double y, 
                       double pressure, const QRectF & boundingBox );
        void paintResultLabel( QImage * image, int id, double x, double y,
                               gestures::GestureResult & gestureResult );
        QString widthHeightRatioAsString( gestures::GestureResult & result );
        QString heightWidthRatioAsString( gestures::GestureResult & result );
        QString greaterRatioAsString( gestures::GestureResult & result );
        QString numberOfPointsAsString( gestures::GestureResult & result );

        void debugPrint( double x, double y );
        void debugPrint( double x, double y, int index );
        void debugPrintIdAndXY( int id, double x, double y );

        gui::ConfirmExitBox * confirmExitBox_;
        gui::ConfirmShutdownBox * confirmShutdownBox_;
        gui::AppIcon * tuioGeneratorIcon_,
                     * reloadIcon_,
                     * shutdownIcon_;
        std::vector<gui::AppIcon *> & appIcons_;
        gestures::GestureDetector * gestureDetector_;
        QList<QColor> penColors_;
        bool dirty_;
        gestures::SidewaysSwipe * sidewaysSwipe_;
        int scrollOffsetX_;
        double scrollFactor_;
    };
}

#endif
