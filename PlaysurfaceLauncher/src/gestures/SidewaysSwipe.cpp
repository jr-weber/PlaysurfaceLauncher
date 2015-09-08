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
#include "gestures/SidewaysSwipe.h"
#include <QTimer>
#include <iostream>

using gestures::SidewaysSwipe;

const int SidewaysSwipe::TIMEOUT_IN_MILLISECONDS = 3000;

SidewaysSwipe::SidewaysSwipe() :
  state_( READY_TO_START ),
  timer_( NULL ),
  blobID_( -1 ),
  startX_( 0 ),
  previousX_( 0 ),
  deltaX_( 0 )
{

}

SidewaysSwipe::~SidewaysSwipe()
{
}

bool SidewaysSwipe::isReadyToStart()
{
    return (state_ == READY_TO_START);
}

void SidewaysSwipe::startSwipe( int id, int x )
{
    //std::cout << "SidewaysSwipe::startSwipe(" << id << ", " <<  x << ") called...\n";
    state_ = SWIPE_STARTED;
    blobID_ = id;
    startX_ = x;
    previousX_ = x;
    deltaX_ = 0;

    timer_ = new QTimer( this );
    connect( timer_, SIGNAL( timeout() ), this, SLOT( timeOut() ) );
    timer_->setSingleShot( true );
    timer_->start( TIMEOUT_IN_MILLISECONDS );
}

int SidewaysSwipe::blobID()
{
    return blobID_;
}

int SidewaysSwipe::lengthOfSwipe( int x )
{
    deltaX_ = x - previousX_;
    previousX_ = x;
    return deltaX_;
}

void SidewaysSwipe::clear()
{
    //std::cout << "SidewaysSwipe::clear() called...\n";
    delete timer_;
    state_ = READY_TO_START;
    blobID_ = -1;
    startX_ = 0;
    previousX_ = 0;
    deltaX_ = 0;
}

void SidewaysSwipe::timeOut()
{
    //std::cout << "SidewaysSwipe::timeOut() called...\n";
    clear();
}
