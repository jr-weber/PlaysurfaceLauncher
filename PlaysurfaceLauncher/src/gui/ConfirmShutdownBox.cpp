/*******************************************************************************
ConfirmShutdownBox

PURPOSE: Draws the Confirm Shutdown buttons seen in ConfirmShutdownBox.ui and
         provides a means of listening for a touch or click on a button.

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
#include "gui/ConfirmShutdownBox.h"
#include "gui/CentralWidget.h"
#include <Windows.h>
#include <iostream>

using gui::ConfirmShutdownBox;

ConfirmShutdownBox::ConfirmShutdownBox( gui::CentralWidget * parent ) : 
  QWidget( NULL, Qt::Window | Qt::WindowStaysOnTopHint| Qt::FramelessWindowHint ),
  ui_(),
  parent_( parent )
{
    ui_.setupUi( this );
    connectButtons();
}

ConfirmShutdownBox::~ConfirmShutdownBox()
{
}

void ConfirmShutdownBox::connectButtons()
{
    connect( ui_.pushButton_shutdownComputer, 
             SIGNAL( pressed() ),
             parent_, 
             SLOT( onShutdownComputerRequest() ) );

    connect( ui_.pushButton_shutdownPlaysurfaceLauncher, 
             SIGNAL( pressed() ),
             parent_, 
             SLOT( onShutdownPlaysurfaceRequest() ) );

    connect( ui_.pushButton_cancel, 
             SIGNAL( pressed() ),
             parent_, 
             SLOT( onCancelShutdownRequest() ) );
}

void ConfirmShutdownBox::show()
{
    QWidget::show();
    raise();
    activateWindow();

    HWND handle = (HWND)effectiveWinId();
    BOOL ok = ::SetWindowPos( handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
    ::ShowWindow( handle, SW_RESTORE );
    //printf( "\ConfirmShutdownBox::show() SetWindowPos( handle, HWND_TOPMOST ) returned %d.\n", ok );
}

void ConfirmShutdownBox::close()
{
    QWidget::close();
}

bool ConfirmShutdownBox::isPointOnShutdownComputerButton( double x, double y )
{
    double xBox = this->x(),
           yBox = this->y(),
           xButton = ui_.pushButton_shutdownComputer->x(),
           yButton = ui_.pushButton_shutdownComputer->y(),
           widthButton = ui_.pushButton_shutdownComputer->width(),
           heightButton = ui_.pushButton_shutdownComputer->height(),
           xMin = xBox + xButton,
           xMax = xMin + widthButton,
           yMin = yBox + yButton,
           yMax = yMin + heightButton;

    //std::cout << "\ntouch(x, y) = (" << x << ", " << y << ")\n\n"
    //          << "x( min, max ) = (" << xMin << ", " << xMax << ")\n"
    //          << "y( min, max ) = (" << yMin << ", " << yMax << ")\n\n"
    //          << "box(x, y) = (" << xBox << ", " << yBox << ")\n"
    //          << "button(x, y, w, h) = " 
    //          << xButton << ", " << yButton << ", " 
    //          << widthButton << ", " << heightButton << ")\n";

    return (x >= xMin && x <= xMax) 
        && (y >= yMin && y <= yMax);
}

bool ConfirmShutdownBox::isPointOnShutdownPlaysurfaceButton( double x, double y )
{
    double xBox = this->x(),
           yBox = this->y(),
           xButton = ui_.pushButton_shutdownPlaysurfaceLauncher->x(),
           yButton = ui_.pushButton_shutdownPlaysurfaceLauncher->y(),
           widthButton = ui_.pushButton_shutdownPlaysurfaceLauncher->width(),
           heightButton = ui_.pushButton_shutdownPlaysurfaceLauncher->height(),
           xMin = xBox + xButton,
           xMax = xMin + widthButton,
           yMin = yBox + yButton,
           yMax = yMin + heightButton;

    //std::cout << "\ntouch(x, y) = (" << x << ", " << y << ")\n\n"
    //          << "x( min, max ) = (" << xMin << ", " << xMax << ")\n"
    //          << "y( min, max ) = (" << yMin << ", " << yMax << ")\n\n"
    //          << "box(x, y) = (" << xBox << ", " << yBox << ")\n"
    //          << "button(x, y, w, h) = " 
    //          << xButton << ", " << yButton << ", " 
    //          << widthButton << ", " << heightButton << ")\n";

    return (x >= xMin && x <= xMax) 
        && (y >= yMin && y <= yMax);

    return false;
}

bool ConfirmShutdownBox::isPointOnCancelButton( double x, double y )
{
    double xBox = this->x(),
           yBox = this->y(),
           xButton = ui_.pushButton_cancel->x(),
           yButton = ui_.pushButton_cancel->y(),
           widthButton = ui_.pushButton_cancel->width(),
           heightButton = ui_.pushButton_cancel->height(),
           xMin = xBox + xButton,
           xMax = xMin + widthButton,
           yMin = yBox + yButton,
           yMax = yMin + heightButton;

    //std::cout << "\ntouch(x, y) = (" << x << ", " << y << ")\n\n"
    //          << "x( min, max ) = (" << xMin << ", " << xMax << ")\n"
    //          << "y( min, max ) = (" << yMin << ", " << yMax << ")\n\n"
    //          << "box(x, y) = (" << xBox << ", " << yBox << ")\n"
    //          << "button(x, y, w, h) = " 
    //          << xButton << ", " << yButton << ", " 
    //          << widthButton << ", " << heightButton << ")\n";

    return (x >= xMin && x <= xMax) 
        && (y >= yMin && y <= yMax);

    return false;
}
