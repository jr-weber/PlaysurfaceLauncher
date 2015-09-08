/*******************************************************************************
WindowUtility

PURPOSE: Provides functions for manipulating the windows provided by the
         Microsoft Windows 7/8 operationg system.

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
#include "utils/WindowUtility.h"
#include <Windows.h>
#include <Propsys.h>
#include <Propkey.h>
#include <strsafe.h>
#include <iostream>

using utils::WindowUtility;

// Windows with these titles should not be minimized.
static const wchar_t PROGRAM_MANAGER_TITLE [] = TEXT( "Program Manager" );
static const wchar_t SCR_HOTS_TITLE [] = TEXT("ScrHots");
static const wchar_t EMPTY_TITLE [] = TEXT("");

static const wchar_t TITLE_OF_BIGBLOBBY_WINDOW [] = TEXT( "Big Blobby" );
static const DWORD TITLE_LENGTH_FOR_BIGBLOBBY = 10;
static unsigned long pidBigBlobby_ = 0;

static const wchar_t TITLE_OF_TOUCHHOOKS2TUIO_WINDOW [] = TEXT( "TouchHooks2Tuio" );
static const DWORD TITLE_LENGTH_FOR_TOUCHHOOKS2TUIO = 15;
static unsigned long pidTouchHooks2Tuio_ = 0;

WindowUtility::WindowUtility()
{
}

WindowUtility::~WindowUtility()
{
}

static void debugPrintPIDs( DWORD processId, DWORD windowProcessId )
{
    std::cout << "(processId, windowProcessId) = (" 
              << processId << ", " 
              << windowProcessId << ")\n";
}

static BOOL CALLBACK getBigBlobbyPid( HWND hwnd, LPARAM id )
{
    DWORD dwTextLength;
    dwTextLength = GetWindowTextLength( hwnd );
    
    if( dwTextLength == TITLE_LENGTH_FOR_BIGBLOBBY ) {
        DWORD dwBufferSize = TITLE_LENGTH_FOR_BIGBLOBBY + 1;
        LPWSTR title;
        title = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, dwBufferSize * sizeof( WCHAR ) );

        if( title != NULL && GetWindowText( hwnd, title, dwBufferSize ) ) {
            if( wcscmp( title, TITLE_OF_BIGBLOBBY_WINDOW ) == 0 ) {
                //std::wcout << title << " was found.\n";
                DWORD windowProcessId = NULL;

                if( GetWindowThreadProcessId( hwnd, &windowProcessId ) ) {
                    //std::cout << "pid = " << windowProcessId << "\n";
                    pidBigBlobby_ = windowProcessId;
                    HeapFree( GetProcessHeap(), 0, (LPVOID)title );
                    return FALSE; // Stop the enumeration.
                }
            }
        }
        HeapFree( GetProcessHeap(), 0, (LPVOID)title );
    }
    return TRUE; // Continue the enumeration.
}

static BOOL CALLBACK getTouchHooks2TuioPid( HWND hwnd, LPARAM id )
{
    DWORD dwTextLength;
    dwTextLength = GetWindowTextLength( hwnd );
    
    if( dwTextLength == TITLE_LENGTH_FOR_TOUCHHOOKS2TUIO ) {
        DWORD dwBufferSize = TITLE_LENGTH_FOR_TOUCHHOOKS2TUIO + 1;
        LPWSTR title;
        title = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, dwBufferSize * sizeof( WCHAR ) );

        if( title != NULL && GetWindowText( hwnd, title, dwBufferSize ) ) {
            if( wcscmp( title, TITLE_OF_TOUCHHOOKS2TUIO_WINDOW ) == 0 ) {
                std::wcout << title << " was found.\n";
                DWORD windowProcessId = NULL;

                if( GetWindowThreadProcessId( hwnd, &windowProcessId ) ) {
                    std::cout << "pid = " << windowProcessId << "\n";
                    pidTouchHooks2Tuio_ = windowProcessId;
                    HeapFree( GetProcessHeap(), 0, (LPVOID)title );
                    return FALSE; // Stop the enumeration.
                }
            }
        }
        HeapFree( GetProcessHeap(), 0, (LPVOID)title );
    }
    return TRUE; // Continue the enumeration.
}

/***************************************************************************//**
If BigBlobby is not open, zero will be returned instead of the Process ID.
*******************************************************************************/
unsigned long WindowUtility::getPidForBigBlobbyIfAlreadyOpen()
{
    long int pid = 0;
    pidBigBlobby_ = 0; 
    EnumWindows( getBigBlobbyPid, reinterpret_cast<LPARAM>(&pid) );
    return pidBigBlobby_;
}

/***************************************************************************//**
If TouchHooks2Tuio is not open, zero will be returned instead of the Process ID.
*******************************************************************************/
unsigned long WindowUtility::getPidForTouchHooks2TuioIfAlreadyOpen()
{
    long int pid = 0;
    pidTouchHooks2Tuio_ = 0; 
    EnumWindows( getTouchHooks2TuioPid, reinterpret_cast<LPARAM>(&pid) );
    return pidTouchHooks2Tuio_;
}

static void toggle( HWND hwnd )
{
    BOOL isMinimized = ::IsIconic( hwnd );
    //std::cout << "isMinimized = " << isMinimized << "\n";

    if( isMinimized ) {
        BOOL showWindowRestoreOk = ::ShowWindow( hwnd, SW_RESTORE );
        //std::cout << "showWindowRestoreOk = " << showWindowRestoreOk << "\n";

        int nCmdShow = SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW;
        BOOL setWindowPosTopOk = ::SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, nCmdShow );
        //std::cout << "setWindowPosTopOk = " << setWindowPosTopOk << "\n";
    }
    else {
        BOOL showWindowMinimizeOk = ::ShowWindow( hwnd, SW_MINIMIZE );
        //std::cout << "showWindowMinimizeOk = " << showWindowMinimizeOk << "\n";
    }
}

static BOOL CALLBACK toggleWindow( HWND hwnd, LPARAM lParam )
{
    DWORD processId = *reinterpret_cast<WORD*>(lParam);
    DWORD windowProcessId = NULL;
    ::GetWindowThreadProcessId( hwnd, &windowProcessId );
    //debugPrintPIDs( processId, windowProcessId );
   
    if( windowProcessId == processId ) {
        BOOL windowVisible = IsWindowVisible( hwnd );
        //std::cout << "windowVisible = " << windowVisible << "\n";

        if( windowVisible ) {
            toggle( hwnd );
            return FALSE; // End the enumeration.
        }
   }
   return TRUE; // Continue the enumeration.
}

static void hideWindowIfOpen( HWND hwnd )
{
    BOOL isMinimized = ::IsIconic( hwnd );
    //std::cout << "isMinimized = " << isMinimized << "\n";

    if( !isMinimized ) {
        BOOL showWindowMinimizeOk = ::ShowWindow( hwnd, SW_MINIMIZE );
        //std::cout << "showWindowMinimizeOk = " << showWindowMinimizeOk << "\n";
    }
}

static BOOL CALLBACK hideWindow( HWND hwnd, LPARAM lParam )
{
    DWORD processId = *reinterpret_cast<WORD*>(lParam);
    DWORD windowProcessId = NULL;
    ::GetWindowThreadProcessId( hwnd, &windowProcessId );
    //debugPrintPIDs( processId, windowProcessId );
   
    if( windowProcessId == processId ) {
        BOOL windowVisible = IsWindowVisible( hwnd );
        //std::cout << "windowVisible = " << windowVisible << "\n";

        if( windowVisible ) {
            hideWindowIfOpen( hwnd );
            return FALSE; // End the enumeration.
        }
   }
   return TRUE; // Continue the enumeration.
}

/*
 * Finds the visible window associated with the pid (process ID)and then checks 
 * if it is iconified (minimized to the taskbar).  If the window is already 
 * minimized, it is pulled out of the taskbar and restored to its previous 
 * position on the desktop.  If instead the window is already out on the 
 * desktop, then it is minimized to the taskbar.
 */
void WindowUtility::toggleWindowVisibility( _PROCESS_INFORMATION * processInfo )
{
    if( processInfo != NULL ) {
        unsigned long pid = processInfo->dwProcessId;
        //std::cout << "\ntoggleWindowVisibility() pid = " << pid << "\n\n";

        if( pid != 0 ) {
            EnumWindows( toggleWindow, reinterpret_cast<LPARAM>(&pid) );
        }
    }
}

void WindowUtility::hideWindowIfVisible( _PROCESS_INFORMATION * processInfo )
{
    if( processInfo != NULL ) {
        unsigned long pid = processInfo->dwProcessId;
        //std::cout << "\nhideWindow() pid = " << pid << "\n\n";

        if( pid != 0 ) {
            EnumWindows( hideWindow, reinterpret_cast<LPARAM>(&pid) );
        }
    }
}

static BOOL CALLBACK moveWindowToTopAndRestore( HWND hwnd, LPARAM lParam )
{
    DWORD processId = *reinterpret_cast<WORD*>(lParam);
    DWORD windowProcessId = NULL;
    ::GetWindowThreadProcessId( hwnd, &windowProcessId );
    //debugPrintPIDs( processId, windowProcessId );
   
    if( windowProcessId == processId ) {
        BOOL windowVisible = IsWindowVisible( hwnd );
        //std::cout << "windowVisible = " << windowVisible << "\n";

        if( windowVisible ) {
            int nCmdShow = SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW;
            BOOL setWindowPosTopMostOk = ::SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, nCmdShow );
            BOOL setWindowPosTopOk = ::SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, nCmdShow );
            BOOL showWindowRestoreOk = ::ShowWindow( hwnd, SW_RESTORE );
            //debugPrintPIDs( processId, windowProcessId );
            //std::cout << "setWindowPosTopMostOk = " << setWindowPosTopMostOk << "\n";
            //std::cout << "setWindowPosTopOk = " << setWindowPosTopMostOk << "\n";
            //std::cout << "showWindowRestoreOk = " << showWindowRestoreOk << "\n";
            return FALSE; // End the enumeration.
        }
   }
   return TRUE; // Continue the enumeration.
}

void WindowUtility::focusOnWindow( _PROCESS_INFORMATION * processInfo )
{
    if( processInfo != NULL ) {
        unsigned long pid = processInfo->dwProcessId;
        //std::cout << "\nfocusOnWindow() pid = " << pid << "\n\n";

        if( pid != 0 ) {
            EnumWindows( moveWindowToTopAndRestore, reinterpret_cast<LPARAM>(&pid) );
        }
    }
}

static BOOL CALLBACK moveWindowToBottom( HWND hwnd, LPARAM lParam )
{
    DWORD processId = *reinterpret_cast<WORD*>(lParam);
    DWORD windowProcessId = NULL;
    ::GetWindowThreadProcessId( hwnd, &windowProcessId );
    //std::cout << "(processId, windowProcessId) = (" << processId << ", " << windowProcessId << ")\n";
   
    if( windowProcessId == processId ) {
        BOOL windowVisible = IsWindowVisible( hwnd );
        //std::cout << "windowVisible = " << windowVisible << "\n";

        if( windowVisible ) {
            int nCmdShow = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
            BOOL ok1 = ::SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, nCmdShow );
            BOOL ok2 = ::SetWindowPos( hwnd, HWND_BOTTOM, 0, 0, 0, 0, nCmdShow );
            //printf( "WindowFunctions::moveWindowToBottom() SetWindowPos( hwnd, HWND_BOTTOM, ... ) returned %d.\n", ok2 );
            return FALSE; // End the enumeration.
        }
   }
   return TRUE; // Continue the enumeration.
}

void WindowUtility::moveWindowToBottomZOrder( _PROCESS_INFORMATION * processInfo )
{
    if( processInfo != NULL ) {
        unsigned long pid = processInfo->dwProcessId;
        //std::cout << "pid = " << pid << "\n";

        if( pid != 0 ) {
            EnumWindows( moveWindowToBottom, reinterpret_cast<LPARAM>(&pid) );
        }
    }
}

void WindowUtility::placeAllOtherWindowsInTaskbar()
{
    HWND lHwnd = FindWindow( TEXT("Shell_TrayWnd"), NULL );
    SendMessage( lHwnd, WM_COMMAND, 419, 0 );
}

void WindowUtility::moveMouse( long int x, long int y )
{
    double fScreenWidth  = ::GetSystemMetrics( SM_CXSCREEN ) - 1; 
    double fScreenHeight = ::GetSystemMetrics( SM_CYSCREEN ) - 1; 
    double fx = x * (65535.0f / fScreenWidth);
    double fy = y * (65535.0f / fScreenHeight);

    //std::cout << "fScreenWidth = " << fScreenWidth << "\n"
    //          << "fScreenHeight = " << fScreenHeight << "\n"
    //          << "fx = " << fx << "\n"
    //          << "fy = " << fy << "\n";

    INPUT  input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    input.mi.dx = LONG(fx);
    input.mi.dy = LONG(fy);
    ::SendInput( 1, &input, sizeof( INPUT ) );
}

void WindowUtility::leftClickMouse()
{
    INPUT input = { 0 };

    // left down 
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput( 1, &input, sizeof( INPUT ) );

    // left up
    ::ZeroMemory( &input, sizeof( INPUT ) );
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    ::SendInput( 1, &input, sizeof( INPUT ) );
}

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
