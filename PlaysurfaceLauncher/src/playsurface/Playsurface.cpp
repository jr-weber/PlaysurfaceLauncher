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
#include "playsurface/Playsurface.h"
#include "app/AppLauncher.h"
#include "utils/TaskbarUtility.h"
#include "gui/XmlSettings.h"
#include "gui/FullScreenWindow.h"
#include "serialport/ProjectorPort.h"
#include "qtuio/QTuio.h"

using playsurface::Playsurface;

const int Playsurface::DEFAULT_TUIO_PORT = 3334;

Playsurface::Playsurface() :
  appLauncher_( new app::AppLauncher( this ) ),
  taskbarUtility_( new utils::TaskbarUtility() ),
  projectorPort_( new serialport::ProjectorPort() ),
  xmlSettings_( new gui::XmlSettings() ),
  fullScreenWindow_( NULL ),
  qTuio_( NULL ),
  shutDownProjector_( true )
{
}

Playsurface::~Playsurface()
{
    delete qTuio_;
    delete fullScreenWindow_;
    delete xmlSettings_;
    delete taskbarUtility_;
    delete appLauncher_;

    if( shutDownProjector_ ) {
        projectorPort_->turnOffProjectorIfFound();
    }
    delete projectorPort_;
}

void Playsurface::killBigBlobbyIfAlreadyOpen()
{
    appLauncher_->killBigBlobbyIfAlreadyOpen();
}

void Playsurface::killTouchHooks2TuioIfAlreadyOpen()
{
    appLauncher_->killTouchHooks2TuioIfAlreadyOpen();
}

void Playsurface::readBigBlobbyInfo()
{
    appLauncher_->readBigBlobbyInfo();
}

void Playsurface::readTouchHooks2TuioInfo()
{
    appLauncher_->readTouchHooks2TuioInfo();
}

void Playsurface::hideBigBlobbyOnStartup()
{
    appLauncher_->hideBigBlobbyOnStartup();
}

void Playsurface::hideTouchHooks2TuioOnStartup()
{
    appLauncher_->hideTouchHooks2TuioOnStartup();
}

void Playsurface::readXmlConfigFile()
{
    xmlSettings_->readXmlConfigFile();
}

void Playsurface::launchTuioGenerator()
{
    appLauncher_->launchTuioGenerator( xmlSettings_->tuioSource() );
}

void Playsurface::readFlashPlayerInfo()
{
    appLauncher_->readFlashPlayerInfo();
}

void Playsurface::turnOnProjectorIfFound()
{
    projectorPort_->turnOnProjectorIfFound();
}

void Playsurface::createFullScreenWindow( bool showGesturesMenu )
{
    fullScreenWindow_ = new gui::FullScreenWindow( this,
                                                   appLauncher_, 
                                                   taskbarUtility_,
                                                   xmlSettings_,
                                                   showGesturesMenu );
}

void Playsurface::readMultitouchAppsInfo()
{
    fullScreenWindow_->readMultitouchAppsInfo();
}

void Playsurface::startTuioClient()
{
    if( qTuio_ == NULL ) {
        qTuio_ = new qtuio::QTuio( fullScreenWindow_ );
        bool allow2DcurFiltering = xmlSettings_->isBigBlobbySelected();
        qTuio_->allowTuioProfileFiltering( allow2DcurFiltering );
        qTuio_->setTuioUdpPort( DEFAULT_TUIO_PORT );
        qTuio_->run();
    }
}

void Playsurface::allowTuio2DcurFiltering( bool b )
{
    if( qTuio_ != NULL ) {
        qTuio_->allowTuioProfileFiltering( b );
    }
}

void Playsurface::hideTaskBarIfOpen()
{
    taskbarUtility_->hideTaskbarIfOpen();
}

void Playsurface::disablePlaysurfaceLauncherEdgeGestures()
{
    appLauncher_->disablePlaysurfaceLauncherEdgeGestures();
}

void Playsurface::showFullScreenWindow()
{
    fullScreenWindow_->showFullScreenWindow();
}

void Playsurface::placeAllOtherWindowsInTaskbar()
{
    appLauncher_->placeAllOtherWindowsInTaskbar();
}

void Playsurface::updateMainWindowSettings()
{
    fullScreenWindow_->updateMainWindowSettings();
}

void Playsurface::reportErrors()
{
    appLauncher_->reportAnyMultitouchAppInfoErrors();
}

void Playsurface::shutDownProjectorIfOn( bool b )
{
    shutDownProjector_ = b;
}
