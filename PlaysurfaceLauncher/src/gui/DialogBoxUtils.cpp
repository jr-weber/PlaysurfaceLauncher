/*******************************************************************************
DialogBoxUtils

PURPOSE: Provides functions for opening a dialog box to report an exception.

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
#include "gui/DialogBoxUtils.h"
#include <QMessageBox>
#include <QString>
#include <iostream>

using gui::DialogBoxUtils;

DialogBoxUtils::DialogBoxUtils()
{
    
}

DialogBoxUtils::~DialogBoxUtils()
{
}

void DialogBoxUtils::warnUser( exceptions::FileNotFoundException e )
{
    openWarningMessageBox( "File Not Found", e.getMessage() );
}

void DialogBoxUtils::warnUser( exceptions::FileNotReadableException e )
{
    openWarningMessageBox( "File Not Readable", e.getMessage() );
}

void DialogBoxUtils::warnUserOnBigBlobby( exceptions::FileNotFoundException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "Touch detection will not work without BigBlobby running.";
    openWarningMessageBox( "File Not Found", msg );
}

void DialogBoxUtils::warnUserOnBigBlobby( exceptions::FileNotReadableException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "Touch detection will not work without BigBlobby running.";
    openWarningMessageBox( "File Not Readable", msg );
}

void DialogBoxUtils::warnUserOnTouchHooks2Tuio( exceptions::FileNotFoundException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "TUIO-based touch detection will not work without TouchHooks2Tuio running.";
    openWarningMessageBox( "File Not Found", msg );
}

void DialogBoxUtils::warnUserOnTouchHooks2Tuio( exceptions::FileNotReadableException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "TUIO-based touch detection will not work without TouchHooks2Tuio running.";
    openWarningMessageBox( "File Not Readable", msg );
}

void DialogBoxUtils::warnUserOnFlashPlayer( exceptions::FileNotFoundException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "Flash applications will not run with without the FlashPlayer.";
    openWarningMessageBox( "File Not Found", msg );
}

void DialogBoxUtils::warnUserOnFlashPlayer( exceptions::FileNotReadableException e )
{
    QString msg = e.getMessage() + "\n";
    msg += "Flash applications will not run with without the FlashPlayer.";
    openWarningMessageBox( "File Not Readable", msg );
}

void DialogBoxUtils::openWarningMessageBox( const QString & title, 
                                            const QString & message )
{
    QMessageBox messageBox;
    messageBox.setWindowTitle( title );
    messageBox.setText( message );
    messageBox.setIcon( QMessageBox::Warning );
    messageBox.exec();
}

void DialogBoxUtils::warnUser( std::vector<exceptions::IOException> errors )
{
    QString msg = "";

    for( int i = 0; i < errors.size(); ++i ) {
        msg += errors.at( i ).getMessage() + "\n";
    }
    if( msg.size() > 0 ) {
        openWarningMessageBox( "File Not Found", msg );
    }
}

void DialogBoxUtils::warnUserBigBlobbyCrashed( QProcess::ProcessError processError )
{
    QString msg = "The Touch Detector, BigBlobby.exe, has exited.\n\n";
    msg += "Process Error: ";

    switch( processError ) {
        case QProcess::FailedToStart:  msg += "Failed to Start";  break;
        case QProcess::Crashed:        msg += "Crashed";          break;
        case QProcess::Timedout:       msg += "Timed Out";        break;
        case QProcess::WriteError:     msg += "Write Error";      break;
        case QProcess::ReadError:      msg += "ReadError";        break;
        case QProcess::UnknownError:   msg += "Unknown";          break;
        default:                       msg += "unknown";          break;
    }
    msg += "\n\nTouch detection will not work without BigBlobby running.\n"
           "After closing this dialog box, you can try restarting BigBlobby\n"
           "by using a mouse to click on the upper-left icon (white circles\n"
           "on a black background).  Clicking or touching the same icon\n"
           "again can then be used to hide BigBlobby.";

    openWarningMessageBox( "Touch Detector Error", msg );
}

void DialogBoxUtils::warnUserTouchHooks2TuioCrashed( QProcess::ProcessError processError )
{
    QString msg = "The Touch Converter, TouchHooks2Tuio.exe, has exited.\n\n";
    msg += "Process Error: ";

    switch( processError ) {
        case QProcess::FailedToStart:  msg += "Failed to Start";  break;
        case QProcess::Crashed:        msg += "Crashed";          break;
        case QProcess::Timedout:       msg += "Timed Out";        break;
        case QProcess::WriteError:     msg += "Write Error";      break;
        case QProcess::ReadError:      msg += "ReadError";        break;
        case QProcess::UnknownError:   msg += "Unknown";          break;
        default:                       msg += "unknown";          break;
    }
    msg += "\n\nTUIO-based touch will not work without TouchHooks2Tuio running.\n"
           "After closing this dialog box, you can try restarting TouchHooks2Tuio\n"
           "by using a mouse to click on the upper-left icon (white circles\n"
           "on a black background).  Clicking or touching the same icon\n"
           "again can then be used to hide TouchHooks2Tuio.";

    openWarningMessageBox( "Touch Detector Error", msg );
}

void DialogBoxUtils::warnUserBigBlobbyClosed()
{
    QString msg = 
        "The touch detector, BigBlobby.exe, has closed.  Touch\n"
        "detection will not work without BigBlobby running.\n\n"

        "After closing this dialog box, please restart BigBlobby\n"
        "by using a mouse to click on the upper-left icon (the\n"
        "one with white circles on a black background).\n\n"

        "After restarting BigBlobby, you can click or touch the\n"
        "icon again to hide BigBlobby without quitting.  Touching\n"
        "the icon repeatedly will show or hide BigBlobby.  Hiding\n"
        "BigBlobby is fine, but do not make it quit.";

    openWarningMessageBox( "Touch Detector Closed", msg );
}

void DialogBoxUtils::warnUserTouchHooks2TuioClosed()
{
    QString msg = 
        "The touch to TUIO converter, TouchHooks2Tuio.exe, has closed.\n"
        "Touch detection will not work without TouchHooks2Tuio running.\n\n"

        "After closing this dialog box, please restart TouchHooks2Tuio\n"
        "by using a mouse to click on the upper-left icon (the\n"
        "one with circles on a green and gray background).\n\n"

        "After restarting TouchHooks2Tuio, you can click or touch the\n"
        "icon again to hide TouchHooks2Tuio without quitting.  Touching\n"
        "the icon repeatedly will show or hide TouchHooks2Tuio.  Hiding\n"
        "TouchHooks2Tuio is fine, but do not make it quit.";

    openWarningMessageBox( "Touch Detector Closed", msg );
}

void DialogBoxUtils::warnUserBigBlobbyFailedToStart()
{
    QString msg = 
        "The Touch Detector, BigBlobby.exe, failed to start.\n"
        "There may be a Windows permissions issue.\n";
        "Touch detection will not work without BigBlobby running.";

    openWarningMessageBox( "Touch Detector Failed to Start", msg );
}

void DialogBoxUtils::warnUserTouchHooks2TuioFailedToStart()
{
    QString msg = 
        "The touch to TUIO converter, TouchHooks2Tuio.exe, failed\n"
        "to start.  There may be a Windows permissions issue.\n";
        "Touch detection will not work without TouchHooks2Tuio running.";

    openWarningMessageBox( "Touch Detector Failed to Start", msg );
}

void DialogBoxUtils::warnUserMultitouchAppFailedToStart()
{
    QString msg = "The multi-touch app failed to start.\n\n";
    msg += "There may be a Windows permissions issue,\n";
    msg += "or the multitouch app has been deleted.\n";
    msg += "Try using the Reload icon to check which\n";
    msg += "multitouch apps are currently available.\n";

    openWarningMessageBox( "Launch Failure", msg );
}

void DialogBoxUtils::warnUserMultitouchAppCrashed( QProcess::ProcessError processError )
{
    QString msg = "The multi-touch app crashed.\n\n";
    msg += "Process Error: ";

    switch( processError ) {
        case QProcess::FailedToStart:  msg += "Failed to Start";  break;
        case QProcess::Crashed:        msg += "Crashed";          break;
        case QProcess::Timedout:       msg += "Timed Out";        break;
        case QProcess::WriteError:     msg += "Write Error";      break;
        case QProcess::ReadError:      msg += "ReadError";        break;
        case QProcess::UnknownError:   msg += "Unknown";          break;
        default:                       msg += "unknown";          break;
    }
    openWarningMessageBox( "Multi-touch App Error", msg );
}
