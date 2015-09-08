/*******************************************************************************
CentralWidget

PURPOSE: Controls the PaintWidget that renders the PlaysurfaceLauncher desktop
         and provides the event() function that QTouchEvent objects are plugged
         into.  Also controls the TouchDetector object and the AppLauncher 
         object that is used to launch multitouch apps after an icon is touched.

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
#ifndef GUI_CENTRAL_WIDGET_H
#define GUI_CENTRAL_WIDGET_H

#include "enums/TuioSourceEnum.h"
#include <QScrollArea>
#include <QImage>
#include <QColor>
#include <QRect>
#include <QTouchEvent>
#include <vector>

namespace gui { class FullScreenWindow; }
namespace gui { class PaintWidget; }
namespace gui { class ConfirmExitBox; }
namespace gui { class ConfirmShutdownBox; }
namespace gui { class TouchDetector; }
namespace gui { class AppIcon; }
namespace app { class AppLauncher; }
namespace sound { class IconSound; }

namespace gui 
{
    class CentralWidget : public QScrollArea
    {
        Q_OBJECT

    public:
        static const int GRAYOUT_ICON_MILLISEC;
        static const QColor BACKGROUND_COLOR;

        CentralWidget( app::AppLauncher * appLauncher, 
                       gui::FullScreenWindow * parent );
        virtual ~CentralWidget();

        void setTuioGeneratorIcon( enums::TuioSourceEnum::Enum tuioSource );
        sound::IconSound * iconSound();
        void readMultitouchAppsInfo();
        void hideConfirmExitBox();
        void hideConfirmShutdownBox();
        void onDeviceArrival( char driveLetter );
        void onDeviceRemoval( char driveLetter );

    public slots:
        void readMultitouchAppsInfoAndUpdate();
        void onMultitouchAppClosed();
        void stopGrayTuioGeneratorIcon();
        void stopGrayReloadIcon();
        void stopGrayShutdownIcon();
        void stopGrayActivatedIcon();
        void launchMultitouchApp();
        void stopQuietPeriod();
        void usePlusGestureToQuitApps( bool b );
        void showGesturePanel( bool b );
        void clearGesturePanelImage();
        void showShutdownComputerIcon( bool b );
        void openConfirmShutdownBox();

        void onMultitouchAppConfirmExitYes();
        void onMultitouchAppConfirmExitNo();
        void onShutdownComputerRequest();
        void onShutdownPlaysurfaceRequest();
        void onCancelShutdownRequest();

    protected:
        void resizeEvent( QResizeEvent * event );
        void mousePressEvent( QMouseEvent * e );
        bool event( QEvent * event );

    private:
        void setBackgroundColor();
        void setUpPaintWidgetScrolling();
        void createTouchDetector();
        void onTuioGeneratorIconActivated();
        void onReloadIconActivated();
        void onShutdownIconActivated();
        void onMultitouchAppIconActivated( int index );
        void handleTouchEvent( QEvent * e );
        void handleTouchPointPressed( const QTouchEvent::TouchPoint & touchPoint );
        void handleTouchPointUpdate( const QTouchEvent::TouchPoint & touchPoint );
        void handleTouchPointReleased( const QTouchEvent::TouchPoint & touchPoint );
        void killCurrentMultitouchApp();
        void startQuietPeriod( int milliseconds );
        void openConfirmExitBox();
        void debugPrintTouchEvent( QEvent * e );
        void debugPrintResizeEvent( int maxScrollBarValue );

        gui::FullScreenWindow * parent_;
        app::AppLauncher * appLauncher_;
        PaintWidget * paintWidget_;
        gui::ConfirmExitBox * confirmExitBox_;
        gui::ConfirmShutdownBox * confirmShutdownBox_;
        sound::IconSound * iconSound_;
        gui::TouchDetector * touchDetector_;
        bool multitouchAppOpen_,
             usePlusGestureToQuitApps_,
             confirmExitBoxOpen_,
             confirmShutdownBoxOpen_,
             quietPeriod_,
             showGesturePanel_,
             scrollBarVisible_;
    };
}

#endif
