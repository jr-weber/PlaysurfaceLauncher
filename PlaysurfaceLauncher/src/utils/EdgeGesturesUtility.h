/*******************************************************************************
EdgeGesturesUtility

PURPOSE: Provides functions for disabling the Windows 8 touch-based edge gesture
         used to open the Charms Bar and other annoying features of Windows 8.
         An "edge gesture" is a touch gesture that starts near a monitor edge
         and then moves quickly directly away from the edge.

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
#ifndef UTILS_EDGEGESTURESUTILITY_H
#define UTILS_EDGEGESTURESUTILITY_H

#include <QObject>

struct _PROCESS_INFORMATION;

namespace utils { class StopWatch; }

namespace utils
{
    class EdgeGesturesUtility : public QObject
    {
        Q_OBJECT

    public:
        static const int TIME_INTERVAL_MILLISEC,
                         NUMBER_OF_ATTEMPTS;

        EdgeGesturesUtility();
        virtual ~EdgeGesturesUtility();

        void disableFullscreenAppEdgeGestures( unsigned long pid );
        void disablePlaysurfaceLauncherEdgeGestures();

    public slots:
        void disableFullscreenAppEdgeGestures();
        void disablePlaysurfaceEdgeGestures();


    private:
        utils::StopWatch * stopWatch_;
        unsigned long fullscreenAppPid_,
                      playsurfaceLauncherPid_;
        int fullscreenAppCountdown_,
            playsurfaceLauncherCountdown_;
    };
}

#endif
