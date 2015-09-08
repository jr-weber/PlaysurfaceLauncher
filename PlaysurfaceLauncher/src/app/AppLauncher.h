/*******************************************************************************
AppLauncher

PURPOSE: Uses the QProcess class for launching other applications: BigBlobby,
         TouchHooks2Tuio, and TUIO-based multitouch apps.

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
#ifndef APP_APPLAUNCHER_H
#define APP_APPLAUNCHER_H

#include "enums/TuioSourceEnum.h"
#include <QObject>
#include <QImage>
#include <QProcess>
#include <vector>

namespace playsurface { class Playsurface; }
namespace io { class DirectoryReader; }
namespace app { class AppInfoList; }
namespace app { class AppInfo; }
namespace gui { class DialogBoxUtils; }
namespace utils { class WindowUtility; }
namespace utils { class EdgeGesturesUtility; }

namespace app
{
    class AppLauncher : public QObject
    {
        Q_OBJECT

    public:
        static const bool ASSUME_FLASHPLAYER_EXE_IS_IN_PATH;
        static const int FOCUS_DELAY_MILLISECONDS,
                         MOUSE_X,
                         MOUSE_Y,
                         DELAY_FOR_CLOSING_TUIO_SERVER_APP;

        AppLauncher( playsurface::Playsurface * parent );
        virtual ~AppLauncher();
        
        void launchTuioGenerator( enums::TuioSourceEnum::Enum tuioSource );
        void resetTuioGenerator( enums::TuioSourceEnum::Enum tuioSource );
        enums::TuioSourceEnum::Enum tuioGenerator();
        void killMultitouchAppProcess();
        std::vector<QImage> * multitouchAppIcons();
        bool hasMultitouchAppsDirectory( char driveLetter );
        bool isMultitouchAppsDirInUse( char driveLetter );
        void placeAllOtherWindowsInTaskbar();
        void hideBigBlobbyOnStartup();
        void hideTouchHooks2TuioOnStartup();
        void disablePlaysurfaceLauncherEdgeGestures();
        
        void killBigBlobbyIfAlreadyOpen();
        void killTouchHooks2TuioIfAlreadyOpen();
        
    public slots:
        void shutdownComputer();
        void readBigBlobbyInfo();
        void readTouchHooks2TuioInfo();
        void readFlashPlayerInfo();
        void toggleBigBlobby();
        void toggleTouchHooks2Tuio();
        void onTuioGeneratorIconTouched();
        void onBigBlobbyIconTouched();
        void onTouchHooks2TuioIconTouched();
        void onBigBlobbyStarted();
        void onTouchHooks2TuioStarted();
        void hideBigBlobby();
        void hideTouchHooks2Tuio();
        void reportBigBlobbyError( QProcess::ProcessError processError );
        void reportBigBlobbyFinished( int exitCode, QProcess::ExitStatus exitStatus );
        void reportTouchHooks2TuioError( QProcess::ProcessError processError );
        void reportTouchHooks2TuioFinished( int exitCode, QProcess::ExitStatus exitStatus );
        void readMultitouchAppsInfo();
        void reportAnyMultitouchAppInfoErrors();
        void launchMultitouchApp( int index );
        void hideMultitouchApp();
        void focusOnMultitouchApp();
        void onMultitouchProcessStarted();
        void disableEdgeGesturesForFullscreenApp();
        void reportMultitouchAppError( QProcess::ProcessError processError );
        void reportMultitouchAppFinished( int exitCode, QProcess::ExitStatus exitStatus );
        
        void launchBigBlobby();
        void launchTouchHooks2Tuio();
        void launchWindows8Touch();

    signals:
        void multitouchAppClosed();

    private:
        void killBigBlobbyProcess();
        void killTouchHooks2TuioProcess();
        void connectBigBlobbyProcess();
        void connectTouchHooks2TuioProcess();
        void connectMultitouchProcess();
        void launchFlashMultitouchApp( app::AppInfo * appInfo );
        void launchExeMultitouchApp( app::AppInfo * appInfo );
        void moveMouseToBottomCenter();

        playsurface::Playsurface * parent_;
        io::DirectoryReader * directoryReader_;
        gui::DialogBoxUtils * dialogBoxUtils_;

        app::AppInfo * bigBlobbyInfo_,
                     * touchHooksInfo_,
                     * flashPlayerInfo_;
        app::AppInfoList * appInfoList_;

        utils::WindowUtility * windowUtility_;
        utils::EdgeGesturesUtility * edgeGesturesUtility_;
        QProcess * bigBlobbyProcess_,
                 * touchHooksProcess_,
                 * multitouchProcess_;
        enums::TuioSourceEnum::Enum tuioGenerator_;
        bool hideBigBlobbyAfterStartup_,
             hideTouchHooks2TuioAfterStartup_;
    };
}

#endif
