/*******************************************************************************
FullScreenWindow

PURPOSE: Provides the full screen window for the PlaysurfaceLauncher, which 
         presents a menu bar (when a user mouse clicks near the top of the 
         screen) and holds the CentralWidget that controls rendering of 
         multitouch app icons and listens for touch events.

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
#ifndef GUI_FULLSCREENWINDOW_H
#define GUI_FULLSCREENWINDOW_H

#include "enums/TuioSourceEnum.h"
#include <QMainWindow>
#include <Windows.h>

namespace playsurface { class Playsurface; }
namespace app { class AppLauncher; }
namespace utils { class TaskbarUtility; }
namespace gui { class CentralWidget; }
namespace gui { class Help; }
namespace gui { class XmlSettings; }
namespace sound { class IconSound; }
namespace localclient { class BigBlobbyClient; }
namespace localclient { class TouchHooksClient; }
class QActionGroup;

namespace gui
{
    class FullScreenWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        FullScreenWindow( playsurface::Playsurface * parent,
                          app::AppLauncher * appLauncher,
                          utils::TaskbarUtility * taskbarUtility,
                          gui::XmlSettings * xmlSettings,
                          bool showGesturesMenu = false );
        virtual ~FullScreenWindow();

        enums::TuioSourceEnum::Enum tuioGenerator();
        void readMultitouchAppsInfo();
        void showFullScreenWindow();
        void setGesturesMenuPlusSignOption( bool usePlusSign );
        bool isPlusSignGestureInUse();
        void setOptionsMenuShowShutdownComputerIcon( bool b );
        bool isShutdownComputerIconShowing();
        
    public slots:
        void shutdownComputer();
        void exitWithoutShuttingDownProjector();
        void closeProgramAfterWarningTuioGenerator();
        void showTaskbar();
        void hideTaskbar();
        void onSoundSelected( bool b );
        void hideMenuBar();
        void showMenuBar();
        bool isMenuBarOpen();
        void updateMainWindowSettings();

        void onBigBlobbyGeneratorSelected( bool b );
        void onTouchHooks2TuioGeneratorSelected( bool b );
        void onWindows8TouchSelected( bool b );

    protected:
        virtual void closeEvent( QCloseEvent * event );
        virtual void keyPressEvent( QKeyEvent * e );
        virtual bool winEvent( MSG * msg, long * result );

    private:
        void createFileMenu();
        void createOptionsMenu();
        void createGesturesMenu();
        void createSoundMenu();
        void createHelpMenu();
        char getDriveLetterFromMsg( MSG * msg );
        char firstDriveFromMask( ULONG unitmask );

        playsurface::Playsurface * playsurface_;
        app::AppLauncher * appLauncher_;
        utils::TaskbarUtility * taskbarUtility_;
        gui::CentralWidget * centralWidget_;
        gui::Help * help_;

        QMenu * fileMenu_;
        QAction * exitAct_;
        QAction * shutdownAct_;

        QMenu * optionsMenu_;
        QAction * showTaskbarAct_;
        QAction * hideTaskbarAct_;

        QMenu * touchDetectorMenu_;
        QAction * bigBlobbyAct_;
        QAction * touchHooksAct_;
        QAction * windows8TouchAct_;
        QActionGroup * touchDetectorGroup_;
        
        QAction * showShutdownComputerAct_;

        QMenu * gesturesMenu_;
        QAction * usePlusGestureToQuitApps_,
                * gesturesModeAct_,
                * clearGesturesPanelAct_;

        QMenu * soundMenu_;
        QActionGroup * soundActionGroup_;
        sound::IconSound * iconSound_;

        QMenu * helpMenu_;
        QAction * aboutBoxAct_,
                * onlineHelpAct_,
                * menuSystemHelpAct_;

        bool isMenuBarVisible_;

        localclient::BigBlobbyClient * bigBlobbyClient_;
        localclient::TouchHooksClient * touchHooksClient_;
        gui::XmlSettings * xmlSettings_;
    };
}

#endif
