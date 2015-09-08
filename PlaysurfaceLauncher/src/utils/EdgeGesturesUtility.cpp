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
#include "utils/EdgeGesturesUtility.h"
#include "utils/WindowUtility.h"
#include "utils/StopWatch.h"
#include <QApplication>
#include <QTimer>
#include <Windows.h>
#include <Propsys.h>
#include <Propkey.h>
#include <strsafe.h>
#include <iostream>

using utils::EdgeGesturesUtility;

const int EdgeGesturesUtility::TIME_INTERVAL_MILLISEC = 100,
          EdgeGesturesUtility::NUMBER_OF_ATTEMPTS = 300;

static bool s_edgeGesturesDisabled_ = false;

static bool isFullscreenWindow( HWND hwnd )
{
    RECT windowRect,
         desktopRect;
    GetWindowRect( hwnd, &windowRect );
    GetWindowRect( GetDesktopWindow(), &desktopRect );

    return (windowRect.left == desktopRect.left)
        && (windowRect.top == desktopRect.top)
        && (windowRect.right == desktopRect.right)
        && (windowRect.bottom == desktopRect.bottom);
}

static void printWindowRect( HWND hwnd )
{
    RECT windowRect;
    GetWindowRect( hwnd, &windowRect );

    std::cout << "windowRect = (" 
              << windowRect.left << ", "
              << windowRect.top << ", "
              << windowRect.right << ", "
              << windowRect.bottom << ")\n";
}

static void printWindowInfo( DWORD pid, HWND hwnd )
{
    std::cout << "pid = " << pid
              << " and hwnd = " << hwnd << "\n"
              << "windowVisible = " << IsWindowVisible( hwnd ) 
              << " and windowFullscreen = " << isFullscreenWindow( hwnd ) << "\n";

    printWindowRect( hwnd );
}

static void disableTouchBasedEdgeGestures( HWND hwnd )
{
    IPropertyStore * pPropStore;
    HRESULT hrReturnValue = SHGetPropertyStoreForWindow( hwnd, IID_PPV_ARGS( &pPropStore ) );

    if( SUCCEEDED( hrReturnValue ) ) {
        PROPVARIANT var;
        var.vt = VT_BOOL;
        var.boolVal = VARIANT_TRUE;

        hrReturnValue = pPropStore->SetValue( PKEY_EdgeGesture_DisableTouchWhenFullscreen, var );
        pPropStore->Release();

        if( hrReturnValue == S_OK ) {
            s_edgeGesturesDisabled_ = true;
            //std::cout << "Touch-based edge gestures are disabled.\n";
        }
        else {
            //std::cout << "Touch-based edge gestures could not be disabled.\n";
        }
    }
}

static BOOL CALLBACK disableEdgeGesturesWhenFullscreen( HWND hwnd, LPARAM lParam )
{
    DWORD targetPid = *reinterpret_cast<WORD*>(lParam);
    DWORD windowParentPid = NULL;
    GetWindowThreadProcessId( hwnd, &windowParentPid );

    if( windowParentPid == targetPid ) {
        //printWindowInfo( targetPid, hwnd );

        if( IsWindowVisible( hwnd ) && isFullscreenWindow( hwnd ) ) {
            //printWindowInfo( targetPid, hwnd );
            disableTouchBasedEdgeGestures( hwnd );
        }
    }
    return TRUE; // Continue the enumeration.
}

EdgeGesturesUtility::EdgeGesturesUtility() :
  stopWatch_( new utils::StopWatch() ),
  fullscreenAppPid_( 0 ),
  playsurfaceLauncherPid_( 0 ),
  fullscreenAppCountdown_( 0 ),
  playsurfaceLauncherCountdown_( 0 )
{
}

EdgeGesturesUtility::~EdgeGesturesUtility()
{
    delete stopWatch_;
}

void EdgeGesturesUtility::disableFullscreenAppEdgeGestures( unsigned long int pid )
{
    //std::cout << "\nEdgeGesturesUtility::disableFullscreenAppEdgeGestures(" << pid << ")\n";
    fullscreenAppPid_ = pid;
    fullscreenAppCountdown_ = NUMBER_OF_ATTEMPTS;
    disableFullscreenAppEdgeGestures();
}

void EdgeGesturesUtility::disableFullscreenAppEdgeGestures()
{
    if( fullscreenAppCountdown_ > 0  ) {
        --fullscreenAppCountdown_;

        if( fullscreenAppPid_ != 0 ) {
            s_edgeGesturesDisabled_ = false;
            EnumWindows( disableEdgeGesturesWhenFullscreen, reinterpret_cast<LPARAM>(&fullscreenAppPid_) );

            if( s_edgeGesturesDisabled_ ) {
                //std::cout << "Edge gestures disabled for pid " << fullscreenAppPid_ << ".\n";
                fullscreenAppCountdown_ = 0;
            }
            else { // Try again after a fraction of a second.
                //std::cout << "Will try again in " << TIME_INTERVAL_MILLISEC << " milliseconds.\n";
                QTimer::singleShot( TIME_INTERVAL_MILLISEC,
                                    this,
                                    SLOT( disableFullscreenAppEdgeGestures() ) );
            }
        }
    }
}

void EdgeGesturesUtility::disablePlaysurfaceLauncherEdgeGestures()
{
    //std::cout << "\nEdgeGesturesUtility::disablePlaysurfaceLauncherEdgeGestures() called...\n";
    playsurfaceLauncherPid_ = (unsigned long)QApplication::instance()->applicationPid();
    playsurfaceLauncherCountdown_ = NUMBER_OF_ATTEMPTS;
    disablePlaysurfaceEdgeGestures();
}

void EdgeGesturesUtility::disablePlaysurfaceEdgeGestures()
{
    if( playsurfaceLauncherCountdown_ > 0  ) {
        --playsurfaceLauncherCountdown_;

        if( playsurfaceLauncherPid_ != 0 ) {
            s_edgeGesturesDisabled_ = false;
            EnumWindows( disableEdgeGesturesWhenFullscreen, reinterpret_cast<LPARAM>(&playsurfaceLauncherPid_) );

            if( s_edgeGesturesDisabled_ ) {
                //std::cout << "Edge gestures disabled for PlaysurfaceLauncher desktop.\n";
                playsurfaceLauncherCountdown_ = 0;
            }
            else { // Try again after a fraction of a second.
                //std::cout << "Will try again in " << TIME_INTERVAL_MILLISEC << " milliseconds.\n";
                QTimer::singleShot( TIME_INTERVAL_MILLISEC,
                                    this,
                                    SLOT( disablePlaysurfaceEdgeGestures() ) );
            }
        }
    }
}
