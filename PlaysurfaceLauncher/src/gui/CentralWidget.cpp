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
#include "gui/CentralWidget.h"
#include "gui/FullScreenWindow.h"
#include "gui/PaintWidget.h"
#include "gui/ConfirmExitBox.h"
#include "gui/ConfirmShutdownBox.h"
#include "gui/TouchDetector.h"
#include "gui/AppIcon.h"
#include "sound/IconSound.h"
#include "app/AppLauncher.h"
#include <QApplication>
#include <QtGui>
#include <QScrollBar>
#include <QPalette>
#include <QTimer>
#include <iostream>
#include <iomanip>

using gui::CentralWidget;

const int CentralWidget::GRAYOUT_ICON_MILLISEC = 300;
const QColor CentralWidget::BACKGROUND_COLOR( 134, 201, 222 );

CentralWidget::CentralWidget( app::AppLauncher * appLauncher, 
                              gui::FullScreenWindow * parent ) : 
  QScrollArea( parent ),
  parent_( parent ),
  appLauncher_( appLauncher ),
  paintWidget_( new PaintWidget( parent, appLauncher->tuioGenerator() ) ),
  confirmExitBox_( new gui::ConfirmExitBox() ),
  confirmShutdownBox_( new gui::ConfirmShutdownBox( this ) ),
  iconSound_( new sound::IconSound() ),
  touchDetector_( NULL ),
  multitouchAppOpen_( false ),
  usePlusGestureToQuitApps_( false ),
  confirmExitBoxOpen_( false ),
  confirmShutdownBoxOpen_( false ),
  quietPeriod_( false ),
  showGesturePanel_( false ),
  scrollBarVisible_( false )
{
    setUpPaintWidgetScrolling();
    createTouchDetector();
    
    // The PlaysurfaceLauncher is only intended to work with TUIO touch events 
    // (or mouse clicks on an icon), but not Windows 8 touch, which has some
    // differences.  If Windows 8 touch events from a bezel or other device 
    // are being converted to TUIO using a program like TouchHooks2Tuio, it 
    // is not a good idea to also listen to the Windows 8 touch events directly.
    // Therefore, do NOT set the WA_AcceptTouchEvents flag to true.
    ////setAttribute( Qt::WA_AcceptTouchEvents ); // Windows 8 touch, not TUIO.
    setAttribute( Qt::WA_StaticContents ); 
    setBackgroundColor();

    connect( appLauncher_, SIGNAL( multitouchAppClosed( ) ),
             this, SLOT( onMultitouchAppClosed() ) );
}

CentralWidget::~CentralWidget()
{
    delete touchDetector_;
    delete iconSound_;
    delete confirmExitBox_;
    delete confirmShutdownBox_;
    delete paintWidget_;
}

void CentralWidget::setTuioGeneratorIcon( enums::TuioSourceEnum::Enum tuioSource )
{
    paintWidget_->setTuioGeneratorIcon( tuioSource );
    paintWidget_->update();
}

void CentralWidget::setUpPaintWidgetScrolling()
{
    setWidget( paintWidget_ );
    setWidgetResizable( false );
    setFrameStyle( QFrame::NoFrame );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
}

void CentralWidget::createTouchDetector()
{
    touchDetector_ = new gui::TouchDetector( confirmExitBox_, 
                                             confirmShutdownBox_,
                                             paintWidget_->appIcons() );
    touchDetector_->setTuioGeneratorIcon( paintWidget_->tuioGeneratorAppIcon() );
    touchDetector_->setReloadIcon( paintWidget_->reloadAppIcon() );
    touchDetector_->setShutdownIcon( paintWidget_->shutdownAppIcon() );
}

void CentralWidget::setBackgroundColor()
{
    QPalette pal = palette();
    pal.setColor( QPalette::Window, BACKGROUND_COLOR );
    setAutoFillBackground( true );
    setPalette( pal );
}

sound::IconSound * CentralWidget::iconSound()
{
    return iconSound_;
}

void CentralWidget::readMultitouchAppsInfo()
{
    appLauncher_->readMultitouchAppsInfo();
    paintWidget_->setFileIcons( appLauncher_->multitouchAppIcons() );
}

void CentralWidget::hideConfirmExitBox()
{
    confirmExitBox_->hide();
    confirmExitBoxOpen_ = false;
}

void CentralWidget::hideConfirmShutdownBox()
{
    confirmShutdownBox_->hide();
    confirmShutdownBoxOpen_ = false;
}

void CentralWidget::onDeviceArrival( char driveLetter )
{
    if( appLauncher_->hasMultitouchAppsDirectory( driveLetter ) ) {
        readMultitouchAppsInfo();
        paintWidget_->update();
    }
}

void CentralWidget::onDeviceRemoval( char driveLetter )
{
    if( appLauncher_->isMultitouchAppsDirInUse( driveLetter ) ) {
        readMultitouchAppsInfo();
        paintWidget_->update();
    }
}

void CentralWidget::onMultitouchAppClosed()
{
    paintWidget_->drawBlackScreen( false );
    multitouchAppOpen_ = false;
    paintWidget_->update();
}

void CentralWidget::stopGrayTuioGeneratorIcon()
{
    paintWidget_->setTuioGeneratorIconToGray( false );
    paintWidget_->update();
}

void CentralWidget::stopGrayReloadIcon()
{  
    paintWidget_->setReloadIconToGray( false );
    paintWidget_->update();
}

void CentralWidget::stopGrayShutdownIcon()
{  
    paintWidget_->setShutdownIconToGray( false );
    paintWidget_->update();
}

void CentralWidget::stopGrayActivatedIcon()
{
    paintWidget_->setActivatedAppIconToGray( false );
    paintWidget_->update();
}

void CentralWidget::launchMultitouchApp()
{
    paintWidget_->drawBlackScreen( true );
    multitouchAppOpen_ = true;
    quietPeriod_ = false;
    paintWidget_->update();
    appLauncher_->launchMultitouchApp( paintWidget_->activatedAppIconIndex() );
    paintWidget_->setActivatedAppIconIndex( -1 );
}

void CentralWidget::stopQuietPeriod()
{
    quietPeriod_ = false;
}

void CentralWidget::usePlusGestureToQuitApps( bool b )
{
    usePlusGestureToQuitApps_ = b;
    //std::cout << "CentralWidget::usePlusGestureToQuitApps(" << usePlusGestureToQuitApps_ << ")\n";
}

void CentralWidget::showGesturePanel( bool b )
{
    showGesturePanel_ = b;
    paintWidget_->drawGesturePanel( showGesturePanel_ );
    paintWidget_->update();
}

void CentralWidget::clearGesturePanelImage()
{
    touchDetector_->clearGestures();
    paintWidget_->clearGesturePanelImage();
}

void CentralWidget::showShutdownComputerIcon( bool b )
{
    //std::cout << "CentralWidget::showShutdownComputerIcon(" << b << ")\n";
    paintWidget_->showShutdownComputerIcon( b );
    paintWidget_->update();
}

void CentralWidget::resizeEvent( QResizeEvent * event )
{
    //std::cout << "CentralWidget w x h = " << width() << " x " << height() << "\n";
    int centralWidgetWidth = width(),
        centralWidgetHeight = height();
    paintWidget_->setWidthAndHeightOfParent( centralWidgetWidth, centralWidgetHeight );
    
    // If the PaintWidget is wider than the viewport, the scrollbar should be visible.
    int differenceInWidths = paintWidget_->width() - viewport()->width();
    scrollBarVisible_ = (differenceInWidths > 0);

    int maxScrollBarValue = (differenceInWidths > 0) ? differenceInWidths : 0;
    horizontalScrollBar()->setRange( 0, maxScrollBarValue );
    touchDetector_->setScrollBarParams( 0, maxScrollBarValue, centralWidgetWidth );
    //debugPrintResizeEvent( maxScrollBarValue );
    QWidget::resizeEvent( event );
}

void CentralWidget::mousePressEvent( QMouseEvent * e )
{
    if( !confirmShutdownBoxOpen_ && !multitouchAppOpen_ && !showGesturePanel_ && !quietPeriod_ ) {
        touchDetector_->setScrollOffsetX( paintWidget_->x() );
        int x = e->x(),
            y = e->y();

        if( touchDetector_->isClickOnTuioGeneratorIcon( x, y ) ) {
            onTuioGeneratorIconActivated();
        }
        else if( touchDetector_->isClickOnReloadIcon( x, y ) ) {
            onReloadIconActivated();
        }
        else if( touchDetector_->isClickOnShutdownIcon( x, y ) ) {
            startQuietPeriod( 1000 );
            onShutdownIconActivated();
        }
        else {
            int iconIndex = touchDetector_->indexOfAppIcon( x, y );
        
            if( iconIndex >= 0 ) {
                onMultitouchAppIconActivated( iconIndex );
            }
            else {
                //std::cout << "CentralWidget(x, y) = " << x << ", " << y << ")\n";

                if( y < 20 ) {
                    parent_->showMenuBar();
                }
                else {
                    parent_->hideMenuBar();
                }
            }
        }
    }
}

void CentralWidget::onTuioGeneratorIconActivated()
{
    paintWidget_->setTuioGeneratorIconToGray( true );
    paintWidget_->update();
    iconSound_->play();
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC, this, SLOT( stopGrayTuioGeneratorIcon() ) );
    appLauncher_->onTuioGeneratorIconTouched();
}

void CentralWidget::onReloadIconActivated()
{
    paintWidget_->setReloadIconToGray( true );
    paintWidget_->update();
    iconSound_->play();
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC, this, SLOT( stopGrayReloadIcon() ) );
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC + 200, this, SLOT( readMultitouchAppsInfoAndUpdate() ) );
}

void CentralWidget::onShutdownIconActivated()
{
    paintWidget_->setShutdownIconToGray( true );
    paintWidget_->update();
    iconSound_->play();
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC, this, SLOT( stopGrayShutdownIcon() ) );
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC + 200, this, SLOT( openConfirmShutdownBox() ) );
}

void CentralWidget::readMultitouchAppsInfoAndUpdate()
{
    readMultitouchAppsInfo();
    paintWidget_->update();
    appLauncher_->placeAllOtherWindowsInTaskbar();
}

void CentralWidget::onMultitouchAppIconActivated( int index )
{
    quietPeriod_ = true;
    paintWidget_->setActivatedAppIconToGray( true );
    paintWidget_->setActivatedAppIconIndex( index );
    paintWidget_->update();
    iconSound_->play();
    QTimer::singleShot( GRAYOUT_ICON_MILLISEC, this, SLOT( stopGrayActivatedIcon() ) );
    QTimer::singleShot( 2 * GRAYOUT_ICON_MILLISEC, this, SLOT( launchMultitouchApp() ) );
}

/***************************************************************************//**
This function expects the QEvent object to be a QTouchEvent that holds an 
array of TouchPoint objects, where each TouchPoint object corresponds to a 
blob (fingertip on a multitouch table).  The names of the QEvent type 
(TouchBegin, TouchUpdate, and TouchEnd) are actually quite misleading because
they appear to refer only to the last TouchPoint added to the array.  For any 
individual TouchPoint object in the array, the state() function must be called 
to figure out if the TouchPoint is a new blob (TouchPointPressed), a blob to 
be updated (TouchPointMoved or TouchPointStationary), or a blob that just 
ended (TouchPointReleased).  Therefore, the QEvent::Type for QEvent objects
sent by the QTuio class is only useful for testing the the QEvent actually is 
a QTouchEvent object before casting it to QTouchEvent.
*******************************************************************************/
bool CentralWidget::event( QEvent * e )
{
    //std::cout << "CentralWidget::event() called.\n";
    bool eventHandled = true;

    switch( e->type() ) {
        case QEvent::TouchBegin:   //std::cout << "\nTouchBegin\n";   
                                   handleTouchEvent( e );  
                                   break;
        case QEvent::TouchUpdate:  //std::cout << "\nTouchUpdate\n";  
                                   handleTouchEvent( e );  
                                   break;
        case QEvent::TouchEnd:     //std::cout << "\nTouchEnd\n";     
                                   handleTouchEvent( e );  
                                   break;
        default:                   eventHandled = QWidget::event( e );  
                                   break;
    }
    return eventHandled;
}

/***************************************************************************//**
Before calling this function, the QEvent object given as an arg should be tested
to make sure that it is really a QTouchEvent object.  That is necessary because 
this function does a static cast from QEvent to QTouchEvent.
*******************************************************************************/
void CentralWidget::handleTouchEvent( QEvent * e )
{
    e->accept();
    QTouchEvent * touchEvent = static_cast<QTouchEvent *>(e);
    QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
    //debugPrintTouchEvent( e );
    touchDetector_->setScrollOffsetX( paintWidget_->x() );

    foreach( const QTouchEvent::TouchPoint & touchPoint, touchPoints ) {
        switch( touchPoint.state() ) {
            case Qt::TouchPointPressed:    handleTouchPointPressed( touchPoint );  break;
            case Qt::TouchPointStationary: handleTouchPointUpdate( touchPoint );   break;
            case Qt::TouchPointMoved:      handleTouchPointUpdate( touchPoint );   break;
            case Qt::TouchPointReleased:   handleTouchPointReleased( touchPoint ); break;
            default:  break;
        }   
    }
}

void CentralWidget::handleTouchPointPressed( const QTouchEvent::TouchPoint & touchPoint )
{
    if( !quietPeriod_ ) {
        if( confirmExitBoxOpen_ ) {
            if( touchDetector_->touchOnYesButton( touchPoint ) ) {
                onMultitouchAppConfirmExitYes();
            }
            else if( touchDetector_->touchOnNoButton( touchPoint ) ) {
                onMultitouchAppConfirmExitNo();
            }
        }
        else if( confirmShutdownBoxOpen_ ) {
            if( touchDetector_->touchOnShutdownComputerButton( touchPoint ) ) {
                onShutdownComputerRequest();
            }
            else if( touchDetector_->touchOnShutdownPlaysurfaceButton( touchPoint ) ) {
                onShutdownPlaysurfaceRequest();
            }
            else if( touchDetector_->touchOnCancelShutdownButton( touchPoint ) ) {
                onCancelShutdownRequest();
            }
        }
        else if( showGesturePanel_ ) {
            QImage * image = paintWidget_->gesturePanelImage();
            touchDetector_->startGesture( touchPoint, image );
            
            if( touchDetector_->isPaintingUpdated() ) {
                paintWidget_->update();
            }
        }
        else if( multitouchAppOpen_ ) {
            touchDetector_->startGesture( touchPoint );
        }
        else {
            if( touchDetector_->isTouchOnTuioGeneratorIcon( touchPoint ) ) {
                startQuietPeriod( 1000 );
                onTuioGeneratorIconActivated();
            }
            else if( touchDetector_->isTouchOnReloadIcon( touchPoint ) ) {
                startQuietPeriod( 1000 );
                onReloadIconActivated();
            }
            else if( touchDetector_->isTouchOnShutdownIcon( touchPoint ) ) {
                startQuietPeriod( 1000 );
                onShutdownIconActivated();
            }
            else { // Playsurface icons are showing.
                int iconIndex = touchDetector_->indexOfAppIcon( touchPoint );
            
                if( iconIndex >= 0 ) { // Launch an app.
                    onMultitouchAppIconActivated( iconIndex );
                }
                else if( scrollBarVisible_ ) {
                    // The PaintWidget is wider than CentralWidget, so start 
                    // a sideways swipe (if one is not already in progress).
                    touchDetector_->startSidewaysSwipe( touchPoint );
                }
            }
        }
    }
}

void CentralWidget::onMultitouchAppConfirmExitYes()
{
    iconSound_->play();
    killCurrentMultitouchApp();
    hideConfirmExitBox();
    startQuietPeriod( 1000 );
}

void CentralWidget::onMultitouchAppConfirmExitNo()
{
    iconSound_->play();
    hideConfirmExitBox();
    appLauncher_->focusOnMultitouchApp();
}

void CentralWidget::onShutdownComputerRequest()
{
    iconSound_->play();
    hideConfirmShutdownBox();
    parent_->shutdownComputer();
}

void CentralWidget::onShutdownPlaysurfaceRequest()
{
    iconSound_->play();
    hideConfirmShutdownBox();
    parent_->exitWithoutShuttingDownProjector();
}

void CentralWidget::onCancelShutdownRequest()
{
    iconSound_->play();
    hideConfirmShutdownBox();
    startQuietPeriod( 500 );
}

void CentralWidget::handleTouchPointUpdate( const QTouchEvent::TouchPoint & touchPoint )
{
    //std::cout << "\nCentralWidget::handleTouchPointUpdate() called...\n";

    if( showGesturePanel_ ) {
        QImage * image = paintWidget_->gesturePanelImage();
        touchDetector_->updateGesture( touchPoint, image );
        
        if( touchDetector_->isPaintingUpdated() ) {
            paintWidget_->update();
        }
    }
    else if( multitouchAppOpen_ ) {
        touchDetector_->updateGesture( touchPoint );
    }
    else if( scrollBarVisible_ ) {
        int deltaX = touchDetector_->updateSidewaysSwipe( touchPoint );

        if( deltaX != 0 ) {
            QScrollBar * scrollBar = horizontalScrollBar();
            int x = scrollBar->sliderPosition() + deltaX;

            if( x > scrollBar->minimum() && x < scrollBar->maximum() ) {
                scrollBar->setSliderPosition( x );
            }
        }
    }
}

void CentralWidget::handleTouchPointReleased( const QTouchEvent::TouchPoint & touchPoint )
{
    //std::cout << "\nCentralWidget::handleTouchEnd() called...\n";

    if( showGesturePanel_ ) {
        QImage * image = paintWidget_->gesturePanelImage();
        touchDetector_->finishGestureAndTestForPlusSign( touchPoint, image );

        if( touchDetector_->isPaintingUpdated() ) {
            paintWidget_->update();
        }
    }
    else if( multitouchAppOpen_ && !confirmExitBoxOpen_   ) {
        bool isPlusSign = touchDetector_->finishGestureAndTestForPlusSign( touchPoint );

        if( isPlusSign ) {
            if( usePlusGestureToQuitApps_ ) {
                openConfirmExitBox();
            }
            touchDetector_->clearGestures();
        }
    }
    else if( scrollBarVisible_ ) {
        touchDetector_->finishSidewaysSwipe( touchPoint );
    }
}

void CentralWidget::killCurrentMultitouchApp()
{
    appLauncher_->killMultitouchAppProcess();
    paintWidget_->drawBlackScreen( false );
    multitouchAppOpen_ = false;
    paintWidget_->update();
}

void CentralWidget::startQuietPeriod( int milliseconds )
{
    quietPeriod_ = true;
    QTimer::singleShot( milliseconds, this, SLOT( stopQuietPeriod() ) );
}

void CentralWidget::openConfirmExitBox()
{
    paintWidget_->hideLoadingIconIfVisible();
    update();
    confirmExitBoxOpen_ = true;
    appLauncher_->hideMultitouchApp();
    confirmExitBox_->show();
    QApplication::setActiveWindow( confirmExitBox_ );
}

void CentralWidget::openConfirmShutdownBox()
{
    paintWidget_->hideLoadingIconIfVisible();
    update();
    confirmShutdownBoxOpen_ = true;
    confirmShutdownBox_->show();
    QApplication::setActiveWindow( confirmShutdownBox_ );
}

void CentralWidget::debugPrintResizeEvent( int maxScrollBarValue )
{
    std::cout << "\nCentralWidget::resizeEvent() called...\n"
              << "    central widget    = "  << width() << " x " << height() << "\n"
              << "    viewport          = " << viewport()->width() << " x " << viewport()->height() << "\n"
              << "    range             = 0 to " << maxScrollBarValue << "\n"
              << "    scrollBarVisible_ = " << scrollBarVisible_ << "\n";
}

/***************************************************************************//**
The QEvent should have already been verified to be aQTouchEvent before calling 
on this function.  Otherwise, the static cast to QTouchEvent will not work.
*******************************************************************************/
void CentralWidget::debugPrintTouchEvent( QEvent * e )
{
    QTouchEvent * touchEvent = static_cast<QTouchEvent *>(e);
    QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

    foreach( const QTouchEvent::TouchPoint & touchPoint, touchPoints ) {
        int id = touchPoint.id();
        double x = touchPoint.pos().x(),
               y = touchPoint.pos().y();

        QString state = "unknown";

        switch( touchPoint.state() ) {
            case Qt::TouchPointPressed:    state = "TouchPointPressed";    break;
            case Qt::TouchPointMoved:      state = "TouchPointMoved";      break;
            case Qt::TouchPointStationary: state = "TouchPointStationary"; break;
            case Qt::TouchPointReleased:   state = "TouchPointReleased";   break;
            default:                       state = "Unknown";              break;
        }   

        std::cout << std::fixed << std::setprecision( 0 ) 
                  << "    (id, x, y) = (" 
                  << id << ", " << x << ", " << y << ")"
                  << " and state = " << state.toStdString() << "\n";
    }
}
