/*******************************************************************************
GestureDetector

PURPOSE: Detects horizontal and vertical line gestures, and whether a horizontal
         and vertical line form a plus sign.

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
#ifndef GESTURES_GESTUREDETECTOR_H
#define GESTURES_GESTUREDETECTOR_H

#include "gestures/GestureResult.h"
#include <list>
#include <string>
#include <vector>

namespace gestures { class LineGesture; }

namespace gestures
{
    class GestureDetector
    {
    public:
        GestureDetector();
        virtual ~GestureDetector();

        void clear();
        bool startGesture( int id, double x, double y );
        void updateGesture( int id, double x, double y );
        bool finishGestureAndTestForPlusSign( int id, double x, double y );
        gestures::GestureResult finishGestureAndCalculateResult( int id, double x, double y );

    private:
        void clearInProgressGestures();
        void clearCompletedGestures();
        void clearExpiredCompletedGestures();
        gestures::GestureResult searchCompletedGestures( gestures::LineGesture * gesture );
        bool searchForPlusSign( gestures::LineGesture * gesture );
        bool isIdFoundInGestureList( int id );

        std::list<gestures::LineGesture *> inProgressGestures_,
                                           completedGestures_;
    };
}

#endif
