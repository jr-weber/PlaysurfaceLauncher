/*******************************************************************************
SidewaysSwipe

PURPOSE: Holds information on a swipe gesture across the PlaysurfaceLauncher
         desktop.  The swipe gesture is only needed if there are more icons
         than will fit in the available monitor space.

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
#ifndef GESTURES_SIDEWAYSSWIPE_H
#define GESTURES_SIDEWAYSSWIPE_H

#include <QObject>
class QTimer;

namespace gestures 
{
    class SidewaysSwipe : public QObject
    {
        Q_OBJECT

    public:
        static const int TIMEOUT_IN_MILLISECONDS;

        enum SwipeState
        {
            READY_TO_START,
            SWIPE_STARTED,
            SWIPING_RIGHT,
            SWIPING_LEFT
        };

        SidewaysSwipe();
        virtual ~SidewaysSwipe();

        bool isReadyToStart();
        void startSwipe( int id, int x );
        int blobID();
        int lengthOfSwipe( int x );
        void clear();

    public slots:
        void timeOut();

    private:
        SwipeState state_;
        QTimer * timer_;
        int blobID_,
            startX_,
            previousX_,
            deltaX_;
    };
}

#endif
