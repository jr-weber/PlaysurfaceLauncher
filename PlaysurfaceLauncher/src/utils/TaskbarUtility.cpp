/*******************************************************************************
TaskbarUtility

PURPOSE: Provides functions for showing/hiding the taskbar that is usually
         present at the bottom of the Windows 7/8 desktop.

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
#include "utils/TaskbarUtility.h"
#include <iostream>

//extern "C" {
#include <Windows.h>
//};

using utils::TaskbarUtility;

TaskbarUtility::TaskbarUtility() :
  reopenTaskbarOnExit_( true )
{
    
}

TaskbarUtility::~TaskbarUtility()
{
    if( reopenTaskbarOnExit_ ) {
        showTaskbar();
    }
}

void TaskbarUtility::hideTaskbarIfOpen()
{
    HWND taskbarHandle = FindWindow( L"Shell_TrayWnd", L"" );
    HWND startButtonHandle = FindWindow( L"Button", L"Start" );
    bool isVisible = IsWindowVisible( taskbarHandle );

    if( isVisible ) {
        reopenTaskbarOnExit_ = true;
        ShowWindow( taskbarHandle, FALSE );
        ShowWindow( startButtonHandle, FALSE );
    }
}

void TaskbarUtility::hideTaskbar()
{
    HWND taskbarHandle = FindWindow( L"Shell_TrayWnd", L"" );
    HWND startButtonHandle = FindWindow( L"Button", L"Start" );
    bool isVisible = IsWindowVisible( taskbarHandle );

    //std::cout << "taskbarHandle = " << taskbarHandle << "\n";
    //std::cout << "startButtonHandle = " << startButtonHandle << "\n";
    //std::cout << "isVisible = " << isVisible << " (about to hide)\n";
    
    ShowWindow( taskbarHandle, FALSE );
    ShowWindow( startButtonHandle, FALSE );
}

void TaskbarUtility::showTaskbar()
{
    HWND taskbarHandle = FindWindow( L"Shell_TrayWnd", L"" );
    HWND startButtonHandle = FindWindow( L"Button", L"Start" );
    bool isVisible = IsWindowVisible( taskbarHandle );
    
    //std::cout << "taskbarHandle = " << taskbarHandle << "\n";
    //std::cout << "startButtonHandle = " << startButtonHandle << "\n";
    //std::cout << "isVisible = " << isVisible << " (about to show)\n";

    ShowWindow( taskbarHandle, TRUE );
    ShowWindow( startButtonHandle, TRUE );
}