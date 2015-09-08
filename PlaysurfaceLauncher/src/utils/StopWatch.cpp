/*******************************************************************************
StopWatch

PURPOSE: Provides a time keeper for benchmarking functions.  The time is
         reported as a floating decimal place number in seconds, but should
         have microsecond accuracy.

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
#include "utils/StopWatch.h"
#include <iostream>

using utils::StopWatch;
using std::chrono::system_clock;

StopWatch::StopWatch() :
  startTime_( system_clock::now() )
{
}

StopWatch::~StopWatch()
{
}

void StopWatch::start()
{
    startTime_ = system_clock::now();
}

double StopWatch::restart()
{
    std::chrono::duration<double> sec = system_clock::now() - startTime_;
    startTime_ = system_clock::now();
    return sec.count();
}

double StopWatch::elapsed()
{
    std::chrono::duration<double> sec = system_clock::now() - startTime_;
    return sec.count();
}

void StopWatch::print( double timeInSeconds )
{
    std::cout << "t = " << timeInSeconds << " seconds\n" << std::flush;
}
