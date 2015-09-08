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
#ifndef GUI_PAINTWIDGET_H
#define GUI_PAINTWIDGET_H

#include "enums/TuioSourceEnum.h"
#include <QWidget>
#include <QImage>
#include <QPen>
#include <vector>

namespace gui { class AppIcon; }
namespace gui { class FullScreenWindow; }
class QMovie;
class QLabel;

namespace gui 
{
    class PaintWidget : public QWidget
    {
        Q_OBJECT

    public:
        static const QString GESTURE_PANEL_MESSAGE_1,
                             GESTURE_PANEL_MESSAGE_2,
                             GESTURE_PANEL_MESSAGE_3,
                             GESTURE_PANEL_MESSAGE_4,
                             BACKGROUND_IMAGE_FILE,
                             BIG_BLOBBY_IMAGE_FILE,
                             TOUCH_HOOKS_2_TUIO_IMAGE_FILE,
                             WINDOWS_8_TOUCH_IMAGE_FILE,
                             RELOAD_ICON_IMAGE_FILE,
                             SHUTDOWN_ICON_IMAGE_FILE;
        static const int LOADING_ICON_TIMEOUT,
                         X_START,
                         Y_START,
                         X_SPACING,
                         Y_SPACING,
                         IMAGE_WIDTH,
                         IMAGE_HEIGHT;
        static const double GESTURE_BOUNDARY_FACTOR;
        static const QColor BACKGROUND_COLOR;
        static const QPen THIN_BORDER_PEN;
        static const QBrush SOLID_BLACK_BRUSH,
                            SOLID_WHITE_BRUSH,
                            TRANSLUCENT_GRAY_BRUSH,
                            TRANSLUCENT_LIGHT_GRAY_BRUSH,
                            TRANSLUCENT_DARK_GRAY_BRUSH;
        static const int LOADING_ICON_WIDTH,
                         LOADING_ICON_HEIGHT;

        PaintWidget( gui::FullScreenWindow * parent, enums::TuioSourceEnum::Enum tuioSource );
        virtual ~PaintWidget();

        void setTuioGeneratorIcon( enums::TuioSourceEnum::Enum tuioSource );
        void setWidthAndHeightOfParent( int width, int height );
        gui::AppIcon * tuioGeneratorAppIcon();
        gui::AppIcon * reloadAppIcon();
        gui::AppIcon * shutdownAppIcon();
        void setTuioGeneratorIconToGray( bool b );
        void setReloadIconToGray( bool b );
        void setShutdownIconToGray( bool b );
        void setActivatedAppIconToGray( bool b );
        void setActivatedAppIconIndex( int index );
        int activatedAppIconIndex();
        void drawGesturePanel( bool show );
        QImage * gesturePanelImage();
        void clearGesturePanelImage();
        void drawBlackScreen( bool b );
        std::vector<gui::AppIcon *> & appIcons();

        void setFileIcons( std::vector<QImage> * iconImages );
        void showShutdownComputerIcon( bool b );

    public slots:
        void hideLoadingIconIfVisible();

    protected:
        void resizeEvent( QResizeEvent * event );
        void paintEvent( QPaintEvent * event );

    private:
        void setBackgroundColor();
        void setBackgroundImage();
        void setTuioGeneratorIcon();
        void setReloadIcon();
        void setShutdownIcon();
        void setLoadingIconAnimation();
        void calculateIconPositions();
        void clearAppIcons();
        void centerLoadingIconLabel();

        void resizeGesturePanelImage();
        void drawGesturePanel();
        void drawGesturePanelMessage( QPainter & painter );
        void drawSolidBlackScreen();
        void drawAllIcons();
        void drawBackground( QPainter & painter );
        void drawTuioGeneratorIcon( QPainter & painter );
        void drawReloadIcon( QPainter & painter );
        void drawShutdownIcon( QPainter & painter );
        void drawMultitouchAppIcons( QPainter & painter );

        void debugPrintWidgetDimensions();
        void debugPrintNumberOfIcons();
        void debugPrintIsMovieValid();

        gui::FullScreenWindow * mainWindow_;
        enums::TuioSourceEnum::Enum tuioSource_;
        int parentWidth_,
            parentHeight_;
        QImage backgroundImage_,
               gesturePanelImage_;
        QRect gesturePanelImageRect_;
        QImage bigBlobbyImage_,
               touchHooks2TuioImage_,
               windows8TouchImage_;
        gui::AppIcon * tuioGeneratorIcon_,
                     * reloadIcon_,
                     * shutdownIcon_;
        std::vector<gui::AppIcon *> appIcons_;
        int activatedAppIconIndex_;
        QMovie * loadingIconMovie_;
        QLabel * loadingIconLabel_;
        bool drawGesturePanel_,
             grayTuioGeneratorIcon_,
             grayReloadIcon_,
             grayShutdownIcon_,
             grayActivatedAppIcon_,
             drawBlackScreen_,
             showLoadingAnimation_,
             showShutdownComputerIcon_;
    };
}

#endif
