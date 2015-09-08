/*******************************************************************************
main.cpp

PURPOSE: Provides a game console-like environment for launching TUIO-based 
         multitouch apps.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
CREATED: 3/27/2014
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
#include "playsurface/Playsurface.h"
#include <QApplication>
#include <iostream>

// function prototypes
bool showGesturesMenu( int, char * [] );

int main( int argc, char * argv [] )
{
    QApplication app( argc, argv );
    //bool gesturesMenu = showGesturesMenu( argc, argv );
    bool gesturesMenu = true;

    playsurface::Playsurface play;
    play.killBigBlobbyIfAlreadyOpen();
    //play.killTouchHooks2TuioIfAlreadyOpen();
    play.readBigBlobbyInfo();
    play.readTouchHooks2TuioInfo();
    play.hideBigBlobbyOnStartup();
    play.hideTouchHooks2TuioOnStartup();
    play.readXmlConfigFile();
    play.launchTuioGenerator();
    play.readFlashPlayerInfo();
    play.turnOnProjectorIfFound();
    play.createFullScreenWindow( gesturesMenu );
    play.readMultitouchAppsInfo();
    play.startTuioClient();
    play.hideTaskBarIfOpen();
    play.showFullScreenWindow();
    play.disablePlaysurfaceLauncherEdgeGestures();
    play.placeAllOtherWindowsInTaskbar();
    play.updateMainWindowSettings();
    play.reportErrors();

    return app.exec();
}

static bool showGesturesMenu( int argc, char * argv[] )
{
    bool show = false;

    if( argc > 1 ) {
        QString arg = argv[1];

        if( arg.startsWith( "-g", Qt::CaseInsensitive ) ) {
            show = true;
            //std::cout << "-g command line arg detected\n";
        }
    }
    return show;
}
