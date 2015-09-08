/*******************************************************************************
Playsurface

PURPOSE: Creates the main objects needed by the PlaysurfaceLauncher program and
         provides public functions that are expected to be called from main.cpp.

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
#ifndef PLAYSURFACE_PLAYSURFACE_H
#define PLAYSURFACE_PLAYSURFACE_H

#include <QObject>

namespace app { class AppLauncher; }
namespace utils { class TaskbarUtility; }
namespace serialport { class ProjectorPort; }
namespace gui { class XmlSettings; }
namespace gui { class FullScreenWindow; }
namespace qtuio { class QTuio; }

namespace playsurface
{
    class Playsurface : public QObject
    {
        Q_OBJECT

    public:
        static const int DEFAULT_TUIO_PORT;

        Playsurface();
        virtual ~Playsurface();

        void killBigBlobbyIfAlreadyOpen();
        void killTouchHooks2TuioIfAlreadyOpen();
        void readBigBlobbyInfo();
        void readTouchHooks2TuioInfo();
        void hideBigBlobbyOnStartup();
        void hideTouchHooks2TuioOnStartup();
        void readXmlConfigFile();
        void launchTuioGenerator();
        void readFlashPlayerInfo();
        void turnOnProjectorIfFound();
        void createFullScreenWindow( bool showGesturesMenu = false );
        void readMultitouchAppsInfo();
        void startTuioClient();
        void hideTaskBarIfOpen();
        void disablePlaysurfaceLauncherEdgeGestures();
        void showFullScreenWindow();
        void placeAllOtherWindowsInTaskbar();
        void updateMainWindowSettings();
        void reportErrors();
        void shutDownProjectorIfOn( bool b );
        void allowTuio2DcurFiltering( bool b );
        
    private:
        app::AppLauncher * appLauncher_;
        utils::TaskbarUtility * taskbarUtility_;
        serialport::ProjectorPort * projectorPort_;
        gui::XmlSettings * xmlSettings_;
        gui::FullScreenWindow * fullScreenWindow_;
        qtuio::QTuio * qTuio_;
        bool shutDownProjector_;
    };
}

#endif
