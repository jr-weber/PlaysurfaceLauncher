/*******************************************************************************
PaintWidget

PURPOSE: Provides functions for rendering multitouch app icons on the 
         PlaysurfaceLauncher desktop.  Also provides a fingerpainting panel 
         for testing if a gesture is being drawn correctly (such as the plus 
         gesture that can be used to close multitouch apps).

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
#include "gui/PaintWidget.h"
#include "gui/AppIcon.h"
#include "gui/FullScreenWindow.h"
#include <QPainter>
#include <QRect>
#include <QMovie>
#include <QLabel>
#include <QTimer>
#include <iostream>

using gui::PaintWidget;

const QString PaintWidget::GESTURE_PANEL_MESSAGE_1 = 
    "When a multitouch application is open, it can be closed with a \"+\" sign gesture.";
const QString PaintWidget::GESTURE_PANEL_MESSAGE_2 = 
    "You can finger paint on this panel to check that you are drawing the plus sign correctly.";
const QString PaintWidget::GESTURE_PANEL_MESSAGE_3 =
    "Each finger painted line will be labeled as Horizontal, Vertical, or Neither.";
const QString PaintWidget::GESTURE_PANEL_MESSAGE_4 =
    "The second line of a Plus Sign must be drawn within 5 seconds of the first.";

const QString PaintWidget::BACKGROUND_IMAGE_FILE =
                  ":/QResources/logos/Playsurface-Hand-Logo-1920x1080.png";
const QString PaintWidget::BIG_BLOBBY_IMAGE_FILE = 
                  ":/QResources/icons/BigBlobby-playsurface-icon.png";
const QString PaintWidget::TOUCH_HOOKS_2_TUIO_IMAGE_FILE = 
                  ":/QResources/icons/TouchHooks2Tuio-playsurface-icon.png";
const QString PaintWidget::WINDOWS_8_TOUCH_IMAGE_FILE = 
                  ":/QResources/icons/Windows-8-playsurface-icon.png";
const QString PaintWidget::RELOAD_ICON_IMAGE_FILE = 
                  ":/QResources/icons/Reload-Circle-playsurface-icon.png";
const QString PaintWidget::SHUTDOWN_ICON_IMAGE_FILE = 
                  ":/QResources/icons/Shutdown-playsurface-icon.png";

const int PaintWidget::LOADING_ICON_TIMEOUT = 5000,
          PaintWidget::X_START = 125,
          PaintWidget::Y_START = 125,
          PaintWidget::X_SPACING = 25,
          PaintWidget::Y_SPACING = 25,
          PaintWidget::IMAGE_WIDTH = 100,
          PaintWidget::IMAGE_HEIGHT = 100;
const double PaintWidget::GESTURE_BOUNDARY_FACTOR = 0.10;
const QColor PaintWidget::BACKGROUND_COLOR( 0, 255, 0 );
const QPen PaintWidget::THIN_BORDER_PEN( Qt::black );
const QBrush PaintWidget::SOLID_BLACK_BRUSH( QColor( 0, 0, 0 ) ),
             PaintWidget::SOLID_WHITE_BRUSH( QColor( 255, 255, 255 ) ),
             PaintWidget::TRANSLUCENT_LIGHT_GRAY_BRUSH( QColor( 200, 200, 200, 100 ) ),
             PaintWidget::TRANSLUCENT_GRAY_BRUSH( QColor( 127, 127, 127, 200 ) ),
             PaintWidget::TRANSLUCENT_DARK_GRAY_BRUSH( QColor( 0, 0, 0, 200 ) );
const int PaintWidget::LOADING_ICON_WIDTH = 400,
          PaintWidget::LOADING_ICON_HEIGHT = 400;

PaintWidget::PaintWidget( gui::FullScreenWindow * mainWindow, enums::TuioSourceEnum::Enum tuioSource ) :
  mainWindow_( mainWindow ),
  tuioSource_( tuioSource ),
  parentWidth_( 1280 ),
  parentHeight_( 800 ),
  backgroundImage_(),
  gesturePanelImage_(),
  gesturePanelImageRect_(),
  bigBlobbyImage_( BIG_BLOBBY_IMAGE_FILE ),
  touchHooks2TuioImage_( TOUCH_HOOKS_2_TUIO_IMAGE_FILE ),
  windows8TouchImage_( WINDOWS_8_TOUCH_IMAGE_FILE ),
  tuioGeneratorIcon_( NULL ),
  reloadIcon_( NULL ),
  shutdownIcon_( NULL ),
  appIcons_(),
  activatedAppIconIndex_( -1 ),
  loadingIconMovie_( NULL ),
  loadingIconLabel_( NULL ),
  drawGesturePanel_( false ),
  grayTuioGeneratorIcon_( false ),
  grayReloadIcon_( false ),
  grayShutdownIcon_( false ),
  grayActivatedAppIcon_( false ),
  drawBlackScreen_( false ),
  showLoadingAnimation_( false ),
  showShutdownComputerIcon_( false )
{
    resize( parentWidth_, parentHeight_ );
    setBackgroundColor();
    setBackgroundImage();
    setTuioGeneratorIcon();
    setReloadIcon();
    setShutdownIcon();
    setLoadingIconAnimation();
}

PaintWidget::~PaintWidget()
{
    delete tuioGeneratorIcon_;
    delete reloadIcon_;
    delete shutdownIcon_;
    clearAppIcons();
}

void PaintWidget::setBackgroundColor()
{
    QPalette pal = palette();
    pal.setColor( QPalette::Window, BACKGROUND_COLOR );
    setAutoFillBackground( true );
    setPalette( pal );
}

void PaintWidget::setBackgroundImage()
{
    backgroundImage_.load( BACKGROUND_IMAGE_FILE );
}

void PaintWidget::setTuioGeneratorIcon()
{
    if( tuioSource_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
        tuioGeneratorIcon_ = new gui::AppIcon( touchHooks2TuioImage_ );
    }
    else if( tuioSource_ == enums::TuioSourceEnum::WINDOWS_8_TOUCH ) {
        tuioGeneratorIcon_ = new gui::AppIcon( windows8TouchImage_ );
    }
    else { // Must be BigBlobby.
        tuioGeneratorIcon_ = new gui::AppIcon( bigBlobbyImage_ );
    }
}

void PaintWidget::setTuioGeneratorIcon( enums::TuioSourceEnum::Enum tuioSource )
{
    tuioSource_ = tuioSource;

    if( tuioSource_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
        tuioGeneratorIcon_->setImage( touchHooks2TuioImage_ );
    }
    else if( tuioSource_ == enums::TuioSourceEnum::WINDOWS_8_TOUCH ) {
        tuioGeneratorIcon_->setImage( windows8TouchImage_ );
    }
    else { // Must be BigBlobby.
        tuioGeneratorIcon_->setImage( bigBlobbyImage_ );
    }
}

void PaintWidget::setReloadIcon()
{
    QImage reloadImage( RELOAD_ICON_IMAGE_FILE );
    reloadIcon_ = new gui::AppIcon( reloadImage );
}

void PaintWidget::setShutdownIcon()
{
    QImage shutdownImage( SHUTDOWN_ICON_IMAGE_FILE );
    shutdownIcon_ = new gui::AppIcon( shutdownImage );
}

void PaintWidget::setLoadingIconAnimation()
{
    loadingIconMovie_ = new QMovie( ":/QResources/icons/avatar-cat.gif" );
    //debugPrintIsMovieValid();

    if( loadingIconMovie_->isValid() ) {
        loadingIconLabel_ = new QLabel( this );
        loadingIconLabel_->setMovie( loadingIconMovie_ );
        loadingIconLabel_->setVisible( false );
    }
}

void PaintWidget::setWidthAndHeightOfParent( int width, int height )
{
    parentWidth_ = width;
    parentHeight_ = height;
    calculateIconPositions();
    resizeGesturePanelImage();
}

void PaintWidget::resizeGesturePanelImage()
{
    QImage newImage( width(), height(), QImage::Format_RGB32);
    gesturePanelImageRect_.setRect( 0, 0, width(), height() );
    newImage.fill( QColor( 255, 255, 255 ) );

    QPainter painter( &newImage );
    painter.drawImage( QPoint(0, 0), gesturePanelImage_ );
    gesturePanelImage_ = newImage;
}

gui::AppIcon * PaintWidget::tuioGeneratorAppIcon()
{
    return tuioGeneratorIcon_;
}

gui::AppIcon * PaintWidget::reloadAppIcon()
{
    return reloadIcon_;
}

gui::AppIcon * PaintWidget::shutdownAppIcon()
{
    return shutdownIcon_;
}

void PaintWidget::setTuioGeneratorIconToGray( bool b )
{
    grayTuioGeneratorIcon_ = b;
}

void PaintWidget::setReloadIconToGray( bool b )
{
    grayReloadIcon_ = b;
}

void PaintWidget::setShutdownIconToGray( bool b )
{
    grayShutdownIcon_ = b;
}

void PaintWidget::setActivatedAppIconToGray( bool b )
{
    grayActivatedAppIcon_ = b;
}

void PaintWidget::setActivatedAppIconIndex( int index )
{
    activatedAppIconIndex_ = index;
}

int PaintWidget::activatedAppIconIndex()
{
    return activatedAppIconIndex_;
}

void PaintWidget::drawGesturePanel( bool show )
{
    drawGesturePanel_ = show;
}

QImage * PaintWidget::gesturePanelImage()
{
    return &gesturePanelImage_;
}

void PaintWidget::clearGesturePanelImage()
{
    gesturePanelImage_.fill( QColor( 255, 255, 255 ) );

    if( drawGesturePanel_ ) {
        update();
    }
}

void PaintWidget::drawBlackScreen( bool b )
{
    drawBlackScreen_ = b;
    showLoadingAnimation_ = b;

    if( showLoadingAnimation_ && loadingIconLabel_ != NULL ) {
        centerLoadingIconLabel();
        loadingIconMovie_->start();
        loadingIconLabel_->setVisible( true );
        QTimer::singleShot( LOADING_ICON_TIMEOUT,
                            this,
                            SLOT( hideLoadingIconIfVisible() ) );
    }
}

void PaintWidget::centerLoadingIconLabel()
{
    if( loadingIconLabel_ != NULL ) {
        QSize movieSize = loadingIconMovie_->scaledSize();
        int iconX = (parentWidth_ - LOADING_ICON_WIDTH) / 2 - x(),
            iconY = (parentHeight_ - LOADING_ICON_HEIGHT) / 2;

        loadingIconLabel_->resize( LOADING_ICON_WIDTH, LOADING_ICON_HEIGHT );
        loadingIconLabel_->move( iconX, iconY );
        //std::cout << "parent width x height = " 
        //          << parentWidth_ << " x " << parentHeight_ << "\n"
        //          << "x = " << x() << "\n\n";
    }
}

std::vector<gui::AppIcon *> & PaintWidget::appIcons()
{
    return appIcons_;
}

/***************************************************************************//**
The PaintWidget is intended to be presented in a scroll window, the 
CentralWidget, so that the width of the PaintWidget can be adjusted if 
the number of app icons will not fit in a single screen width.
Therefore, after the icon images have been copied by this function, the 
helper function that calculates the icon positions will also adjust the 
width of the widget to make it wider than the CentralWidget if necessary.
*******************************************************************************/
void PaintWidget::setFileIcons( std::vector<QImage> * iconImages )
{
    clearAppIcons();

    for( int i = 0; i < iconImages->size(); ++i ) {
        appIcons_.push_back( new gui::AppIcon( iconImages->at( i ) ) );
    }
    calculateIconPositions();
}

void PaintWidget::showShutdownComputerIcon( bool b )
{
    showShutdownComputerIcon_ = b;
    calculateIconPositions();
}

void PaintWidget::calculateIconPositions()
{
    //std::cout << "PaintWidget::calculateIconPositions() called.\n";
    //debugPrintWidgetDimensions();
    //debugPrintNumberOfIcons();
    //bool menuBarVisible = mainWindow_->isMenuBarOpen();
    //int yStart = menuBarVisible ? Y_START : (Y_START + 21),
    int yStart = Y_START,
        x = X_START,
        y = yStart,
        lastX = x;
    //std::cout << "yStart = " << yStart << "\n";

    if( tuioGeneratorIcon_ != NULL ) {
        tuioGeneratorIcon_->setRect( x, y, IMAGE_WIDTH, IMAGE_HEIGHT );
        y += Y_SPACING + IMAGE_HEIGHT;

        if( y > height() - (IMAGE_HEIGHT + yStart) ) {
            x += IMAGE_WIDTH + X_SPACING;
            y = yStart;
        }
    }
    if( reloadIcon_ != NULL ) {
        reloadIcon_->setRect( x, y, IMAGE_WIDTH, IMAGE_HEIGHT );
        y += Y_SPACING + IMAGE_HEIGHT;

        if( y > height() - (IMAGE_HEIGHT + yStart) ) {
            x += IMAGE_WIDTH + X_SPACING;
            y = yStart;
        }
    }
    if( shutdownIcon_ != NULL ) {
        if( showShutdownComputerIcon_ ) {
            shutdownIcon_->setRect( x, y, IMAGE_WIDTH, IMAGE_HEIGHT );
            y += Y_SPACING + IMAGE_HEIGHT;

            if( y > height() - (IMAGE_HEIGHT + yStart) ) {
                x += IMAGE_WIDTH + X_SPACING;
                y = yStart;
            }
        }
        else { // The Shutdown Computer Icon is not showing, so the rectangle
            // should be something that will make the TouchDetector always
            // return false when a screen (x, y) is plugged in and tested 
            // for being on top of the shutdownIcon_.
            shutdownIcon_->setRect( -1000, -1000, 0, 0 );
        }
    }
    for( int i = 0; i < appIcons_.size(); ++i ) {
        appIcons_.at( i )->setRect( x, y, IMAGE_WIDTH, IMAGE_HEIGHT );
        y += Y_SPACING + IMAGE_HEIGHT;
        lastX = x;

        if( y > height() - (IMAGE_HEIGHT + yStart) ) {
            x += IMAGE_WIDTH + X_SPACING;
            y = yStart;
        }
    }
    // Need to resize widget based on last x value (plus IMAGE_WIDTH).
    int drawingWidth = lastX + IMAGE_WIDTH + X_START,
        widgetWidth = (drawingWidth > parentWidth_) ? drawingWidth : parentWidth_;
    resize( widgetWidth, parentHeight_ );
    //debugPrintWidgetDimensions();
}

void PaintWidget::clearAppIcons()
{
    for( int i = 0; i < appIcons_.size(); ++i ) {
        delete appIcons_.at( i );
    }
    appIcons_.clear();
}

/***************************************************************************//**
This function is only needed for debugging purposes.
*******************************************************************************/
void PaintWidget::resizeEvent( QResizeEvent * event )
{
    //std::cout << "\nPaintWidget::resizeEvent() called...\n";
    //debugPrintWidgetDimensions();
    QWidget::resizeEvent( event );
}

void PaintWidget::paintEvent( QPaintEvent * event )
{
    if( drawGesturePanel_ ) {
        drawGesturePanel();
    }
    else if( drawBlackScreen_ ) {
        drawSolidBlackScreen();
    }
    else {
        drawAllIcons();
    }
}

void PaintWidget::drawGesturePanel()
{
    hideLoadingIconIfVisible();
    QPainter painter( this );
    painter.drawImage( gesturePanelImageRect_, gesturePanelImage_ );
    drawGesturePanelMessage( painter );
}

void PaintWidget::hideLoadingIconIfVisible()
{
    if( loadingIconLabel_ != NULL && loadingIconLabel_->isVisible() ) {
        loadingIconMovie_->stop();
        loadingIconLabel_->setVisible( false );
    }
}

void PaintWidget::drawGesturePanelMessage( QPainter & painter )
{
    int x = 10,
        y1 = 20,
        y2 = 40,
        y3 = 60,
        y4 = 80,
        textPointSize = 10;

    QFont font;
    font.setPointSize( textPointSize );
    painter.setFont( font );
    painter.setPen( Qt::red );
    painter.drawText( QPoint( x, y1 ), GESTURE_PANEL_MESSAGE_1 );
    painter.drawText( QPoint( x, y2 ), GESTURE_PANEL_MESSAGE_2 );
    painter.drawText( QPoint( x, y3 ), GESTURE_PANEL_MESSAGE_3 );
    painter.drawText( QPoint( x, y4 ), GESTURE_PANEL_MESSAGE_4 );
}

void PaintWidget::drawSolidBlackScreen()
{
    QPainter painter( this );
    painter.setBrush( SOLID_BLACK_BRUSH );
    painter.drawRect( 0, 0, width(), height() );
}

void PaintWidget::drawAllIcons()
{
    hideLoadingIconIfVisible();
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    drawBackground( painter );
    drawTuioGeneratorIcon( painter );
    drawReloadIcon( painter );
    drawShutdownIcon( painter );
    drawMultitouchAppIcons( painter );
}

void PaintWidget::drawBackground( QPainter & painter )
{
    int x = 0,
        y = 0,
        w = parentWidth_,
        h = parentHeight_;

    // Always draw the background image at least once.
    QRect rect( x, y, w, h );
    painter.setBrush( Qt::NoBrush );
    painter.drawImage( rect, backgroundImage_ );

    // Check if PaintWidget is wide enough to need left-right tiling of background image.
    int paintWidgetWidth = width();

    if( paintWidgetWidth > parentWidth_ ) {
        int n = paintWidgetWidth / parentWidth_,
            r = paintWidgetWidth % parentWidth_;

        for( int i = 0; i < n; ++i ) {
            x += parentWidth_;
            rect.setRect( x, y, w, h );
            painter.drawImage( rect, backgroundImage_ );
        }
    }
}

void PaintWidget::drawTuioGeneratorIcon( QPainter & painter )
{
    if( tuioGeneratorIcon_ != NULL ) {
        if( tuioSource_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
            painter.setBrush( Qt::NoBrush );
            painter.drawImage( tuioGeneratorIcon_->rect(), tuioGeneratorIcon_->iconImage() );

            if( grayTuioGeneratorIcon_  ) {
                painter.setBrush( TRANSLUCENT_LIGHT_GRAY_BRUSH );
                painter.drawRoundedRect( tuioGeneratorIcon_->rect(), 10, 10 );
            }
            painter.setBrush( Qt::NoBrush );
            painter.setPen( THIN_BORDER_PEN );
            painter.drawRoundedRect( tuioGeneratorIcon_->rect(), 10, 10 );
        }
        else { // Must be BigBlobby.
            painter.setBrush( grayTuioGeneratorIcon_ ? TRANSLUCENT_GRAY_BRUSH : SOLID_BLACK_BRUSH );
            painter.drawRoundedRect( tuioGeneratorIcon_->rect(), 10, 10 );

            painter.setBrush( grayTuioGeneratorIcon_ ? TRANSLUCENT_GRAY_BRUSH : Qt::NoBrush );
            painter.drawImage( tuioGeneratorIcon_->rect(), tuioGeneratorIcon_->iconImage() );
        }
    }
}

void PaintWidget::drawReloadIcon( QPainter & painter )
{
    if( reloadIcon_ != NULL ) {
        painter.setBrush( grayReloadIcon_ ? TRANSLUCENT_GRAY_BRUSH : SOLID_BLACK_BRUSH );
        painter.drawRoundedRect( reloadIcon_->rect(), 10, 10 );

        painter.setBrush( grayReloadIcon_ ? TRANSLUCENT_GRAY_BRUSH : Qt::NoBrush );
        painter.drawImage( reloadIcon_->rect(), reloadIcon_->iconImage() );
    }
}

void PaintWidget::drawShutdownIcon( QPainter & painter )
{
    if( shutdownIcon_ != NULL ) {
        painter.setBrush( grayShutdownIcon_ ? TRANSLUCENT_GRAY_BRUSH : SOLID_BLACK_BRUSH );
        painter.drawRoundedRect( shutdownIcon_->rect(), 10, 10 );

        painter.setBrush( grayShutdownIcon_ ? TRANSLUCENT_GRAY_BRUSH : Qt::NoBrush );
        painter.drawImage( shutdownIcon_->rect(), shutdownIcon_->iconImage() );
    }
}

void PaintWidget::drawMultitouchAppIcons( QPainter & painter )
{
    for( int i = 0; i < appIcons_.size(); ++i ) {
        gui::AppIcon * appIcon = appIcons_.at( i );

        //painter.setPen( Qt::NoPen );
        painter.setBrush( SOLID_WHITE_BRUSH );
        painter.drawRoundedRect( appIcon->rect(), 10, 10 );

        painter.setBrush( Qt::NoBrush );
        painter.drawImage( appIcon->rect(), appIcon->iconImage() );

        if( grayActivatedAppIcon_ && i == activatedAppIconIndex_  ) {
            painter.setBrush( TRANSLUCENT_DARK_GRAY_BRUSH );
            painter.drawRoundedRect( appIcon->rect(), 10, 10 );
        }
    }
}

void PaintWidget::debugPrintWidgetDimensions()
{
    std::cout << "    widget(w, h) = (" 
              << width() << ", " << height() << ")\n"
              << "    parent(w, h) = (" 
              << parentWidth_ << ", " << parentHeight_ << ")\n";
}

void PaintWidget::debugPrintNumberOfIcons()
{
    std::cout << "    Number of Icons = " << appIcons_.size() << "\n";
}

void PaintWidget::debugPrintIsMovieValid()
{
    if( loadingIconMovie_ != NULL && loadingIconMovie_->isValid() ) {
        std::cout << "Movie is valid.\n";
    }
    else {
        std::cout << "Movie is not valid.\n";
    }
}
